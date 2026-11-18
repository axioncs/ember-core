

#pragma once

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace AffTrace
{

	inline constexpr std::size_t kMaxRecords = 4096;
	inline constexpr std::size_t kMaxRollups = 48;

	enum class Kind : std::uint8_t
	{
		IpcOwner = 0,
		IpcEnter,
		IpcExit,
		WatchEnter,
		WatchExit,
		SteamCallEnter,
		SteamCallExit,
		SteamFnEnter,
		SteamFnExit,
		QueuePush,
		QueueDrain,
		QueueStale,
		HourRollup,
	};

	enum class Src : std::uint8_t
	{
		None = 0,
		Config,
		Api,
		Ipc,
		Site,
	};

	enum class Call : std::uint8_t
	{
		None = 0,
		OnModify,
		Package0Inject,
		Package0Sync,
		LicenseReconcile,
		InstallApp,
		CompatMapping,
		CutlMemoryGrow,
		MarkLicenseChanged,
		ProcessLicenseUpdates,
		NotifyLicensesUpdated,
		RunIpcFrame,
		Drain,
	};

	enum class Mode : std::uint8_t
	{
		NA = 0,
		Queued,
		Direct,
		DirectNoOwner,
		DirectOverflow,
		DirectDisabled,
		DirectStale,
	};

	struct Record
	{
		std::uint64_t seq = 0;
		std::uint64_t tMonoUs = 0;
		Kind          kind = Kind::IpcOwner;
		Src           src = Src::None;
		Call          call = Call::None;
		Mode          mode = Mode::NA;
		long          tid = 0;
		long          ownerTid = 0;
		int           offOwner = 0;
		std::uint32_t ipcDepth = 0;
		std::uint64_t ipcFrames = 0;
		std::uint32_t queueDepth = 0;
		std::uint64_t durUs = 0;
		std::uint64_t framesDuring = 0;
	};

	struct Rollup
	{
		std::uint64_t seq = 0;
		std::uint64_t tMonoUs = 0;
		std::uint64_t hour = 0;
		std::uint64_t watcherCallbacks = 0;
		std::uint64_t steamCalls = 0;
		std::uint64_t offOwnerSteamCalls = 0;
		std::uint64_t overlaps = 0;
		std::uint64_t ipcFrames = 0;
	};

	inline const char* kindName(Kind k)
	{
		switch (k)
		{
			case Kind::IpcOwner:       return "IPC_OWNER";
			case Kind::IpcEnter:       return "IPC_ENTER";
			case Kind::IpcExit:        return "IPC_EXIT";
			case Kind::WatchEnter:     return "WATCH_ENTER";
			case Kind::WatchExit:      return "WATCH_EXIT";
			case Kind::SteamCallEnter: return "STEAMCALL_ENTER";
			case Kind::SteamCallExit:  return "STEAMCALL_EXIT";
			case Kind::SteamFnEnter:   return "STEAMFN_ENTER";
			case Kind::SteamFnExit:    return "STEAMFN_EXIT";
			case Kind::QueuePush:      return "QUEUE_PUSH";
			case Kind::QueueDrain:     return "QUEUE_DRAIN";
			case Kind::QueueStale:     return "QUEUE_STALE";
			case Kind::HourRollup:     return "HOUR_ROLLUP";
		}
		return "UNKNOWN";
	}

	inline const char* srcName(Src s)
	{
		switch (s)
		{
			case Src::None:   return "none";
			case Src::Config: return "config";
			case Src::Api:    return "api";
			case Src::Ipc:    return "ipc";
			case Src::Site:   return "site";
		}
		return "none";
	}

	inline const char* callName(Call c)
	{
		switch (c)
		{
			case Call::None:                  return "none";
			case Call::OnModify:              return "on_modify";
			case Call::Package0Inject:        return "package0_inject";
			case Call::Package0Sync:          return "package0_sync";
			case Call::LicenseReconcile:      return "license_reconcile";
			case Call::InstallApp:            return "install_app";
			case Call::CompatMapping:         return "compat_mapping";
			case Call::CutlMemoryGrow:        return "cutlmemory_grow";
			case Call::MarkLicenseChanged:    return "mark_license_changed";
			case Call::ProcessLicenseUpdates: return "process_license_updates";
			case Call::NotifyLicensesUpdated: return "notify_licenses_updated";
			case Call::RunIpcFrame:           return "run_ipc_frame";
			case Call::Drain:                 return "drain";
		}
		return "none";
	}

	inline const char* modeName(Mode m)
	{
		switch (m)
		{
			case Mode::NA:             return "n/a";
			case Mode::Queued:         return "queued";
			case Mode::Direct:         return "direct";
			case Mode::DirectNoOwner:  return "direct_no_owner";
			case Mode::DirectOverflow: return "direct_overflow";
			case Mode::DirectDisabled: return "direct_disabled";
			case Mode::DirectStale:    return "direct_stale";
		}
		return "n/a";
	}

	inline bool optInEnabled(const char* value)
	{
		if (value == nullptr || value[0] == '\0')
			return false;
		std::string v(value);
		for (char& c : v)
			c = static_cast<char>((c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c);
		return v == "1" || v == "y" || v == "yes" || v == "on" || v == "true";
	}

	inline std::string format(const Record& r)
	{
		char buf[352];
		std::snprintf(buf, sizeof(buf),
			"seq=%llu t_mono_us=%llu kind=%s src=%s call=%s mode=%s tid=%ld "
			"owner_tid=%ld off_owner=%d ipc_depth=%u ipc_frames=%llu "
			"queue_depth=%u dur_us=%llu frames_during=%llu\n",
			static_cast<unsigned long long>(r.seq),
			static_cast<unsigned long long>(r.tMonoUs),
			kindName(r.kind), srcName(r.src), callName(r.call), modeName(r.mode),
			r.tid, r.ownerTid, r.offOwner,
			r.ipcDepth,
			static_cast<unsigned long long>(r.ipcFrames),
			r.queueDepth,
			static_cast<unsigned long long>(r.durUs),
			static_cast<unsigned long long>(r.framesDuring));
		return std::string(buf);
	}

	inline std::string formatRollup(const Rollup& s)
	{
		char buf[288];
		std::snprintf(buf, sizeof(buf),
			"seq=%llu t_mono_us=%llu kind=%s hour=%llu watch_cb=%llu "
			"steam_calls=%llu off_owner_calls=%llu overlaps=%llu ipc_frames=%llu\n",
			static_cast<unsigned long long>(s.seq),
			static_cast<unsigned long long>(s.tMonoUs),
			kindName(Kind::HourRollup),
			static_cast<unsigned long long>(s.hour),
			static_cast<unsigned long long>(s.watcherCallbacks),
			static_cast<unsigned long long>(s.steamCalls),
			static_cast<unsigned long long>(s.offOwnerSteamCalls),
			static_cast<unsigned long long>(s.overlaps),
			static_cast<unsigned long long>(s.ipcFrames));
		return std::string(buf);
	}

	inline bool isOverlap(const Record& r)
	{
		return r.kind == Kind::SteamCallExit && r.offOwner != 0
		    && (r.ipcDepth >= 1 || r.framesDuring >= 1);
	}

	namespace detail
	{
		inline std::atomic<bool>          g_enabled{ false };
		inline std::atomic<bool>          g_initDone{ false };
		inline std::atomic<long>          g_ownerTid{ 0 };
		inline std::atomic<std::uint64_t> g_seq{ 0 };
		inline std::atomic<std::uint32_t> g_ipcDepth{ 0 };
		inline std::atomic<std::uint64_t> g_ipcFrames{ 0 };

		inline std::atomic<int>           g_callWindows{ 0 };
		inline std::atomic<std::uint64_t> g_records{ 0 };
		inline std::atomic<std::uint64_t> g_rollups{ 0 };
		inline std::atomic<std::uint64_t> g_startUs{ 0 };
		inline std::atomic<std::uint64_t> g_hourBucket{ 0 };

		inline std::atomic<std::uint64_t> g_rollupUs{ 3600ULL * 1000000ULL };

		inline std::mutex& sinkMutex()
		{
			static std::mutex m;
			return m;
		}
		inline int& sinkFd()
		{
			static int fd = -1;
			return fd;
		}

		inline std::atomic<std::uint64_t> g_hourWatchCb{ 0 };
		inline std::atomic<std::uint64_t> g_hourSteamCalls{ 0 };
		inline std::atomic<std::uint64_t> g_hourOffOwner{ 0 };
		inline std::atomic<std::uint64_t> g_hourOverlaps{ 0 };
	}

	inline bool enabled()
	{
		return detail::g_enabled.load(std::memory_order_relaxed);
	}

	inline std::uint64_t monotonicUs()
	{
		struct timespec ts{};
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return static_cast<std::uint64_t>(ts.tv_sec) * 1000000ULL
		     + static_cast<std::uint64_t>(ts.tv_nsec) / 1000ULL;
	}

	inline long selfTid()
	{

		static thread_local long tid = 0;
		if (tid == 0)
			tid = static_cast<long>(syscall(SYS_gettid));
		return tid;
	}

	inline long ownerTid()
	{
		return detail::g_ownerTid.load(std::memory_order_relaxed);
	}

	inline bool onOwnerThread()
	{
		const long owner = detail::g_ownerTid.load(std::memory_order_relaxed);
		return owner != 0 && owner == selfTid();
	}

	inline std::uint64_t ipcFrameCount()
	{
		return detail::g_ipcFrames.load(std::memory_order_relaxed);
	}

	inline std::uint32_t ipcDepth()
	{
		return detail::g_ipcDepth.load(std::memory_order_relaxed);
	}

	inline void writeLine(const std::string& line)
	{
		std::lock_guard<std::mutex> lk(detail::sinkMutex());
		const int fd = detail::sinkFd();
		if (fd < 0)
			return;
		const ssize_t written = ::write(fd, line.data(), line.size());
		(void)written;
	}

	inline void maybeRollup(std::uint64_t nowUs)
	{
		const std::uint64_t start = detail::g_startUs.load(std::memory_order_relaxed);
		if (start == 0 || nowUs < start)
			return;
		const std::uint64_t width = detail::g_rollupUs.load(std::memory_order_relaxed);
		const std::uint64_t hour = (nowUs - start) / (width ? width : 1);
		std::uint64_t seen = detail::g_hourBucket.load(std::memory_order_relaxed);
		if (hour == seen)
			return;
		if (!detail::g_hourBucket.compare_exchange_strong(seen, hour))
			return;
		if (detail::g_rollups.fetch_add(1) >= kMaxRollups)
			return;

		Rollup s;
		s.seq = detail::g_seq.fetch_add(1);
		s.tMonoUs = nowUs;
		s.hour = seen;
		s.watcherCallbacks = detail::g_hourWatchCb.exchange(0);
		s.steamCalls = detail::g_hourSteamCalls.exchange(0);
		s.offOwnerSteamCalls = detail::g_hourOffOwner.exchange(0);
		s.overlaps = detail::g_hourOverlaps.exchange(0);
		s.ipcFrames = detail::g_ipcFrames.load(std::memory_order_relaxed);
		writeLine(formatRollup(s));
	}

	inline void emit(Record& r)
	{
		if (!enabled())
			return;
		if (detail::g_records.fetch_add(1) >= kMaxRecords)
			return;
		r.seq = detail::g_seq.fetch_add(1);
		if (r.tMonoUs == 0)
			r.tMonoUs = monotonicUs();
		maybeRollup(r.tMonoUs);
		if (r.tid == 0)
			r.tid = selfTid();
		r.ownerTid = ownerTid();
		r.offOwner = (r.ownerTid != 0 && r.tid != r.ownerTid) ? 1 : 0;
		writeLine(format(r));
	}

	inline bool openSink(const std::string& path)
	{
		std::lock_guard<std::mutex> lk(detail::sinkMutex());
		if (detail::sinkFd() >= 0)
			return true;
		const int fd = ::open(path.c_str(),
			O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0600);
		if (fd < 0)
			return false;

		(void)::fchmod(fd, 0600);
		detail::sinkFd() = fd;
		return true;
	}

	inline std::string defaultPath()
	{
		const char* home = std::getenv("HOME");
		if (home == nullptr || home[0] == '\0')
			return std::string();
		return std::string(home) + "/.ember.afftrace.log";
	}

	inline bool init()
	{
		bool expected = false;
		if (!detail::g_initDone.compare_exchange_strong(expected, true))
			return enabled();
		if (!optInEnabled(std::getenv("EMBER_AFFTRACE")))
			return false;

		const std::string path = defaultPath();
		if (path.empty() || !openSink(path))
			return false;

		detail::g_startUs.store(monotonicUs(), std::memory_order_relaxed);
		detail::g_enabled.store(true, std::memory_order_relaxed);
		return true;
	}

	inline void reset()
	{
		{
			std::lock_guard<std::mutex> lk(detail::sinkMutex());
			if (detail::sinkFd() >= 0)
			{
				::close(detail::sinkFd());
				detail::sinkFd() = -1;
			}
		}
		detail::g_enabled.store(false);
		detail::g_initDone.store(false);
		detail::g_ownerTid.store(0);
		detail::g_seq.store(0);
		detail::g_ipcDepth.store(0);
		detail::g_ipcFrames.store(0);
		detail::g_callWindows.store(0);
		detail::g_records.store(0);
		detail::g_rollups.store(0);
		detail::g_startUs.store(0);
		detail::g_hourBucket.store(0);
		detail::g_hourWatchCb.store(0);
		detail::g_hourSteamCalls.store(0);
		detail::g_hourOffOwner.store(0);
		detail::g_hourOverlaps.store(0);
		detail::g_rollupUs.store(3600ULL * 1000000ULL);
	}

	inline void setRollupIntervalUs(std::uint64_t us)
	{
		detail::g_rollupUs.store(us ? us : 1);
	}

	inline void latchOwner()
	{
		if (!enabled())
			return;
		long expected = 0;
		const long self = selfTid();
		if (!detail::g_ownerTid.compare_exchange_strong(expected, self))
			return;
		Record r;
		r.kind = Kind::IpcOwner;
		r.src = Src::Ipc;
		r.call = Call::RunIpcFrame;
		r.tid = self;
		emit(r);
	}

	inline void ipcFrameEnter()
	{
		if (!enabled())
			return;
		detail::g_ipcFrames.fetch_add(1, std::memory_order_relaxed);
		const std::uint32_t depth =
			detail::g_ipcDepth.fetch_add(1, std::memory_order_relaxed) + 1;
		if (detail::g_callWindows.load(std::memory_order_relaxed) == 0)
			return;
		Record r;
		r.kind = Kind::IpcEnter;
		r.src = Src::Ipc;
		r.call = Call::RunIpcFrame;
		r.ipcDepth = depth;
		r.ipcFrames = detail::g_ipcFrames.load(std::memory_order_relaxed);
		emit(r);
	}

	inline void ipcFrameExit()
	{
		if (!enabled())
			return;
		const std::uint32_t before =
			detail::g_ipcDepth.fetch_sub(1, std::memory_order_relaxed);
		if (detail::g_callWindows.load(std::memory_order_relaxed) == 0)
			return;
		Record r;
		r.kind = Kind::IpcExit;
		r.src = Src::Ipc;
		r.call = Call::RunIpcFrame;
		r.ipcDepth = before ? before - 1 : 0;
		r.ipcFrames = detail::g_ipcFrames.load(std::memory_order_relaxed);
		emit(r);
	}

	inline void queuePush(Src src, Call call, Mode mode, std::uint32_t queueDepth)
	{
		if (!enabled())
			return;
		Record r;
		r.kind = Kind::QueuePush;
		r.src = src;
		r.call = call;
		r.mode = mode;
		r.queueDepth = queueDepth;
		r.ipcDepth = ipcDepth();
		r.ipcFrames = ipcFrameCount();
		emit(r);
	}

	inline void queueDrain(std::uint64_t taken, std::uint32_t queueDepth)
	{
		if (!enabled())
			return;
		Record r;
		r.kind = Kind::QueueDrain;
		r.src = Src::Ipc;
		r.call = Call::Drain;
		r.mode = Mode::Queued;
		r.queueDepth = queueDepth;
		r.framesDuring = taken;
		r.ipcDepth = ipcDepth();
		r.ipcFrames = ipcFrameCount();
		emit(r);
	}

	inline void queueStale(std::uint64_t flushed, std::uint32_t queueDepth)
	{
		if (!enabled())
			return;
		Record r;
		r.kind = Kind::QueueStale;
		r.src = Src::None;
		r.call = Call::Drain;
		r.mode = Mode::DirectStale;
		r.queueDepth = queueDepth;
		r.framesDuring = flushed;
		r.ipcDepth = ipcDepth();
		r.ipcFrames = ipcFrameCount();
		emit(r);
	}

	class FrameGuard
	{
	public:
		FrameGuard() : m_counting(enabled() && onOwnerThread())
		{
			if (m_counting)
				ipcFrameEnter();
		}
		~FrameGuard()
		{
			if (m_counting)
				ipcFrameExit();
		}
		FrameGuard(const FrameGuard&) = delete;
		FrameGuard& operator=(const FrameGuard&) = delete;

		bool counting() const { return m_counting; }

	private:
		bool m_counting;
	};

	class Scope
	{
	public:
		Scope(Kind enterKind, Kind exitKind, Src src, Call call, Mode mode,
		      bool openCallWindow = false)
			: m_exitKind(exitKind), m_src(src), m_call(call), m_mode(mode),
			  m_window(openCallWindow), m_active(enabled())
		{
			if (!m_active)
				return;
			if (m_window)
				detail::g_callWindows.fetch_add(1, std::memory_order_relaxed);
			m_startUs = monotonicUs();
			m_startFrames = ipcFrameCount();
			Record r;
			r.tMonoUs = m_startUs;
			r.kind = enterKind;
			r.src = m_src;
			r.call = m_call;
			r.mode = m_mode;
			r.ipcDepth = ipcDepth();
			r.ipcFrames = m_startFrames;
			emit(r);
		}

		~Scope()
		{
			if (!m_active)
				return;
			const std::uint64_t now = monotonicUs();
			Record r;
			r.tMonoUs = now;
			r.kind = m_exitKind;
			r.src = m_src;
			r.call = m_call;
			r.mode = m_mode;
			r.ipcDepth = ipcDepth();
			r.ipcFrames = ipcFrameCount();
			r.durUs = now - m_startUs;
			r.framesDuring = r.ipcFrames - m_startFrames;
			r.tid = selfTid();
			r.ownerTid = ownerTid();
			r.offOwner = (r.ownerTid != 0 && r.tid != r.ownerTid) ? 1 : 0;

			if (m_exitKind == Kind::WatchExit)
				detail::g_hourWatchCb.fetch_add(1, std::memory_order_relaxed);
			if (m_exitKind == Kind::SteamCallExit)
			{
				detail::g_hourSteamCalls.fetch_add(1, std::memory_order_relaxed);
				if (r.offOwner)
					detail::g_hourOffOwner.fetch_add(1, std::memory_order_relaxed);
				if (isOverlap(r))
					detail::g_hourOverlaps.fetch_add(1, std::memory_order_relaxed);
			}

			emit(r);
			if (m_window)
				detail::g_callWindows.fetch_sub(1, std::memory_order_relaxed);
		}

		Scope(const Scope&) = delete;
		Scope& operator=(const Scope&) = delete;

	private:
		Kind          m_exitKind;
		Src           m_src;
		Call          m_call;
		Mode          m_mode;
		bool          m_window;
		bool          m_active;
		std::uint64_t m_startUs = 0;
		std::uint64_t m_startFrames = 0;
	};

	inline Scope watchSpan(Src src)
	{
		return Scope(Kind::WatchEnter, Kind::WatchExit, src, Call::OnModify, Mode::NA);
	}

	inline Scope callSpan(Src src, Call call, Mode mode)
	{
		return Scope(Kind::SteamCallEnter, Kind::SteamCallExit, src, call, mode, true);
	}

	inline Scope fnSpan(Call call, Mode mode)
	{
		return Scope(Kind::SteamFnEnter, Kind::SteamFnExit, Src::Site, call, mode);
	}
}
