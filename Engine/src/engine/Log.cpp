#include "Log.h"

#if defined(FE_PLAT_WINDOWS)
#	include <Windows.h>
#endif

#include <spdlog/spdlog.h>

#include "../vendor/debug-trap.h"

static struct SpdlogStaticInitializer final
{
	SpdlogStaticInitializer()
	{
		spdlog::default_logger()->set_level(spdlog::level::trace);
	}

	~SpdlogStaticInitializer()
	{
		spdlog::default_logger()->flush();
		spdlog::drop_all();
		spdlog::shutdown();
	}
} s_SpdlogStaticInitializer;

namespace feLog
{
	void Trace(std::string_view string)
	{
		spdlog::trace(string);
	}

	void Debug(std::string_view string)
	{
		spdlog::debug(string);
	}

	void Info(std::string_view string)
	{
		spdlog::info(string);
	}

	void Warn(std::string_view string)
	{
		spdlog::warn(string);
	}

	void Error(std::string_view string)
	{
		spdlog::error(string);
	}

	void Critical(std::string_view string)
	{
		spdlog::critical(string);
	}

	void Break()
	{
#if defined(FE_PLAT_WINDOWS)
		if (!IsDebuggerPresent()) return;
#endif
		psnip_trap();
	}
}