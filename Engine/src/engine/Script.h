#pragma once

#include <lua.hpp>

class feScript final
{
public:
	feScript();
	~feScript() noexcept;
	feScript(const feScript&) = delete;
	feScript& operator=(const feScript&) = delete;
	feScript(feScript&& other) noexcept;
	feScript& operator=(feScript&& other) noexcept;

	void RunFile(const char* file);
	bool GetGlobalInt(const char* name, int& output);
	bool GetGlobalFloat(const char* name, float& output);

	lua_State* GetState() const;
private:
	lua_State* L = nullptr;
};