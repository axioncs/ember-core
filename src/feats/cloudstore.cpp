// SPDX-License-Identifier: AGPL-3.0-only

#include "cloudstore.hpp"

#include "sha256.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <system_error>

namespace
{
	// Steam's cloud threads call in concurrently (download and upload workers), so
	// every read-modify-write of a manifest is serialised. The CLI writes the
	// same tree while Steam is stopped, so this is not protecting against it.
	std::mutex& storeMutex()
	{
		static std::mutex mutex;
		return mutex;
	}

	// std::mutex is not recursive, and put() needs both the lock and a read of
	// the current manifest. Every public entry point holds one of these for its
	// whole body, and the read helpers below never lock again.
	struct Lock
	{
		std::unique_lock<std::mutex> guard;

		Lock() : guard(storeMutex()) {}
	};

	std::filesystem::path appDir(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId)
	{
		return root / std::to_string(accountId) / std::to_string(appId);
	}

	std::filesystem::path manifestPath(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId)
	{
		return appDir(root, accountId, appId) / "manifest";
	}

	std::filesystem::path blobDir(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId)
	{
		return appDir(root, accountId, appId) / "blobs";
	}

	bool readText(const std::filesystem::path& path, std::string& out)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
		{
			return false;
		}

		std::ostringstream buffer;
		buffer << file.rdbuf();
		out = buffer.str();
		return true;
	}

	// tmp + rename: a crash mid-write leaves the previous manifest intact, and
	// Steam never reads a half-written one.
	bool writeText(const std::filesystem::path& path, const std::string& text)
	{
		std::error_code ec;
		std::filesystem::create_directories(path.parent_path(), ec);

		const auto tmp = path.string() + ".tmp";
		{
			std::ofstream file(tmp, std::ios::binary | std::ios::trunc);
			if (!file.is_open())
			{
				return false;
			}
			file << text;
			file.close();
		}

		std::filesystem::rename(tmp, path, ec);
		if (ec)
		{
			std::filesystem::remove(tmp, ec);
			return false;
		}

		return true;
	}

	// Caller holds the lock.
	std::vector<CloudStore::Entry> listLocked(const std::filesystem::path& root,
		const uint32_t accountId, const uint32_t appId)
	{
		std::vector<CloudStore::Entry> entries;

		std::string text;
		if (!readText(manifestPath(root, accountId, appId), text))
		{
			return entries;
		}

		std::istringstream stream(text);
		std::string line;

		while (std::getline(stream, line))
		{
			CloudStore::Entry entry;
			if (CloudStore::parseManifestLine(line, entry))
			{
				entries.push_back(entry);
			}
		}

		std::sort(entries.begin(), entries.end(), [](const CloudStore::Entry& a, const CloudStore::Entry& b) {
			return a.path < b.path;
		});

		return entries;
	}
}

namespace CloudStore
{
	bool isSafePath(const std::string& path)
	{
		if (path.empty() || path.size() > 4096)
		{
			return false;
		}

		// A leading '/' or a Windows drive makes std::filesystem treat the path
		// as absolute, which would escape the app directory entirely.
		if (path.front() == '/' || path.front() == '\\')
		{
			return false;
		}

		if (path.find('\\') != std::string::npos)
		{
			return false;
		}

		if (path.find('\0') != std::string::npos)
		{
			return false;
		}

		std::istringstream stream(path);
		std::string segment;

		while (std::getline(stream, segment, '/'))
		{
			if (segment.empty() || segment == "." || segment == "..")
			{
				return false;
			}

			// `.ember` is ours; a game that ships a save with that name is
			// trying to write into our bookkeeping.
			if (segment == ".ember")
			{
				return false;
			}

			for (const char c : segment)
			{
				if (static_cast<unsigned char>(c) < 0x20)
				{
					return false;
				}
			}
		}

		return true;
	}

	bool parseManifestLine(const std::string& line, Entry& out)
	{
		if (line.empty())
		{
			return false;
		}

		std::vector<std::string> columns;
		std::istringstream stream(line);
		std::string column;

		while (std::getline(stream, column, '\t'))
		{
			columns.push_back(column);
		}

		// The path is last and cannot contain a tab (isSafePath rejects control
		// characters), so exactly four columns is the whole contract.
		if (columns.size() != 4)
		{
			return false;
		}

		const auto toNumber = [](const std::string& text, uint64_t& target) {
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

			try
			{
				target = std::stoull(text);
			}
			catch (...)
			{
				return false;
			}

			return true;
		};

		Entry entry;
		entry.sha = columns[0];

		if (entry.sha.size() != 64 || !toNumber(columns[1], entry.size) || !toNumber(columns[2], entry.timestamp))
		{
			return false;
		}

		for (const char c : entry.sha)
		{
			const bool hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
			if (!hex)
			{
				return false;
			}
		}

		entry.path = columns[3];

		if (!isSafePath(entry.path))
		{
			return false;
		}

		out = entry;
		return true;
	}

	std::string formatManifestLine(const Entry& entry)
	{
		std::ostringstream line;
		line << entry.sha << '\t' << entry.size << '\t' << entry.timestamp << '\t' << entry.path << '\n';
		return line.str();
	}

	std::vector<Entry> list(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId)
	{
		const Lock lock;
		return listLocked(root, accountId, appId);
	}

	PutResult put(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId,
		const std::string& path, const std::string& data, Entry& out)
	{
		if (!isSafePath(path))
		{
			return PutResult::Error;
		}

		const Lock lock;

		const auto sha = Ember::sha256Hex(data);

		std::error_code ec;
		std::filesystem::create_directories(blobDir(root, accountId, appId), ec);

		const auto blob = blobDir(root, accountId, appId) / sha;

		// Content-addressed: the same bytes from another app or another machine
		// are already here. Write to a temp name and rename so a reader never
		// sees a partial blob.
		if (!std::filesystem::exists(blob))
		{
			const auto tmp = blob.string() + ".tmp";
			{
				std::ofstream file(tmp, std::ios::binary | std::ios::trunc);
				if (!file.is_open())
				{
					return PutResult::Error;
				}
				file << data;
				file.close();
			}

			std::filesystem::rename(tmp, blob, ec);
			if (ec)
			{
				std::filesystem::remove(tmp, ec);
				return PutResult::Error;
			}
		}

		const auto existing = listLocked(root, accountId, appId);

		const auto current = std::find_if(existing.begin(), existing.end(), [&path](const Entry& entry) {
			return entry.path == path;
		});

		if (current != existing.end() && current->sha == sha && current->size == data.size())
		{
			// Same bytes: nothing to record, and no new change. This is a
			// successful PUT to the caller, not a rejected one.
			out = *current;
			return PutResult::Unchanged;
		}

		// A changed file inherits its timestamp, so Steam sees a modification
		// rather than a delete plus a new file.
		const uint64_t timestamp = (current != existing.end())
			? current->timestamp
			: static_cast<uint64_t>(std::time(nullptr));

		std::ostringstream rebuilt;

		for (const auto& entry : existing)
		{
			if (entry.path != path)
			{
				rebuilt << formatManifestLine(entry);
			}
		}

		out.path = path;
		out.sha = sha;
		out.size = data.size();
		out.timestamp = timestamp;

		rebuilt << formatManifestLine(out);

		if (!writeText(manifestPath(root, accountId, appId), rebuilt.str()))
		{
			return PutResult::Error;
		}

		return PutResult::Stored;
	}

	bool get(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId,
		const std::string& path, std::string& out)
	{
		if (!isSafePath(path))
		{
			return false;
		}

		const Lock lock;

		const auto entries = listLocked(root, accountId, appId);

		const auto it = std::find_if(entries.begin(), entries.end(), [&path](const Entry& entry) {
			return entry.path == path;
		});

		if (it == entries.end())
		{
			return false;
		}

		return readText(blobDir(root, accountId, appId) / it->sha, out);
	}

	bool remove(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId,
		const std::string& path)
	{
		if (!isSafePath(path))
		{
			return false;
		}

		const Lock lock;

		const auto existing = listLocked(root, accountId, appId);

		std::ostringstream rebuilt;
		bool removed = false;

		for (const auto& entry : existing)
		{
			if (entry.path == path)
			{
				removed = true;
				continue;
			}

			rebuilt << formatManifestLine(entry);
		}

		if (!removed)
		{
			return false;
		}

		return writeText(manifestPath(root, accountId, appId), rebuilt.str());
	}

	uint64_t changeNumber(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId)
	{
		const Lock lock;

		std::string text;
		if (!readText(appDir(root, accountId, appId) / "cn", text))
		{
			return 0;
		}

		uint64_t value = 0;

		for (const char c : text)
		{
			if (c < '0' || c > '9')
			{
				return 0;
			}

			value = value * 10 + static_cast<uint64_t>(c - '0');
		}

		return value;
	}

	void setChangeNumber(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId,
		const uint64_t value)
	{
		const Lock lock;
		writeText(appDir(root, accountId, appId) / "cn", std::to_string(value));
	}

	std::vector<uint32_t> listApps(const std::filesystem::path& root, const uint32_t accountId)
	{
		std::vector<uint32_t> apps;
		std::error_code ec;

		const auto accountDir = root / std::to_string(accountId);
		if (!std::filesystem::is_directory(accountDir, ec))
		{
			return apps;
		}

		for (const auto& entry : std::filesystem::directory_iterator(accountDir, ec))
		{
			if (!entry.is_directory())
			{
				continue;
			}

			const std::string name = entry.path().filename().string();

			if (name.empty() || name.find_first_not_of("0123456789") != std::string::npos)
			{
				continue;
			}

			try
			{
				apps.push_back(static_cast<uint32_t>(std::stoul(name)));
			}
			catch (...)
			{
				continue;
			}
		}

		std::sort(apps.begin(), apps.end());
		return apps;
	}

	uint64_t totalSize(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId)
	{
		const Lock lock;

		uint64_t total = 0;

		for (const auto& entry : listLocked(root, accountId, appId))
		{
			total += entry.size;
		}

		return total;
	}

	std::filesystem::path defaultRoot(const std::filesystem::path& configDir)
	{
		return configDir / "cloud";
	}
}