

#define CMWIRE_PROTOBUF 1
#include "cmwire.hpp"

#include "cmclient.hpp"

#include "cm_budget.hpp"
#include "cmlist_cache.hpp"
#include "provision_network.hpp"

#include "../config.hpp"
#include "../log.hpp"
#include "../cainfo.hpp"

#include "../sdk/protobufs/steammessages_base.pb.h"

#include "base64/base64.hpp"

#include <curl/curl.h>
#include <dlfcn.h>

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace CmClient
{

namespace
{

constexpr uint32_t EMSG_MULTI                       = 1;
constexpr uint32_t EMSG_CLIENT_LOGON                = 5514;
constexpr uint32_t EMSG_CLIENT_LOGON_RESPONSE       = 751;
constexpr uint32_t EMSG_PICS_PRODUCT_INFO_REQUEST   = 8903;
constexpr uint32_t EMSG_PICS_PRODUCT_INFO_RESPONSE  = 8904;

constexpr uint64_t kAnonSteamId =
	(static_cast<uint64_t>(1) << 56) | (static_cast<uint64_t>(10) << 52);

constexpr int kTotalTimeoutSecs = 15;
constexpr long long kTotalTimeoutMs = kTotalTimeoutSecs * 1000LL;

class BatchBudget
{
public:
	using Clock = std::chrono::steady_clock;

	BatchBudget() : m_started(Clock::now()) {}

	long remainingSeconds(long operationCapSecs) const
	{
		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
		    Clock::now() - m_started).count();
		return remainingBudgetSeconds(elapsed, kTotalTimeoutMs,
		                              operationCapSecs);
	}

	long remainingMilliseconds(long operationCapMs) const
	{
		const long long elapsed =
		    std::chrono::duration_cast<std::chrono::milliseconds>(
		        Clock::now() - m_started).count();
		if (elapsed >= kTotalTimeoutMs || operationCapMs <= 0) return 0;
		return static_cast<long>(std::min<long long>(
		    kTotalTimeoutMs - std::max(0LL, elapsed),
		    static_cast<long long>(operationCapMs)));
	}

	bool expired() const { return remainingMilliseconds(kTotalTimeoutMs) == 0; }
	Clock::time_point deadline() const
	{
		return m_started + std::chrono::milliseconds(kTotalTimeoutMs);
	}

private:
	Clock::time_point m_started;
};

typedef CURL*    (*curl_easy_init_t)();
typedef CURLcode (*curl_easy_setopt_t)(CURL*, CURLoption, ...);
typedef CURLcode (*curl_easy_perform_t)(CURL*);
typedef void     (*curl_easy_cleanup_t)(CURL*);
typedef CURLcode (*curl_easy_getinfo_t)(CURL*, CURLINFO, ...);
typedef CURLcode (*curl_easy_send_t)(CURL*, const void*, size_t, size_t*);
typedef CURLcode (*curl_easy_recv_t)(CURL*, void*, size_t, size_t*);
typedef const char* (*curl_easy_strerror_t)(CURLcode);

curl_easy_init_t     p_init     = nullptr;
curl_easy_setopt_t   p_setopt   = nullptr;
curl_easy_perform_t  p_perform  = nullptr;
curl_easy_cleanup_t  p_cleanup  = nullptr;
curl_easy_getinfo_t  p_getinfo  = nullptr;
curl_easy_send_t     p_send     = nullptr;
curl_easy_recv_t     p_recv     = nullptr;
curl_easy_strerror_t p_strerror = nullptr;

bool loadCurl()
{
	if (p_init) return true;
	void* h = dlopen("libcurl.so.4", RTLD_NOLOAD | RTLD_LAZY);
	if (!h) h = dlopen("libcurl.so.4", RTLD_LAZY);
	if (!h) h = RTLD_DEFAULT;
	p_init     = (curl_easy_init_t)     dlsym(h, "curl_easy_init");
	p_setopt   = (curl_easy_setopt_t)   dlsym(h, "curl_easy_setopt");
	p_perform  = (curl_easy_perform_t)  dlsym(h, "curl_easy_perform");
	p_cleanup  = (curl_easy_cleanup_t)  dlsym(h, "curl_easy_cleanup");
	p_getinfo  = (curl_easy_getinfo_t)  dlsym(h, "curl_easy_getinfo");
	p_send     = (curl_easy_send_t)     dlsym(h, "curl_easy_send");
	p_recv     = (curl_easy_recv_t)     dlsym(h, "curl_easy_recv");
	p_strerror = (curl_easy_strerror_t) dlsym(h, "curl_easy_strerror");
	return p_init && p_setopt && p_perform && p_cleanup &&
	       p_getinfo && p_send && p_recv;
}

std::size_t writeCb(const char* p, std::size_t sz, std::size_t n, std::string* dst)
{
	dst->append(p, sz * n);
	return sz * n;
}

AppInfoProvision::NetworkFailure httpsGet(const std::string& url,
                                          std::string& body,
                                          const BatchBudget& budget)
{
	if (!loadCurl()) return AppInfoProvision::NetworkFailure::Provider;
	const long timeoutMs = budget.remainingMilliseconds(8000);
	if (timeoutMs == 0) return AppInfoProvision::NetworkFailure::Provider;
	CURL* c = p_init();
	if (!c) return AppInfoProvision::NetworkFailure::Provider;
	body.clear();
	p_setopt(c, CURLOPT_URL, url.c_str());
	p_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
	p_setopt(c, CURLOPT_WRITEFUNCTION, writeCb);
	p_setopt(c, CURLOPT_WRITEDATA, &body);
	p_setopt(c, CURLOPT_TIMEOUT_MS, timeoutMs);
	p_setopt(c, CURLOPT_CONNECTTIMEOUT_MS, std::min(timeoutMs, 5000L));
	p_setopt(c, CURLOPT_NOSIGNAL, 1L);
	p_setopt(c, CURLOPT_USERAGENT, "ember-CmClient/0.1");

	if (const char* f = ca::bundleFile()) p_setopt(c, CURLOPT_CAINFO, f);
	if (const char* d = ca::bundleDir())  p_setopt(c, CURLOPT_CAPATH, d);
	const CURLcode rc = p_perform(c);
	long status = 0;
	p_getinfo(c, CURLINFO_RESPONSE_CODE, &status);
	p_cleanup(c);
	if (rc != CURLE_OK) return AppInfoProvision::classifyCurlFailure(rc);
	if (status != 200) return AppInfoProvision::classifyHttpFailure(status);
	return body.empty() ? AppInfoProvision::NetworkFailure::Provider
	                    : AppInfoProvision::NetworkFailure::None;
}

struct Endpoint { std::string host; int port; };

void parseEndpointsFromJson(const std::string& body, std::vector<Endpoint>& out)
{
	const std::string needle = "\"endpoint\":\"";
	size_t pos = 0;
	while ((pos = body.find(needle, pos)) != std::string::npos)
	{
		pos += needle.size();
		const size_t end = body.find('"', pos);
		if (end == std::string::npos) break;
		const std::string ep = body.substr(pos, end - pos);
		pos = end + 1;
		const auto colon = ep.rfind(':');
		if (colon == std::string::npos) continue;
		Endpoint e;
		e.host = ep.substr(0, colon);
		try { e.port = std::stoi(ep.substr(colon + 1)); }
		catch (...) { continue; }
		if (!e.host.empty() && e.port > 0) out.push_back(std::move(e));
	}
}

std::string cmListCachePath()
{
	const std::string dir = g_config.getDir() + "/cache";
	std::error_code ec;
	if (!std::filesystem::exists(dir)) std::filesystem::create_directories(dir, ec);
	return dir + "/cmlist.txt";
}

long long cmListTtlSecs()
{
	if (const char* ov = std::getenv("EMBER_CMLIST_TTL"); ov && *ov)
	{
		try { return std::stoll(ov); } catch (...) {}
	}
	return 24 * 3600;
}

bool readCachedCmList(std::vector<Endpoint>& out)
{
	const auto path = cmListCachePath();
	struct stat st{};
	if (stat(path.c_str(), &st) != 0 || st.st_size <= 0) return false;
	const long long now = static_cast<long long>(std::time(nullptr));
	if (!cache::isCmListReusable(true, static_cast<long long>(st.st_mtime),
	                             now, cmListTtlSecs()))
		return false;
	std::ifstream ifs(path);
	if (!ifs.is_open()) return false;
	std::string line;
	while (std::getline(ifs, line))
	{
		const auto colon = line.rfind(':');
		if (colon == std::string::npos) continue;
		Endpoint e;
		e.host = line.substr(0, colon);
		try { e.port = std::stoi(line.substr(colon + 1)); }
		catch (...) { continue; }
		if (!e.host.empty() && e.port > 0) out.push_back(std::move(e));
	}
	return !out.empty();
}

void writeCachedCmList(const std::vector<Endpoint>& eps)
{
	if (eps.empty()) return;
	const auto path = cmListCachePath();
	std::ofstream ofs(path, std::ios::trunc);
	if (!ofs.is_open()) return;
	for (const auto& e : eps) ofs << e.host << ':' << e.port << '\n';
}

std::vector<Endpoint> fetchCmList(AppInfoProvision::NetworkFailure& failure,
                                  const BatchBudget& budget)
{
	failure = AppInfoProvision::NetworkFailure::None;

	{
		std::vector<Endpoint> cached;
		if (readCachedCmList(cached))
		{
			g_pLog->info("CmClient: reusing cached CM list (%zu endpoints)\n",
			             cached.size());
			return cached;
		}
	}

	std::vector<Endpoint> out;
	const std::string url =
		"https://api.steampowered.com/ISteamDirectory/GetCMListForConnect/v1/"
		"?cellid=0&cmtype=websockets&format=json";
	std::string body;
	failure = httpsGet(url, body, budget);
	if (failure != AppInfoProvision::NetworkFailure::None) return out;

	parseEndpointsFromJson(body, out);
	writeCachedCmList(out);
	return out;
}

bool gzipInflate(const std::string& in, uint32_t expected, std::string& out)
{
	char tmpIn[] = "/tmp/ember_cm_gz_XXXXXX";
	int fd = mkstemp(tmpIn);
	if (fd < 0) return false;
	const ssize_t w = write(fd, in.data(), in.size());
	close(fd);
	if (w != static_cast<ssize_t>(in.size())) { unlink(tmpIn); return false; }

	const std::string cmd = "gzip -dc " + std::string(tmpIn) + " 2>/dev/null";
	FILE* pp = popen(cmd.c_str(), "r");
	if (!pp) { unlink(tmpIn); return false; }
	out.clear();
	char buf[8192];
	size_t got;

	const size_t cap = expected ? (static_cast<size_t>(expected) + 64)
	                            : (64u * 1024u * 1024u);
	bool overflow = false;
	while ((got = fread(buf, 1, sizeof(buf), pp)) > 0)
	{
		if (out.size() + got > cap) { overflow = true; break; }
		out.append(buf, got);
	}
	const int rc = pclose(pp);
	unlink(tmpIn);
	if (overflow || rc != 0) { out.clear(); return false; }
	return !out.empty();
}

class TlsSocket
{
public:
	explicit TlsSocket(const BatchBudget& budget)
	    : m_budget(budget), m_deadline(budget.deadline()) {}
	~TlsSocket() { if (m_c) p_cleanup(m_c); }

	bool connect(const Endpoint& ep)
	{
		const long timeoutMs = m_budget.remainingMilliseconds(kTotalTimeoutMs);
		if (timeoutMs == 0) return false;
		m_c = p_init();
		if (!m_c) return false;

		const std::string url = "https://" + ep.host + ":" +
		                        std::to_string(ep.port) + "/";
		p_setopt(m_c, CURLOPT_URL, url.c_str());
		p_setopt(m_c, CURLOPT_CONNECT_ONLY, 1L);

		p_setopt(m_c, CURLOPT_SSL_ENABLE_ALPN, 0L);
		p_setopt(m_c, CURLOPT_CONNECTTIMEOUT_MS, std::min(timeoutMs, 6000L));
		p_setopt(m_c, CURLOPT_TIMEOUT_MS, timeoutMs);
		p_setopt(m_c, CURLOPT_NOSIGNAL, 1L);

		if (const char* f = ca::bundleFile()) p_setopt(m_c, CURLOPT_CAINFO, f);
		if (const char* d = ca::bundleDir())  p_setopt(m_c, CURLOPT_CAPATH, d);
		const CURLcode rc = p_perform(m_c);
		return rc == CURLE_OK;
	}

	bool sendAll(const std::string& data)
	{
		size_t total = 0;
		while (total < data.size())
		{
			if (expired()) return false;
			size_t n = 0;
			const CURLcode rc = p_send(m_c, data.data() + total,
			                           data.size() - total, &n);
			if (rc == CURLE_OK) { total += n; continue; }
			if (rc == CURLE_AGAIN) { usleep(20 * 1000); continue; }
			return false;
		}
		return true;
	}

	bool recvSome()
	{
		char buf[16384];
		size_t n = 0;
		const CURLcode rc = p_recv(m_c, buf, sizeof(buf), &n);
		if (rc == CURLE_OK) { m_rx.append(buf, n); return n > 0; }
		if (rc == CURLE_AGAIN) { usleep(20 * 1000); return true; }
		return false;
	}

	std::string& rx() { return m_rx; }

	bool expired() const
	{
		return std::chrono::steady_clock::now() >= m_deadline;
	}

private:
	CURL* m_c = nullptr;
	std::string m_rx;
	const BatchBudget& m_budget;
	std::chrono::steady_clock::time_point m_deadline;
};

std::string randomKeyB64()
{
	std::random_device rd;
	std::string raw(16, '\0');
	for (auto& ch : raw) ch = static_cast<char>(rd() & 0xFF);
	return base64::to_base64(raw);
}

bool wsHandshake(TlsSocket& sock, const Endpoint& ep)
{
	const std::string key = randomKeyB64();
	std::string req;
	req += "GET /cmsocket/ HTTP/1.1\r\n";
	req += "Host: " + ep.host + ":" + std::to_string(ep.port) + "\r\n";
	req += "Upgrade: websocket\r\n";
	req += "Connection: Upgrade\r\n";
	req += "Sec-WebSocket-Key: " + key + "\r\n";
	req += "Sec-WebSocket-Version: 13\r\n";
	req += "\r\n";
	if (!sock.sendAll(req)) return false;

	while (sock.rx().find("\r\n\r\n") == std::string::npos)
	{
		if (sock.expired()) return false;
		if (!sock.recvSome()) return false;
	}
	const auto hdrEnd = sock.rx().find("\r\n\r\n");
	const std::string headers = sock.rx().substr(0, hdrEnd);

	if (headers.compare(0, 12, "HTTP/1.1 101") != 0 &&
	    headers.find(" 101 ") == std::string::npos)
	{
		g_pLog->info("CmClient: ws upgrade rejected: %.40s\n", headers.c_str());
		return false;
	}

	sock.rx().erase(0, hdrEnd + 4);
	return true;
}

std::string wsFrame(const std::string& payload)
{
	std::string f;
	f.push_back(static_cast<char>(0x82));
	const size_t len = payload.size();
	uint8_t maskFlag = 0x80;
	if (len <= 125)
	{
		f.push_back(static_cast<char>(maskFlag | len));
	}
	else if (len <= 0xFFFF)
	{
		f.push_back(static_cast<char>(maskFlag | 126));
		f.push_back(static_cast<char>((len >> 8) & 0xFF));
		f.push_back(static_cast<char>(len & 0xFF));
	}
	else
	{
		f.push_back(static_cast<char>(maskFlag | 127));
		for (int i = 7; i >= 0; --i)
			f.push_back(static_cast<char>((static_cast<uint64_t>(len) >> (i * 8)) & 0xFF));
	}
	uint8_t mask[4];
	std::random_device rd;
	for (auto& m : mask) m = static_cast<uint8_t>(rd() & 0xFF);
	f.append(reinterpret_cast<char*>(mask), 4);
	std::string masked = payload;
	for (size_t i = 0; i < masked.size(); ++i)
		masked[i] = static_cast<char>(masked[i] ^ mask[i % 4]);
	f += masked;
	return f;
}

bool wsTryReadFrame(std::string& buf, std::string& payload,
                    uint8_t& opcode, bool& error)
{
	error = false;
	if (buf.size() < 2) return false;
	const uint8_t b0 = static_cast<uint8_t>(buf[0]);
	const uint8_t b1 = static_cast<uint8_t>(buf[1]);
	opcode = b0 & 0x0F;
	const bool masked = (b1 & 0x80) != 0;
	uint64_t len = b1 & 0x7F;
	size_t pos = 2;
	if (len == 126)
	{
		if (buf.size() < pos + 2) return false;
		len = (static_cast<uint8_t>(buf[pos]) << 8) |
		      static_cast<uint8_t>(buf[pos + 1]);
		pos += 2;
	}
	else if (len == 127)
	{
		if (buf.size() < pos + 8) return false;
		len = 0;
		for (int i = 0; i < 8; ++i)
			len = (len << 8) | static_cast<uint8_t>(buf[pos + i]);
		pos += 8;
	}
	if (masked) pos += 4;
	if (len > (128u * 1024u * 1024u)) { error = true; return false; }
	if (buf.size() < pos + len) return false;
	payload = buf.substr(pos, len);
	buf.erase(0, pos + len);
	return true;
}

struct Session
{
	uint64_t steamid   = kAnonSteamId;
	int32_t  sessionid = 0;
	bool     loggedOn  = false;
};

std::string buildCmPacket(uint32_t emsg, const std::string& body,
                          const Session& s)
{
	CMsgProtoBufHeader hdr;
	hdr.set_steamid(s.steamid);
	hdr.set_client_sessionid(s.sessionid);
	const std::string hdrBytes = hdr.SerializeAsString();
	return CmWire::packPacket(emsg, hdrBytes, body);
}

void handlePacket(const std::string& pkt, Session& session,
                  std::unordered_map<uint32_t, std::string>& out,
                  std::unordered_map<uint32_t, uint32_t>* changes,
                  bool& gotLogon, bool& picsDone)
{
	uint32_t emsg = 0;
	std::string hdrBytes, body;
	if (!CmWire::parsePacket(pkt, emsg, hdrBytes, body)) return;

	if (emsg == EMSG_MULTI)
	{
		CMsgMulti multi;
		if (!multi.ParseFromString(body)) return;
		std::string payload = multi.message_body();
		if (multi.size_unzipped() > 0)
		{
			std::string inflated;
			if (!gzipInflate(payload, multi.size_unzipped(), inflated)) return;
			payload.swap(inflated);
		}
		for (const auto& inner : CmWire::expandMultiBody(payload))
			handlePacket(inner, session, out, changes, gotLogon, picsDone);
		return;
	}

	if (emsg == EMSG_CLIENT_LOGON_RESPONSE)
	{
		CMsgProtoBufHeader hdr;
		if (hdr.ParseFromString(hdrBytes))
		{
			session.steamid   = hdr.steamid();
			session.sessionid = hdr.client_sessionid();
		}
		CMsgClientLogonResponse resp;
		if (resp.ParseFromString(body))
		{
			gotLogon = (resp.eresult() == 1);
			session.loggedOn = gotLogon;
		}
		return;
	}

	if (emsg == EMSG_PICS_PRODUCT_INFO_RESPONSE)
	{
		const bool pending = CmWire::parsePicsResponse(body, out, changes);
		if (!pending) picsDone = true;
		return;
	}
}

template <typename Pred>
bool pumpUntil(TlsSocket& sock, Session& session,
               std::unordered_map<uint32_t, std::string>& out,
               std::unordered_map<uint32_t, uint32_t>* changes,
               bool& gotLogon, bool& picsDone, Pred predicate)
{
	for (;;)
	{

		for (;;)
		{
			std::string payload; uint8_t opcode = 0; bool err = false;
			if (!wsTryReadFrame(sock.rx(), payload, opcode, err))
			{
				if (err) return false;
				break;
			}
			if (opcode == 0x8) return false;
			if (opcode == 0x9 || opcode == 0xA) continue;
			if (opcode == 0x2 || opcode == 0x0)
				handlePacket(payload, session, out, changes, gotLogon, picsDone);
		}
		if (predicate()) return true;
		if (sock.expired()) return false;
		if (!sock.recvSome()) return false;
	}
}

bool runSession(const Endpoint& ep, const std::vector<uint32_t>& appids,
                std::unordered_map<uint32_t, std::string>& out,
                std::unordered_map<uint32_t, uint32_t>* changes,
                const BatchBudget& budget)
{
	TlsSocket sock(budget);
	if (!sock.connect(ep)) return false;
	if (!wsHandshake(sock, ep)) return false;

	Session session;
	bool gotLogon = false, picsDone = false;

	const std::string logonBody = CmWire::buildAnonLogon();
	if (!sock.sendAll(wsFrame(buildCmPacket(EMSG_CLIENT_LOGON, logonBody, session))))
		return false;
	if (!pumpUntil(sock, session, out, changes, gotLogon, picsDone,
	               [&] { return gotLogon; }))
		return false;
	if (!gotLogon) return false;

	const std::string picsBody = CmWire::buildPicsRequest(appids);
	if (!sock.sendAll(wsFrame(buildCmPacket(EMSG_PICS_PRODUCT_INFO_REQUEST,
	                                        picsBody, session))))
		return false;
	if (!pumpUntil(sock, session, out, changes, gotLogon, picsDone,
	               [&] { return picsDone; }))
		return false;

	return !out.empty();
}

}

bool prepareForThreadedFetch()
{
	return loadCurl();
}

FetchResult fetchProductInfoDetailed(
    const std::vector<uint32_t>& appids,
    std::unordered_map<uint32_t, std::string>& out,
    std::unordered_map<uint32_t, uint32_t>* changesOut)
{
	if (appids.empty()) return FetchResult::Failed;
	if (!loadCurl())
	{
		g_pLog->info("CmClient: libcurl unavailable, falling back\n");
		return FetchResult::Failed;
	}
	const BatchBudget budget;

	AppInfoProvision::NetworkFailure listFailure =
	    AppInfoProvision::NetworkFailure::None;
	auto cms = fetchCmList(listFailure, budget);
	if (cms.empty())
	{
		g_pLog->info("CmClient: empty CM list, falling back\n");
		return listFailure == AppInfoProvision::NetworkFailure::Connectivity
		    ? FetchResult::NetworkUnavailable
		    : FetchResult::Failed;
	}

	for (int round = 0; round < 2; ++round)
	{
		const size_t maxTries = std::min<size_t>(cms.size(), 4);
		for (size_t i = 0; i < maxTries; ++i)
		{
			if (budget.expired()) break;
			out.clear();
			if (changesOut) changesOut->clear();
			const auto& ep = cms[i];
			g_pLog->info("CmClient: trying CM %s:%d (%zu apps)\n",
			             ep.host.c_str(), ep.port, appids.size());
			if (runSession(ep, appids, out, changesOut, budget))
			{
				g_pLog->info("CmClient: fetched %zu/%zu apps via %s\n",
				             out.size(), appids.size(), ep.host.c_str());
				return FetchResult::Success;
			}
		}

		if (round == 0)
		{
			if (budget.expired()) break;
			std::error_code ec;
			std::filesystem::remove(cmListCachePath(), ec);
			std::vector<Endpoint> fresh;
			const std::string url =
				"https://api.steampowered.com/ISteamDirectory/GetCMListForConnect/v1/"
				"?cellid=0&cmtype=websockets&format=json";
			std::string body;
			const auto freshFailure = httpsGet(url, body, budget);
			if (freshFailure == AppInfoProvision::NetworkFailure::None)
			{
				parseEndpointsFromJson(body, fresh);
				writeCachedCmList(fresh);
			}
			if (fresh.empty())
			{
				if (freshFailure == AppInfoProvision::NetworkFailure::Connectivity)
				{
					out.clear();
					if (changesOut) changesOut->clear();
					g_pLog->info("CmClient: CM directory unavailable; network appears offline\n");
					return FetchResult::NetworkUnavailable;
				}
				break;
			}
			g_pLog->info("CmClient: cached CMs failed, retrying with fresh list (%zu)\n",
			             fresh.size());
			cms.swap(fresh);
		}
	}
	out.clear();
	if (changesOut) changesOut->clear();
	g_pLog->info("CmClient: all CM attempts failed, falling back\n");
	return FetchResult::Failed;
}

bool fetchProductInfo(const std::vector<uint32_t>& appids,
                      std::unordered_map<uint32_t, std::string>& out,
                      std::unordered_map<uint32_t, uint32_t>* changesOut)
{
	return fetchProductInfoDetailed(appids, out, changesOut) ==
	       FetchResult::Success;
}

}
