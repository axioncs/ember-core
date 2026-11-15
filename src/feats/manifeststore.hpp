// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

namespace ManifestStore
{
	// Copy <depot>_<gid>.manifest from ~/.config/ember/manifests into the
	// Steam depotcache. Idempotent; safe on every config reload.
	void publish();

	// Stage one depot's pinned manifest. The client purges depotcache entries
	// for apps whose update was cancelled, so the plan hook has to restage on
	// every attempt, not just at load. Returns true when the entry is in
	// depotcache afterwards.
	bool stage(unsigned int depotId, unsigned long long manifestId);
}
