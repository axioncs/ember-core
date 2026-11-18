

#include "../src/cainfo.hpp"

#include <cstdio>
#include <map>
#include <set>
#include <string>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

static std::function<const char*(const char*)> envFrom(std::map<std::string, std::string> m)
{
	return [m = std::move(m)](const char* key) -> const char* {
		auto it = m.find(key ? key : "");
		return it == m.end() ? nullptr : it->second.c_str();
	};
}

static std::function<bool(const std::string&)> existsFrom(std::set<std::string> present)
{
	return [present = std::move(present)](const std::string& p) { return present.count(p) != 0; };
}

int main()
{

	{
		auto env = envFrom({{"CURL_CA_BUNDLE", "/custom/ca.pem"}});
		auto ex  = existsFrom({"/custom/ca.pem", "/etc/ssl/certs/ca-certificates.crt"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file == "/custom/ca.pem", "CURL_CA_BUNDLE present -> used verbatim");
	}

	{
		auto env = envFrom({{"SSL_CERT_FILE", "/x/sslcert.pem"}});
		auto ex  = existsFrom({"/x/sslcert.pem"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file == "/x/sslcert.pem", "SSL_CERT_FILE honored");
	}

	{
		auto env = envFrom({{"EMBER_CA_BUNDLE", "/override.pem"},
		                    {"CURL_CA_BUNDLE",     "/curl.pem"}});
		auto ex  = existsFrom({"/override.pem", "/curl.pem"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file == "/override.pem", "EMBER_CA_BUNDLE overrides CURL_CA_BUNDLE");
	}

	{
		auto env = envFrom({{"CURL_CA_BUNDLE", "/missing.pem"},
		                    {"SSL_CERT_FILE",  "/present.pem"}});
		auto ex  = existsFrom({"/present.pem"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file == "/present.pem", "missing env path skipped, next present env used");
	}

	{
		auto env = envFrom({{"CURL_CA_BUNDLE", ""}});
		auto ex  = existsFrom({"/etc/ssl/certs/ca-certificates.crt"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file == "/etc/ssl/certs/ca-certificates.crt",
		      "empty env value ignored, probe list used");
	}

	{
		auto env = envFrom({});
		auto ex  = existsFrom({"/etc/ssl/certs/ca-certificates.crt"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file == "/etc/ssl/certs/ca-certificates.crt",
		      "probe finds Debian/Arch bundle");
	}

	{
		auto env = envFrom({});
		auto ex  = existsFrom({"/etc/ca-certificates/extracted/tls-ca-bundle.pem"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file == "/etc/ca-certificates/extracted/tls-ca-bundle.pem",
		      "probe finds the extracted SteamOS/Arch bundle");
	}

	{
		auto env = envFrom({});
		auto ex  = existsFrom({"/etc/pki/tls/certs/ca-bundle.crt"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file == "/etc/pki/tls/certs/ca-bundle.crt",
		      "probe finds the Fedora/RHEL bundle");
	}

	{
		auto env = envFrom({{"SSL_CERT_DIR", "/my/certs"}});
		auto ex  = existsFrom({"/my/certs"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.dir == "/my/certs", "SSL_CERT_DIR honored for CAPATH");
	}
	{
		auto env = envFrom({});
		auto ex  = existsFrom({"/etc/ssl/certs"});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.dir == "/etc/ssl/certs", "dir probe finds /etc/ssl/certs");
	}

	{
		auto env = envFrom({});
		auto ex  = existsFrom({});
		const auto r = ca::discoverWith(env, ex);
		CHECK(r.file.empty() && r.dir.empty(),
		      "no trust store anywhere -> empty (curl keeps its default)");
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}
