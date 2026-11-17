// SPDX-License-Identifier: AGPL-3.0-only
//
// Host test for the loopback HTTP server. Real sockets, real store: this is the
// layer where a wiring mistake would otherwise only show up inside Steam.

#include "../src/feats/cloudhttp.hpp"
#include "../src/feats/cloudendpoint.hpp"
#include "../src/feats/cloudstore.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

static int failures = 0;

#define CHECK(cond)                                                          \
	do                                                                     \
	{                                                                      \
		if (!(cond))                                                       \
		{                                                                  \
			std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
			++failures;                                                    \
		}                                                                  \
	} while (0)

namespace
{
	struct Scratch
	{
		std::filesystem::path root;

		Scratch()
		{
			const char* tmp = std::getenv("TMPDIR");
			root = std::filesystem::path(tmp ? tmp : "/tmp") / "ember-cloud-http-test";
			std::error_code ec;
			std::filesystem::remove_all(root, ec);
			std::filesystem::create_directories(root, ec);
		}

		~Scratch()
		{
			std::error_code ec;
			std::filesystem::remove_all(root, ec);
		}
	};

	constexpr uint32_t kAccount = 76561198u;
	constexpr uint32_t kApp = 1091500u;

	struct Response
	{
		int status = 0;
		std::string body;
	};

	// Minimal HTTP client: send raw bytes, read to EOF, split status and body.
	Response request(const uint16_t port, const std::string& raw)
	{
		Response response;

		const int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0)
		{
			return response;
		}

		sockaddr_in address = {};
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		address.sin_port = htons(port);

		if (connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0)
		{
			close(fd);
			return response;
		}

		size_t sent = 0;
		while (sent < raw.size())
		{
			const auto n = send(fd, raw.data() + sent, raw.size() - sent, MSG_NOSIGNAL);
			if (n <= 0)
			{
				break;
			}
			sent += static_cast<size_t>(n);
		}

		std::string all;
		char buffer[4096];

		while (true)
		{
			const auto n = recv(fd, buffer, sizeof(buffer), 0);
			if (n <= 0)
			{
				break;
			}
			all.append(buffer, static_cast<size_t>(n));
		}

		close(fd);

		const auto statusEnd = all.find("\r\n");
		if (statusEnd == std::string::npos)
		{
			return response;
		}

		const auto statusLine = all.substr(0, statusEnd);
		std::sscanf(statusLine.c_str(), "HTTP/%*s %d", &response.status);

		const auto bodyStart = all.find("\r\n\r\n");
		if (bodyStart != std::string::npos)
		{
			response.body = all.substr(bodyStart + 4);
		}

		return response;
	}

	std::string getRequest(const std::string& path)
	{
		return "GET " + path + " HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n";
	}

	std::string putRequest(const std::string& path, const std::string& body)
	{
		return "PUT " + path + " HTTP/1.1\r\nHost: 127.0.0.1\r\nContent-Length: "
			+ std::to_string(body.size()) + "\r\nConnection: close\r\n\r\n" + body;
	}
}

static void testUrlDecode()
{
	CHECK(CloudHTTP::urlDecode("plain") == "plain");
	CHECK(CloudHTTP::urlDecode("a%20b") == "a b");
	CHECK(CloudHTTP::urlDecode("100%25") == "100%");
	// A literal '+' must survive: save names can contain it, and our paths never
	// carry a query string.
	CHECK(CloudHTTP::urlDecode("a+b") == "a+b");
	// Truncated escapes are passed through, not dropped.
	CHECK(CloudHTTP::urlDecode("a%2") == "a%2");
}

static void testParseHead()
{
	CloudHTTP::Request request;
	std::string error;

	CHECK(CloudHTTP::parseHead("GET /x HTTP/1.1\r\nHost: y\r\n\r\n", request, error));
	CHECK(request.method == CloudHTTP::Method::Get);
	CHECK(request.path == "/x");
	CHECK(request.contentLength == 0);

	CHECK(CloudHTTP::parseHead("PUT /y HTTP/1.1\r\nContent-Length: 42\r\n\r\n", request, error));
	CHECK(request.method == CloudHTTP::Method::Put);
	CHECK(request.contentLength == 42);

	CHECK(!CloudHTTP::parseHead("DELETE /y HTTP/1.1\r\n\r\n", request, error));
	CHECK(request.method == CloudHTTP::Method::Unsupported);

	CHECK(!CloudHTTP::parseHead("PUT /y HTTP/1.1\r\nContent-Length: abc\r\n\r\n", request, error));
	CHECK(!CloudHTTP::parseHead("garbage\r\n\r\n", request, error));

	// A negative length is not a number as far as we are concerned.
	CHECK(!CloudHTTP::parseHead("PUT /y HTTP/1.1\r\nContent-Length: -1\r\n\r\n", request, error));
}

static void testDecodePath()
{
	Ember::CloudEndpoint endpoint;
	endpoint.port = 45678;
	endpoint.token = "0123456789abcdef0123456789abcdef";

	uint32_t account = 0;
	uint32_t app = 0;
	std::string save;

	CHECK(CloudHTTP::decodePath(endpoint,
		"/ember/cloud/76561198/1091500/0123456789abcdef0123456789abcdef/save.dat",
		account, app, save));
	CHECK(account == kAccount);
	CHECK(app == kApp);
	CHECK(save == "save.dat");

	// Nested paths and percent-encoded separators.
	CHECK(CloudHTTP::decodePath(endpoint,
		"/ember/cloud/76561198/1091500/0123456789abcdef0123456789abcdef/profile%2Fsave.dat",
		account, app, save));
	CHECK(save == "profile/save.dat");

	// Wrong token.
	CHECK(!CloudHTTP::decodePath(endpoint,
		"/ember/cloud/76561198/1091500/deadbeef/save.dat", account, app, save));

	// Non-numeric ids.
	CHECK(!CloudHTTP::decodePath(endpoint,
		"/ember/cloud/abc/1091500/0123456789abcdef0123456789abcdef/save.dat", account, app, save));

	// Traversal, encoded or not.
	CHECK(!CloudHTTP::decodePath(endpoint,
		"/ember/cloud/76561198/1091500/0123456789abcdef0123456789abcdef/..%2F..%2Fetc%2Fpasswd",
		account, app, save));
	CHECK(!CloudHTTP::decodePath(endpoint,
		"/ember/cloud/76561198/1091500/0123456789abcdef0123456789abcdef//etc/passwd",
		account, app, save));

	// Not our prefix at all.
	CHECK(!CloudHTTP::decodePath(endpoint, "/download/1/2/save.dat", account, app, save));
}

static void testServerRoundTrip()
{
	Scratch scratch;

	Ember::CloudEndpoint endpoint;
	endpoint.token = "0123456789abcdef0123456789abcdef";

	CloudHTTP::Server server;
	if (!server.start(scratch.root, endpoint))
	{
		CHECK(false);
		return;
	}

	const auto port = server.endpoint().port;
	CHECK(port != 0);

	const auto base = "/ember/cloud/" + std::to_string(kAccount) + "/" + std::to_string(kApp)
		+ "/" + endpoint.token + "/";

	// PUT a save, then GET it back.
	{
		const std::string body = "save contents\x00with a NUL";
		const auto put = request(port, putRequest(base + "save.dat", body));
		CHECK(put.status == 200);

		const auto got = request(port, getRequest(base + "save.dat"));
		CHECK(got.status == 200);
		CHECK(got.body == body);
	}

	// Nested path, and a name needing percent-encoding.
	{
		const std::string body = "nested";
		const auto put = request(port, putRequest(base + "profile%2Fdeep%2Fsave.bin", body));
		CHECK(put.status == 200);

		const auto got = request(port, getRequest(base + "profile%2Fdeep%2Fsave.bin"));
		CHECK(got.status == 200);
		CHECK(got.body == body);
	}

	// The store really has it, under the decoded path.
	{
		const auto entries = CloudStore::list(scratch.root, kAccount, kApp);
		CHECK(entries.size() == 2);
	}

	// Missing file is a 404, not an empty 200.
	{
		const auto got = request(port, getRequest(base + "nope.dat"));
		CHECK(got.status == 404);
	}

	// A wrong token is indistinguishable from a missing file.
	{
		const std::string other = std::string(32, 'f');

		const auto got = request(port, getRequest("/ember/cloud/" + std::to_string(kAccount)
			+ "/" + std::to_string(kApp) + "/" + other + "/save.dat"));
		CHECK(got.status == 404);
	}

	// Traversal cannot escape the app directory.
	{
		const auto got = request(port, getRequest(base + "..%2F..%2F..%2Fetc%2Fpasswd"));
		CHECK(got.status == 404);
	}

	// A method we do not implement is refused, not silently accepted.
	{
		const auto got = request(port, "DELETE " + base + "save.dat HTTP/1.1\r\nConnection: close\r\n\r\n");
		CHECK(got.status == 400);
	}

	server.stop();
	CHECK(!server.running());
}

int main()
{
	testUrlDecode();
	testParseHead();
	testDecodePath();
	testServerRoundTrip();

	if (failures != 0)
	{
		std::printf("\n%d check(s) failed\n", failures);
		return 1;
	}

	std::printf("cloud http: all checks passed\n");
	return 0;
}