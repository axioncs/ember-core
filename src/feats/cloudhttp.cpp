// SPDX-License-Identifier: AGPL-3.0-only

#include "cloudhttp.hpp"

#include "../ascii.hpp"
#include "../log.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>

namespace
{
	// Steam's ufs client sends one save at a time, but a save can be a few MB and
	// the header block has to be bounded before we look at Content-Length.
	constexpr size_t kMaxHeadBytes = 64 * 1024;
	constexpr size_t kMaxBodyBytes = 256ull * 1024 * 1024;

	const char* kNoStore =
		"Cache-Control: no-store\r\n"
		"Access-Control-Allow-Origin: *\r\n";

	std::string statusLine(const int code)
	{
		switch (code)
		{
			case 200: return "HTTP/1.1 200 OK\r\n";
			case 400: return "HTTP/1.1 400 Bad Request\r\n";
			case 401: return "HTTP/1.1 401 Unauthorized\r\n";
			case 404: return "HTTP/1.1 404 Not Found\r\n";
			case 405: return "HTTP/1.1 405 Method Not Allowed\r\n";
			case 500: return "HTTP/1.1 500 Internal Server Error\r\n";
			default:  return "HTTP/1.1 400 Bad Request\r\n";
		}
	}

	bool sendAll(const int fd, const char* data, const size_t len)
	{
		size_t sent = 0;

		while (sent < len)
		{
			const auto n = send(fd, data + sent, len - sent, MSG_NOSIGNAL);
			if (n <= 0)
			{
				return false;
			}
			sent += static_cast<size_t>(n);
		}

		return true;
	}

	bool sendAll(const int fd, const std::string& text)
	{
		return sendAll(fd, text.data(), text.size());
	}

	void sendError(const int fd, const int code, const std::string& message)
	{
		const auto body = message + "\n";

		std::ostringstream response;
		response << statusLine(code)
			<< "Content-Type: text/plain\r\n"
			<< "Content-Length: " << body.size() << "\r\n"
			<< "Connection: close\r\n" << kNoStore << "\r\n"
			<< body;

		sendAll(fd, response.str());
	}

	std::string toLower(std::string value)
	{
		// The ASCII helper, not the locale-aware ctype call: this runs inside
		// the injected library, where the C locale is not installed yet and
		// that call can read uninitialised global state. Every other
		// lowercasing path in the tree already uses this helper.
		std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
			return static_cast<char>(Ascii::toLower(c));
		});
		return value;
	}

	// Case-insensitive header lookup over the raw head block.
	std::string header(const std::string& raw, const std::string& name)
	{
		const auto wanted = toLower(name);

		std::istringstream stream(raw);
		std::string line;

		while (std::getline(stream, line))
		{
			if (!line.empty() && line.back() == '\r')
			{
				line.pop_back();
			}

			const auto colon = line.find(':');
			if (colon == std::string::npos)
			{
				continue;
			}

			if (toLower(line.substr(0, colon)) != wanted)
			{
				continue;
			}

			auto value = line.substr(colon + 1);
			const auto first = value.find_first_not_of(" \t");
			return (first == std::string::npos) ? std::string() : value.substr(first);
		}

		return {};
	}

	int hexValue(const char c)
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		return -1;
	}

	bool numeric(const std::string& text)
	{
		if (text.empty())
		{
			return false;
		}

		for (const char c : text)
		{
			if (c < '0' || c > '9')
			{
				return false;
			}
		}

		return true;
	}
}

namespace CloudHTTP
{
	std::string urlDecode(const std::string& value)
	{
		std::string out;
		out.reserve(value.size());

		for (size_t i = 0; i < value.size(); ++i)
		{
			if (value[i] == '%' && i + 2 < value.size())
			{
				const int high = hexValue(value[i + 1]);
				const int low = hexValue(value[i + 2]);

				if (high >= 0 && low >= 0)
				{
					out.push_back(static_cast<char>((high << 4) | low));
					i += 2;
					continue;
				}
			}

			// A literal '+' is a space in a query, but our paths never carry a
			// query, and a save named "a+b" must survive. Leave it alone.
			out.push_back(value[i]);
		}

		return out;
	}

	bool parseHead(const std::string& raw, Request& out, std::string& error)
	{
		const auto lineEnd = raw.find("\r\n");
		const auto requestLine = raw.substr(0, lineEnd == std::string::npos ? raw.size() : lineEnd);

		std::istringstream stream(requestLine);
		std::string method;
		std::string target;

		if (!(stream >> method >> target))
		{
			error = "malformed request line";
			return false;
		}

		out.path = target;

		if (method == "GET")
		{
			out.method = Method::Get;
		}
		else if (method == "PUT")
		{
			out.method = Method::Put;
		}
		else
		{
			out.method = Method::Unsupported;
			error = "only GET and PUT are supported";
			return false;
		}

		out.contentLength = 0;

		const auto lengthText = header(raw, "content-length");
		if (!lengthText.empty())
		{
			if (!numeric(lengthText))
			{
				error = "invalid Content-Length";
				return false;
			}

			try
			{
				out.contentLength = static_cast<size_t>(std::stoull(lengthText));
			}
			catch (...)
			{
				error = "invalid Content-Length";
				return false;
			}
		}

		if (out.contentLength > kMaxBodyBytes)
		{
			error = "body too large";
			return false;
		}

		return true;
	}

	bool decodePath(const Ember::CloudEndpoint& endpoint, const std::string& path,
		uint32_t& accountId, uint32_t& appId, std::string& savePath)
	{
		// /ember/cloud/<account>/<app>/<token>/<encoded save path>
		const std::string prefix = "/ember/cloud/";

		if (path.compare(0, prefix.size(), prefix) != 0)
		{
			return false;
		}

		std::string remaining = path.substr(prefix.size());

		const auto cut = [&remaining](std::string& out) {
			const auto slash = remaining.find('/');
			if (slash == std::string::npos)
			{
				return false;
			}

			out = remaining.substr(0, slash);
			remaining = remaining.substr(slash + 1);
			return true;
		};

		std::string accountText;
		std::string appText;
		std::string token;

		if (!cut(accountText) || !cut(appText) || !cut(token))
		{
			return false;
		}

		savePath = urlDecode(remaining);

		// The token is what stops another local process from using the port.
		if (token != endpoint.token)
		{
			return false;
		}

		if (!numeric(accountText) || !numeric(appText))
		{
			return false;
		}

		try
		{
			accountId = static_cast<uint32_t>(std::stoul(accountText));
			appId = static_cast<uint32_t>(std::stoul(appText));
		}
		catch (...)
		{
			return false;
		}

		return CloudStore::isSafePath(savePath);
	}

	bool Server::start(const std::filesystem::path& root, const Ember::CloudEndpoint& endpoint)
	{
		root_ = root;
		endpoint_ = endpoint;

		listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
		if (listenFd_ < 0)
		{
			g_pLog->warn("Cloud: cannot create a socket (%s)\n", strerror(errno));
			return false;
		}

		int one = 1;
		setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

		sockaddr_in address = {};
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		address.sin_port = 0;	// Let the kernel pick.

		if (bind(listenFd_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0)
		{
			g_pLog->warn("Cloud: cannot bind a loopback port (%s)\n", strerror(errno));
			close(listenFd_);
			listenFd_ = -1;
			return false;
		}

		if (listen(listenFd_, 8) != 0)
		{
			g_pLog->warn("Cloud: cannot listen (%s)\n", strerror(errno));
			close(listenFd_);
			listenFd_ = -1;
			return false;
		}

		socklen_t length = sizeof(address);
		if (getsockname(listenFd_, reinterpret_cast<sockaddr*>(&address), &length) != 0)
		{
			g_pLog->warn("Cloud: cannot read the bound port\n");
			close(listenFd_);
			listenFd_ = -1;
			return false;
		}

		endpoint_.port = ntohs(address.sin_port);

		running_ = true;
		thread_ = std::thread(&Server::serveLoop, this);

		g_pLog->info("Cloud: listening on %s\n", endpoint_.host().c_str());
		return true;
	}

	void Server::stop()
	{
		running_ = false;

		// Closing the listen socket is what unblocks accept().
		if (listenFd_ >= 0)
		{
			shutdown(listenFd_, SHUT_RDWR);
			close(listenFd_);
			listenFd_ = -1;
		}

		if (thread_.joinable())
		{
			thread_.join();
		}
	}

	void Server::serveLoop()
	{
		while (running_)
		{
			const int fd = accept(listenFd_, nullptr, nullptr);
			if (fd < 0)
			{
				if (!running_)
				{
					return;
				}
				continue;
			}

			handle(fd);
			close(fd);
		}
	}

	void Server::handle(int fd)
	{
		std::string raw;
		char buffer[4096];
		size_t headEnd = std::string::npos;

		// Read until the blank line that ends the head.
		while (headEnd == std::string::npos)
		{
			if (raw.size() > kMaxHeadBytes)
			{
				sendError(fd, 400, "head too large");
				return;
			}

			const auto n = recv(fd, buffer, sizeof(buffer), 0);
			if (n <= 0)
			{
				return;
			}

			raw.append(buffer, static_cast<size_t>(n));

			const auto crlf = raw.find("\r\n\r\n");
			const auto lf = raw.find("\n\n");

			if (crlf != std::string::npos)
			{
				headEnd = crlf + 4;
			}
			else if (lf != std::string::npos)
			{
				headEnd = lf + 2;
			}
		}

		Request request;
		std::string error;

		if (!parseHead(raw.substr(0, headEnd), request, error))
		{
			sendError(fd, 400, error);
			return;
		}

		uint32_t accountId = 0;
		uint32_t appId = 0;
		std::string savePath;

		if (!decodePath(endpoint_, request.path, accountId, appId, savePath))
		{
			// A token mismatch and a bad path are deliberately indistinguishable:
			// a caller that guessed one must not learn it guessed right.
			sendError(fd, 404, "no such save");
			return;
		}

		if (request.method == Method::Put)
		{
			std::string body = raw.substr(headEnd);

			while (body.size() < request.contentLength)
			{
				const auto want = std::min(sizeof(buffer), request.contentLength - body.size());
				const auto n = recv(fd, buffer, want, 0);

				if (n <= 0)
				{
					sendError(fd, 400, "truncated body");
					return;
				}

				body.append(buffer, static_cast<size_t>(n));
			}

			body.resize(request.contentLength);

			CloudStore::Entry entry;
			if (!CloudStore::put(root_, accountId, appId, savePath, body, entry))
			{
				g_pLog->warn("Cloud: PUT %u/%u rejected (%s)\n", accountId, appId, savePath.c_str());
				sendError(fd, 400, "cannot store save");
				return;
			}

			g_pLog->info("Cloud: PUT %u/%u %s (%zu bytes, sha %s)\n",
				accountId, appId, savePath.c_str(), body.size(), entry.sha.c_str());

			const auto response = std::string("HTTP/1.1 200 OK\r\n")
				+ "Content-Length: 0\r\nConnection: close\r\n" + kNoStore + "\r\n";

			sendAll(fd, response);
			return;
		}

		std::string data;

		if (!CloudStore::get(root_, accountId, appId, savePath, data))
		{
			g_pLog->warn("Cloud: GET %u/%u missing (%s)\n", accountId, appId, savePath.c_str());
			sendError(fd, 404, "no such save");
			return;
		}

		std::ostringstream response;
		response << statusLine(200)
			<< "Content-Type: application/octet-stream\r\n"
			<< "Content-Length: " << data.size() << "\r\n"
			<< "Connection: close\r\n" << kNoStore << "\r\n";

		if (!sendAll(fd, response.str()))
		{
			return;
		}

		sendAll(fd, data.data(), data.size());

		g_pLog->info("Cloud: GET %u/%u %s (%zu bytes)\n", accountId, appId, savePath.c_str(), data.size());
	}
}
