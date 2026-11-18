
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace ContentServerDirectory
{

enum class HttpsSupport
{
	Mandatory,
	Optional,
	Unavailable,
};

struct Server
{
	std::string type;
	std::string host;
	std::string vhost;
	HttpsSupport httpsSupport = HttpsSupport::Optional;
};

std::vector<Server> parseServerList(std::string_view response);

std::string manifestUrl(const Server& server, uint32_t depotId,
                        uint64_t manifestGid, uint64_t requestCode);

std::string hostHeader(const Server& server);

}
