#ifndef BASE_LUA_MACROS_H
#define BASE_LUA_MACROS_H
#include "macros.h"
#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>

#define BASE_LUA_MALLOC_FAIL(L)      	luaL_error(L, "malloc failed.")
#define BASE_LUA_STACK_FAIL(L, n)    	luaL_stack(L, n, "Stack failed to grow.")
#define BASE_LUA_TEST_LUDATA(L, idx) 	(lua_islightuserdata(L, idx) ? lua_touserdata(L, idx) : NULL)
#define BASE_LUA_NUM_METHODS(reg_array) ((sizeof(reg_array) / sizeof(luaL_Reg)) - 1)
#define BASE_LUA_MT_SELF_INDEX(L) do { \
	lua_pushvalue(L, -1); \
	lua_setfield(L, -2, "__index"); \
} while (0)
#endif
