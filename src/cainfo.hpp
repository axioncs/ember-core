#pragma once

#include <functional>
#include <string>
#include <vector>

#include <unistd.h>

namespace ca
{

struct CaPaths
{
	std::string file;
	std::string dir;
};

inline const std::vector<std::string>& caFileEnvVars()
{
	static const std::vector<std::string> v = {
		"EMBER_CA_BUNDLE",
		"CURL_CA_BUNDLE",
		"SSL_CERT_FILE",
	};
	return v;
}

inline const std::vector<std::string>& caFileCandidates()
{
	static const std::vector<std::string> v = {
		"/etc/ssl/certs/ca-certificates.crt",
		"/etc/pki/tls/certs/ca-bundle.crt",
		"/etc/ssl/ca-bundle.pem",
		"/etc/pki/tls/cacert.pem",
		"/etc/ssl/cert.pem",
		"/etc/ca-certificates/extracted/tls-ca-bundle.pem",
		"/usr/local/share/certs/ca-root-nss.crt",
	};
	return v;
}

inline const std::vector<std::string>& caDirCandidates()
{
	static const std::vector<std::string> v = {
		"/etc/ssl/certs",
		"/etc/pki/tls/certs",
		"/etc/ca-certificates/extracted",
	};
	return v;
}

inline CaPaths discoverWith(const std::function<const char*(const char*)>& getenv_fn,
                            const std::function<bool(const std::string&)>& exists_fn)
{
	CaPaths out;

	auto envValue = [&](const char* key) -> const char* {
		const char* v = getenv_fn(key);
		return (v && v[0] != '\0') ? v : nullptr;
	};

	for (const std::string& key : caFileEnvVars())
	{
		if (const char* v = envValue(key.c_str()); v && exists_fn(v))
		{
			out.file = v;
			break;
		}
	}
	if (out.file.empty())
	{
		for (const std::string& cand : caFileCandidates())
		{
			if (exists_fn(cand)) { out.file = cand; break; }
		}
	}

	if (const char* v = envValue("SSL_CERT_DIR"); v && exists_fn(v))
	{
		out.dir = v;
	}
	if (out.dir.empty())
	{
		for (const std::string& cand : caDirCandidates())
		{
			if (exists_fn(cand)) { out.dir = cand; break; }
		}
	}

	return out;
}

inline bool pathExists(const std::string& p)
{
	return ::access(p.c_str(), F_OK) == 0;
}

inline const CaPaths& discovered()
{
	static const CaPaths paths = discoverWith(
		[](const char* k) -> const char* { return std::getenv(k); },
		pathExists);
	return paths;
}

inline const char* bundleFile()
{
	const auto& p = discovered();
	return p.file.empty() ? nullptr : p.file.c_str();
}

inline const char* bundleDir()
{
	const auto& p = discovered();
	return p.dir.empty() ? nullptr : p.dir.c_str();
}

}
