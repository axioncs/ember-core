// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

namespace ManifestStore
{
	// Copy <depot>_<gid>.manifest from ~/.config/ember/manifests into the
	// Steam depotcache. Idempotent; safe on every config reload.
	void publish();
}
