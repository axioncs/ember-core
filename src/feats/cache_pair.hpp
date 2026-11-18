

#pragma once

#include "provision_cache.hpp"

#include "../utils/atomic_file.hpp"

#include <filesystem>
#include <functional>
#include <string>

namespace CachePair
{

namespace detail
{

inline std::string backupPath(const std::string& path)
{
	return path + ".ember-pair-backup";
}

inline bool stageMember(const std::string& path, bool& staged,
                        std::string& error)
{
	staged = false;
	std::error_code ec;
	const bool exists = std::filesystem::exists(path, ec);
	if (ec)
	{
		error = "cannot inspect cache pair member " + path + ": " +
		         ec.message();
		return false;
	}
	if (!exists)
	{

		std::filesystem::remove(backupPath(path), ec);
		return true;
	}

	std::filesystem::copy_file(path, backupPath(path),
		std::filesystem::copy_options::overwrite_existing, ec);
	if (ec)
	{
		error = "cannot stage cache pair member " + path + ": " +
		         ec.message();
		return false;
	}
	staged = true;
	return true;
}

inline bool restoreMember(const std::string& path, bool staged,
                          std::string& error)
{
	std::error_code ec;
	if (!staged)
	{

		if (std::filesystem::exists(path, ec) &&
		    !std::filesystem::remove(path, ec))
		{
			error += "; unable to remove failed cache member " + path;
			if (ec) error += ": " + ec.message();
			return false;
		}
		return !ec;
	}

	std::filesystem::rename(backupPath(path), path, ec);
	if (ec)
	{
		error += "; unable to restore cache member " + path + ": " +
		         ec.message();
		return false;
	}
	return true;
}

inline void discardBackup(const std::string& path, bool staged)
{
	if (!staged) return;
	std::error_code ec;
	std::filesystem::remove(backupPath(path), ec);
}

}

inline bool publish(
	const std::string& bufferPath, const std::string& metadataPath,
	const std::string& buffer, const std::string& metadata, bool synthetic,
	bool markerBefore, const std::function<bool(bool)>& setMarker,
	const std::function<bool()>& markerIsPresent, std::string& error)
{
	error.clear();
	bool bufferStaged = false;
	bool metadataStaged = false;

	if (!detail::stageMember(bufferPath, bufferStaged, error)) return false;
	if (!detail::stageMember(metadataPath, metadataStaged, error))
	{
		(void)detail::restoreMember(bufferPath, bufferStaged, error);
		return false;
	}

	const auto rollback = [&] {
		const bool bufferRestored =
			detail::restoreMember(bufferPath, bufferStaged, error);
		const bool metadataRestored =
			detail::restoreMember(metadataPath, metadataStaged, error);
		if (!bufferRestored || !metadataRestored)
			error += "; previous cache pair could not be restored";
	};

	if (!AtomicFile::write(bufferPath, buffer, error))
	{
		rollback();
		return false;
	}
	if (!AtomicFile::write(metadataPath, metadata, error))
	{
		rollback();
		return false;
	}

	const bool markerOperationSucceeded = setMarker(synthetic);
	if (!AppInfoProvision::cache::syntheticMarkerPublicationConsistent(
	        synthetic, markerOperationSucceeded, markerIsPresent()))
	{
		error = markerOperationSucceeded
		    ? "cache marker state is inconsistent"
		    : "cache marker publication failed";
		if (!setMarker(markerBefore))
			error += "; cache marker rollback failed";
		rollback();
		return false;
	}

	detail::discardBackup(bufferPath, bufferStaged);
	detail::discardBackup(metadataPath, metadataStaged);
	return true;
}

}
