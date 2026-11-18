

#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace CmWire
{

constexpr uint32_t PROTO_MASK = 0x80000000u;

inline std::string packPacket(uint32_t emsg, const std::string& hdr,
                              const std::string& body)
{
	std::string out;
	const uint32_t e  = emsg | PROTO_MASK;
	const uint32_t hl = static_cast<uint32_t>(hdr.size());
	out.resize(8);
	std::memcpy(&out[0], &e, 4);
	std::memcpy(&out[4], &hl, 4);
	out += hdr;
	out += body;
	return out;
}

inline bool parsePacket(const std::string& d, uint32_t& emsg,
                        std::string& hdr, std::string& body)
{
	if (d.size() < 8) return false;
	uint32_t e = 0, hl = 0;
	std::memcpy(&e, &d[0], 4);
	std::memcpy(&hl, &d[4], 4);
	if (static_cast<uint64_t>(8) + hl > d.size()) return false;
	emsg = e & ~PROTO_MASK;
	hdr  = d.substr(8, hl);
	body = d.substr(8 + hl);
	return true;
}

inline std::vector<std::string> expandMultiBody(const std::string& blob)
{
	std::vector<std::string> out;
	size_t off = 0;
	while (off + 4 <= blob.size())
	{
		uint32_t sz = 0;
		std::memcpy(&sz, &blob[off], 4);
		off += 4;
		if (static_cast<uint64_t>(off) + sz > blob.size())
		{
			out.clear();
			break;
		}
		out.emplace_back(blob.substr(off, sz));
		off += sz;
	}
	return out;
}

}

#ifdef CMWIRE_PROTOBUF

#include "../sdk/protobufs/steammessages_clientserver_appinfo.pb.h"
#include "../sdk/protobufs/steammessages_clientserver_login.pb.h"

#include <unordered_map>
#include <vector>

namespace CmWire
{

inline std::string buildAnonLogon()
{
	CMsgClientLogon b;
	b.set_protocol_version(65580);
	b.set_client_package_version(1561159470);
	b.set_client_os_type(4);
	return b.SerializeAsString();
}

inline std::string buildPicsRequest(const std::vector<uint32_t>& appids)
{
	CMsgClientPICSProductInfoRequest r;
	r.set_meta_data_only(false);
	for (uint32_t a : appids)
	{
		auto* e = r.add_apps();
		e->set_appid(a);
	}
	return r.SerializeAsString();
}

inline bool parsePicsResponse(const std::string& body,
                              std::unordered_map<uint32_t, std::string>& out,
                              std::unordered_map<uint32_t, uint32_t>* changesOut = nullptr)
{
	CMsgClientPICSProductInfoResponse r;
	if (!r.ParseFromString(body)) return false;
	for (const auto& app : r.apps())
	{
		if (app.buffer().empty()) continue;
		out[app.appid()] = app.buffer();
		if (changesOut) (*changesOut)[app.appid()] = app.change_number();
	}
	return r.response_pending();
}

}

#endif
