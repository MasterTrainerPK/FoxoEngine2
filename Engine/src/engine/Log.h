#pragma once

#include <string_view>

#include <spdlog/fmt/fmt.h>

namespace feLog
{
	void Trace(std::string_view string);
	void Debug(std::string_view string);
	void Info(std::string_view string);
	void Warn(std::string_view string);
	void Error(std::string_view string);
	void Critical(std::string_view string);

	void Break();

	template<typename... t_Args>
	void Trace(fmt::format_string<t_Args...> fmt, t_Args &&...args)
	{
		Trace(fmt::format(fmt, std::forward<t_Args>(args)...));
	}

	template<typename... t_Args>
	void Debug(fmt::format_string<t_Args...> fmt, t_Args &&...args)
	{
		Debug(fmt::format(fmt, std::forward<t_Args>(args)...));
	}

	template<typename... t_Args>
	void Info(fmt::format_string<t_Args...> fmt, t_Args &&...args)
	{
		Info(fmt::format(fmt, std::forward<t_Args>(args)...));
	}

	template<typename... t_Args>
	void Warn(fmt::format_string<t_Args...> fmt, t_Args &&...args)
	{
		Warn(fmt::format(fmt, std::forward<t_Args>(args)...));
	}

	template<typename... t_Args>
	void Error(fmt::format_string<t_Args...> fmt, t_Args &&...args)
	{
		Error(fmt::format(fmt, std::forward<t_Args>(args)...));
	}

	template<typename... t_Args>
	void Critical(fmt::format_string<t_Args...> fmt, t_Args &&...args)
	{
		Critical(fmt::format(fmt, std::forward<t_Args>(args)...));
	}
}