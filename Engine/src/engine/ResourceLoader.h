#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace feResourceLoader
{
	std::optional<std::string> LoadTextFile(std::string_view filename);
}