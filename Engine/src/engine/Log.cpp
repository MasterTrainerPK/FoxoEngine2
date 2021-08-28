#include "Log.h"

#if defined(FE_PLAT_WINDOWS)
#	include <Windows.h>
#endif

#include <spdlog/spdlog.h>
#include <tinyfiledialogs.h>

#include "../vendor/debug-trap.h"

/// This structure is only here so that you dont have to explicitly setup logging
/// Since this is a global variable, this will be constructed before the main function gets called
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
		tinyfd_messageBox("A critical error has occured", string.data(), "ok", "error", 1);
		Break();
	}
	
#if defined(FE_PLAT_WINDOWS)
	void Break()
	{
		if (IsDebuggerPresent()) psnip_trap();
	}
#else
	void Break()
	{
		psnip_trap();
	}
#endif
}