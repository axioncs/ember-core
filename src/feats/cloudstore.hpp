// SPDX-License-Identifier: AGPL-3.0-only
//
// The on-disk cloud save store. No Steam, no config, no logging: pure filesystem,
// so it is unit-testable on the host.
//
// Layout:
//
//   <root>/<accountId>/<appId>/manifest    sha \t size \t timestamp \t path
//   <root>/<accountId>/<appId>/blobs/<sha> content-addressed save bytes
//   <root>/<accountId>/<appId>/cn          change number, one uint64
//
// Blobs are content-addressed, so two files with the same bytes share one blob
// and a re-upload of unchanged content is a no-op. `manifest` is authoritative
// for what Steam sees; the blobs directory is a cache that can be rebuilt from
// any other machine's copy.

#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace CloudStore
{
	struct Entry
	{
		std::string path;	// relative to the app directory, '/' separated
		std::string sha;
		uint64_t size = 0;
		uint64_t timestamp = 0;
	};

	// A save file name is untrusted: it arrives from another process's protobuf
	// and is used to build a path. False for absolute paths, any '..' segment,
	// any '\' (Windows separators must not become directories), control
	// characters, and the reserved .ember segments.
	bool isSafePath(const std::string& path);

	// Splits a manifest line. False when the line is malformed.
	bool parseManifestLine(const std::string& line, Entry& out);

	// Formats an entry as a manifest line.
	std::string formatManifestLine(const Entry& entry);

	// All entries for an app, sorted by path. A missing app directory is empty,
	// not an error — a game that never synced has no store entry yet.
	std::vector<Entry> list(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId);

	// Stores `data` under its SHA, links it into the app's blob directory, and
	// records it in the manifest. Re-storing identical bytes leaves the manifest
	// untouched and reports false. Returns false when the path is unsafe.
	bool put(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId,
		const std::string& path, const std::string& data, Entry& out);

	// Reads a file back. False when the path is absent from the manifest or the
	// blob is missing from disk (a partially copied store).
	bool get(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId,
		const std::string& path, std::string& out);

	// Drops a path from the manifest. Blobs are left alone: they are shared and
	// may still be referenced, and the store is small.
	bool remove(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId,
		const std::string& path);

	uint64_t changeNumber(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId);
	void setChangeNumber(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId,
		const uint64_t value);

	// Every app id with a store entry, for the CLI to enumerate.
	std::vector<uint32_t> listApps(const std::filesystem::path& root, const uint32_t accountId);

	// Total bytes of save data (blobs excluded) across an app.
	uint64_t totalSize(const std::filesystem::path& root, const uint32_t accountId, const uint32_t appId);

	// Root of the whole store: <config>/cloud. `config` is the ember config dir.
	std::filesystem::path defaultRoot(const std::filesystem::path& configDir);
}