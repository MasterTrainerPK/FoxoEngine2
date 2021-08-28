#include "Script.h"

#include <utility>
#include "Log.h"

feScript::feScript()
{
	L = luaL_newstate();
	luaL_openlibs(L);
}

feScript::~feScript() noexcept
{
	if (L) lua_close(L);
}

feScript::feScript(feScript&& other) noexcept
{
	std::swap(L, other.L);
}

feScript& feScript::operator=(feScript&& other) noexcept
{
	std::swap(L, other.L);
	return *this;
}

void feScript::RunFile(const char* file)
{
	if (luaL_dofile(L, file)) feLog::Error(lua_tostring(L, -1));
}

bool feScript::GetGlobalInt(const char* name, int& output)
{
	// Push the global, name on to the top of the stack
	lua_getglobal(L, name);

	// If the value isnt a number then pop this varaible off the stack and return
	if (!lua_isnumber(L, -1))
	{
		lua_pop(L, 1);
		return false;
	}

	// otherwise get the value and set the output
	output = (int) lua_tointeger(L, -1);
	lua_pop(L, 1);
	return true;
}

bool feScript::GetGlobalFloat(const char* name, float& output)
{
	// Push the global, name on to the top of the stack
	lua_getglobal(L, name);

	// If the value isnt a number then pop this varaible off the stack and return
	if (!lua_isnumber(L, -1))
	{
		lua_pop(L, 1);
		return false;
	}

	// otherwise get the value and set the output
	output = (float) lua_tonumber(L, -1);
	lua_pop(L, 1);
	return true;
}

lua_State* feScript::GetState() const
{
	return L;
}

void feScript::PrintStack(lua_State* L)
{
	int top = lua_gettop(L);

	if (top > 0)
	{
		for (int i = 1; i <= top; i++)
		{
			const char* typen = luaL_typename(L, i);

			switch (lua_type(L, i)) {
				case LUA_TNUMBER:
					feLog::Info("{} {:>8} = {}", i, typen, lua_tonumber(L, i));
					break;
				case LUA_TSTRING:
					feLog::Info("{} {:>8} = {}", i, typen, lua_tostring(L, i));
					break;
				case LUA_TBOOLEAN:
					feLog::Info("{} {:>8} = {}", i, typen, (lua_toboolean(L, i) ? "true" : "false"));
					break;
				case LUA_TNIL:
					feLog::Info("{} {:>8} = {}", i, typen, "nil");
					break;
				default:
					feLog::Info("{} {:>8} = {}", i, typen, lua_topointer(L, i));
					break;
			}
		}
	}
	else feLog::Info("Empty stack");
	feLog::Info("-----------------");
}

#if 0
// https://stackoverflow.com/a/6142700/15771797
static void iterate_and_print(lua_State* L, int index)
{
	// Push another reference to the table on top of the stack (so we know
	// where it is, and this function can work for negative, positive and
	// pseudo indices
	lua_pushvalue(L, index);
	// stack now contains: -1 => table
	lua_pushnil(L);
	// stack now contains: -1 => nil; -2 => table
	while (lua_next(L, -2))
	{
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(L, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		const char* key = lua_tostring(L, -1);
		const char* value = lua_tostring(L, -2);
		printf("%s => %s\n", key, value);
		// pop value + copy of key, leaving original key
		lua_pop(L, 2);
		// stack now contains: -1 => key; -2 => table
	}
	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)
	// Pop table
	lua_pop(L, 1);
	// Stack is now the same as it was on entry to this function
}
#endif