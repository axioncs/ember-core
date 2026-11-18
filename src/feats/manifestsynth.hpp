

#pragma once

#include "../ascii.hpp"
#include "yaml-cpp/yaml.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ManifestSynth
{

	struct SynthDepot
	{
		uint32_t    depotId  = 0;
		uint64_t    gid      = 0;
		std::string oslist;
		uint64_t    size     = 0;
		uint64_t    download = 0;
	};

	inline int injectSynthesizedDepots(YAML::Node& body,
	                                   const std::vector<SynthDepot>& depots)
	{
		if (!body.IsMap()) return 0;

		if (YAML::Node existing = body["depots"];
		    existing && existing.IsMap() && existing.size() > 0)
			return 0;

		YAML::Node depotsNode(YAML::NodeType::Map);
		int added = 0;
		for (const auto& d : depots)
		{
			if (d.gid == 0) continue;
			const std::string key = std::to_string(d.depotId);
			YAML::Node entry(YAML::NodeType::Map);
			YAML::Node pub(YAML::NodeType::Map);
			pub["gid"] = std::to_string(d.gid);
			if (d.size > 0)     pub["size"]     = std::to_string(d.size);
			if (d.download > 0) pub["download"] = std::to_string(d.download);
			entry["manifests"]["public"] = pub;
			if (!d.oslist.empty())
				entry["config"]["oslist"] = d.oslist;
			depotsNode[key] = entry;
			++added;
		}

		if (added > 0) body["depots"] = depotsNode;
		return added;
	}

	inline std::string detectOsFromFiles(const std::vector<std::string>& files)
	{
		auto lower = [](std::string s)
		{
			for (auto& c : s)
				c = static_cast<char>(
					Ascii::toLower(static_cast<unsigned char>(c)));
			return s;
		};
		auto ends = [](const std::string& s, const std::string& suf)
		{
			return s.size() >= suf.size() && s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
		};

		bool win = false, mac = false, lin = false;
		for (const auto& raw : files)
		{
			const std::string f = lower(raw);
			if (ends(f, ".exe")) win = true;
			if (f.find(".app/") != std::string::npos ||
			    f.find(".app\\") != std::string::npos ||
			    f.find("contents/macos") != std::string::npos ||
			    f.find("contents\\macos") != std::string::npos ||
			    ends(f, ".dylib")) mac = true;
			if (ends(f, ".so") || f.find(".so.") != std::string::npos ||
			    f.find("-linux-gnu") != std::string::npos ||
			    f.find("-linux/") != std::string::npos ||
			    ends(f, ".x86_64") || ends(f, ".x86")) lin = true;
		}
		if (win) return "windows";
		if (mac) return "macos";
		if (lin) return "linux";
		return "";
	}

	inline bool parseManifestSizes(const std::string& m,
	                               uint64_t& sizeOut, uint64_t& downloadOut)
	{
		constexpr uint32_t kMetadataMagic = 0x1F4812BEu;

		auto readU32le = [&](size_t off, uint32_t& out) -> bool
		{
			if (off + 4 > m.size()) return false;
			out = static_cast<uint8_t>(m[off]) |
			      (static_cast<uint32_t>(static_cast<uint8_t>(m[off + 1])) << 8) |
			      (static_cast<uint32_t>(static_cast<uint8_t>(m[off + 2])) << 16) |
			      (static_cast<uint32_t>(static_cast<uint8_t>(m[off + 3])) << 24);
			return true;
		};

		size_t meta = std::string::npos, metaLen = 0;
		for (size_t i = 0; i + 8 <= m.size();)
		{
			uint32_t magic = 0, len = 0;
			if (!readU32le(i, magic) || !readU32le(i + 4, len)) break;
			if (static_cast<uint64_t>(i) + 8 + len > m.size()) break;
			if (magic == kMetadataMagic) { meta = i + 8; metaLen = len; break; }
			i += 8 + len;
		}
		if (meta == std::string::npos) return false;

		auto readVarint = [&](size_t& p, size_t end, uint64_t& out) -> bool
		{
			uint64_t r = 0; int shift = 0;
			while (p < end)
			{
				const uint8_t b = static_cast<uint8_t>(m[p++]);
				if (shift <= 63) r |= static_cast<uint64_t>(b & 0x7F) << shift;
				shift += 7;
				if (!(b & 0x80)) { out = r; return true; }
				if (shift > 70) return false;
			}
			return false;
		};

		const size_t end = meta + metaLen;
		bool gotSize = false, gotDownload = false;
		for (size_t p = meta; p < end;)
		{
			uint64_t tag = 0;
			if (!readVarint(p, end, tag)) break;
			const uint32_t field = static_cast<uint32_t>(tag >> 3);
			const uint32_t wire  = static_cast<uint32_t>(tag & 0x7);
			if (wire == 0)
			{
				uint64_t v = 0;
				if (!readVarint(p, end, v)) break;
				if (field == 5) { sizeOut = v; gotSize = true; }
				else if (field == 6) { downloadOut = v; gotDownload = true; }
			}
			else if (wire == 2)
			{
				uint64_t len = 0;
				if (!readVarint(p, end, len) || p + len > end) break;
				p += len;
			}
			else if (wire == 5) { p += 4; }
			else if (wire == 1) { p += 8; }
			else break;
		}
		return gotSize || gotDownload;
	}

	inline bool ensureInstallDir(YAML::Node& body)
	{
		if (!body.IsMap()) return false;

		if (YAML::Node cfg = body["config"]; cfg && cfg.IsMap())
		{
			YAML::Node dir = cfg["installdir"];
			if (dir && dir.IsScalar() && !dir.as<std::string>().empty())
				return false;
		}

		YAML::Node common = body["common"];
		if (!common || !common.IsMap()) return false;
		YAML::Node name = common["name"];
		if (!name || !name.IsScalar()) return false;

		std::string raw;
		try { raw = name.as<std::string>(); }
		catch (...) { return false; }

		std::string out;
		out.reserve(raw.size());
		bool pendingSpace = false;
		for (unsigned char c : raw)
		{
			const bool bad = c < 0x20 || c == '/' || c == '\\' || c == ':' ||
			                 c == '*' || c == '?' || c == '"' || c == '<' ||
			                 c == '>' || c == '|';
			if (bad || c == ' ')
			{
				if (!out.empty()) pendingSpace = true;
				continue;
			}
			if (pendingSpace) { out.push_back(' '); pendingSpace = false; }
			out.push_back(static_cast<char>(c));
		}
		if (out.empty()) return false;

		body["config"]["installdir"] = out;
		return true;
	}

	inline std::vector<std::string> extractManifestFilenames(const std::string& m)
	{
		constexpr uint32_t kPayloadMagic = 0x71F617D0u;
		std::vector<std::string> out;

		auto u32le = [&](size_t off, uint32_t& v) -> bool
		{
			if (off + 4 > m.size()) return false;
			v = static_cast<uint8_t>(m[off]) |
			    (static_cast<uint32_t>(static_cast<uint8_t>(m[off + 1])) << 8) |
			    (static_cast<uint32_t>(static_cast<uint8_t>(m[off + 2])) << 16) |
			    (static_cast<uint32_t>(static_cast<uint8_t>(m[off + 3])) << 24);
			return true;
		};

		size_t pl = std::string::npos, plLen = 0;
		for (size_t i = 0; i + 8 <= m.size();)
		{
			uint32_t magic = 0, len = 0;
			if (!u32le(i, magic) || !u32le(i + 4, len)) break;
			if (static_cast<uint64_t>(i) + 8 + len > m.size()) break;
			if (magic == kPayloadMagic) { pl = i + 8; plLen = len; break; }
			i += 8 + len;
		}
		if (pl == std::string::npos) return out;

		auto varint = [&](size_t& p, size_t end, uint64_t& v) -> bool
		{
			uint64_t r = 0; int s = 0;
			while (p < end)
			{
				const uint8_t b = static_cast<uint8_t>(m[p++]);
				if (s <= 63) r |= static_cast<uint64_t>(b & 0x7F) << s;
				s += 7;
				if (!(b & 0x80)) { v = r; return true; }
				if (s > 70) return false;
			}
			return false;
		};

		const size_t plEnd = pl + plLen;
		for (size_t p = pl; p < plEnd;)
		{
			uint64_t tag = 0;
			if (!varint(p, plEnd, tag)) break;
			const uint32_t field = static_cast<uint32_t>(tag >> 3);
			const uint32_t wire  = static_cast<uint32_t>(tag & 0x7);
			if (wire == 2)
			{
				uint64_t len = 0;
				if (!varint(p, plEnd, len) || p + len > plEnd) break;
				if (field == 1)
				{

					size_t q = p, qEnd = p + len;
					uint64_t ftag = 0;
					if (varint(q, qEnd, ftag) && (ftag >> 3) == 1 && (ftag & 7) == 2)
					{
						uint64_t flen = 0;
						if (varint(q, qEnd, flen) && q + flen <= qEnd)
							out.emplace_back(m.substr(q, flen));
					}
				}
				p += len;
			}
			else if (wire == 0) { uint64_t v; if (!varint(p, plEnd, v)) break; }
			else if (wire == 5) { p += 4; }
			else if (wire == 1) { p += 8; }
			else break;
		}
		return out;
	}

	inline std::string pickLauncher(const std::vector<std::string>& files,
	                                const std::string& installdir,
	                                const std::string& os)
	{
		auto lower = [](std::string s)
		{
			for (auto& c : s)
				c = static_cast<char>(
					Ascii::toLower(static_cast<unsigned char>(c)));
			return s;
		};
		auto ends = [&](const std::string& s, const std::string& suf)
		{
			std::string ls = lower(s), lsuf = lower(suf);
			return ls.size() >= lsuf.size() &&
			       ls.compare(ls.size() - lsuf.size(), lsuf.size(), lsuf) == 0;
		};
		auto isRoot = [](const std::string& f)
		{
			return f.find('\\') == std::string::npos && f.find('/') == std::string::npos;
		};
		auto isHelper = [&](const std::string& f)
		{
			const std::string l = lower(f);
			return l.find("crashhandler") != std::string::npos ||
			       l.find("crashpad") != std::string::npos ||
			       l.find("unitycrash") != std::string::npos ||
			       l.find("vc_redist") != std::string::npos ||
			       l.find("vcredist") != std::string::npos ||
			       l.find("dxsetup") != std::string::npos ||
			       l.find("uninstall") != std::string::npos;
		};
		auto stem = [&](std::string f)
		{
			const auto dot = f.rfind('.');
			if (dot != std::string::npos) f = f.substr(0, dot);
			return lower(f);
		};
		const std::string want = lower(installdir);

		if (os == "macos")
		{

			std::vector<std::string> bundles;
			for (const auto& f : files)
			{
				const std::string lf = lower(f);
				size_t pos = lf.find(".app/");
				if (pos == std::string::npos) pos = lf.find(".app\\");
				if (pos == std::string::npos) continue;
				const std::string bundle = f.substr(0, pos + 4);
				if (isRoot(bundle)) bundles.push_back(bundle);
			}
			if (bundles.empty()) return "";
			for (const auto& b : bundles)
				if (stem(b) == want) return b;
			return bundles.front();
		}

		std::vector<std::string> cands;
		for (const auto& f : files)
		{
			if (!isRoot(f) || isHelper(f)) continue;
			if (os == "windows")
			{
				if (ends(f, ".exe")) cands.push_back(f);
			}
			else
			{
				const bool lib = ends(f, ".so") || f.find(".so.") != std::string::npos ||
				                 ends(f, ".dll");
				if (lib) continue;
				const bool execLike = ends(f, ".sh") || ends(f, ".x86_64") ||
				                      ends(f, ".x86") || ends(f, ".run") ||
				                      f.find('.') == std::string::npos;
				if (execLike) cands.push_back(f);
			}
		}
		if (cands.empty()) return "";

		for (const auto& f : cands)
			if (stem(f) == "steamshim") return f;

		for (const auto& f : cands)
			if (stem(f) == want) return f;

		auto squash = [&](std::string s)
		{
			std::string o; for (char c : lower(s)) if (c != ' ') o.push_back(c); return o;
		};
		const std::string wantSquash = squash(installdir);
		for (const auto& f : cands)
			if (squash(stem(f)) == wantSquash) return f;
		return cands.front();
	}

	inline int ensureLaunchEntries(YAML::Node& body,
	    const std::vector<std::pair<std::string, std::string>>& entries)
	{
		if (!body.IsMap()) return 0;

		if (YAML::Node cfg = body["config"]; cfg && cfg.IsMap())
		{
			YAML::Node launch = cfg["launch"];
			if (launch && launch.IsMap() && launch.size() > 0)
				return 0;
		}

		std::string desc;
		if (YAML::Node n = body["common"]["name"]; n && n.IsScalar())
			desc = n.as<std::string>();

		int added = 0;
		for (const auto& [executable, oslist] : entries)
		{
			if (executable.empty()) continue;
			YAML::Node entry(YAML::NodeType::Map);
			entry["executable"] = executable;
			if (!desc.empty()) entry["description"] = desc;
			if (!oslist.empty()) entry["config"]["oslist"] = oslist;
			body["config"]["launch"][std::to_string(added)] = entry;
			++added;
		}
		return added;
	}
}
