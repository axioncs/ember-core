

#pragma once

#include <mutex>

namespace DepotKey
{

	class LuaScriptImportGate
	{
	public:
		template <typename Ready, typename Import>
		bool run(Ready&& ready, Import&& import) noexcept
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_done) return false;

			try
			{
				if (!ready()) return false;
				import();
				m_done = true;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		bool done() const noexcept
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_done;
		}

		template <typename Ready, typename Import>
		bool rerun(Ready&& ready, Import&& import) noexcept
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			try
			{
				if (!ready()) return false;
				import();
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

	private:
		mutable std::mutex m_mutex;
		bool m_done = false;
	};
}
