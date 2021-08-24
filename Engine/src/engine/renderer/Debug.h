#pragma once

#include <string_view>

class feDebugGroup final
{
public:
	feDebugGroup(std::string_view message, unsigned int id);
	~feDebugGroup();

	feDebugGroup(const feDebugGroup&) = delete;
	feDebugGroup& operator=(const feDebugGroup&) = delete;
	feDebugGroup(feDebugGroup&&) noexcept = delete;
	feDebugGroup& operator=(feDebugGroup&&) noexcept = delete;
};