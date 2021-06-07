#ifndef BASE_LUA_MACROS_H
#define BASE_LUA_MACROS_H
#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>

#define BASE_LUA_MALLOC_FAIL(s)       luaL_error(s, "malloc failed.")
#define BASE_LUA_STACK_FAIL(s, n)     luaL_stack(s, n, "Stack failed to grow.")
#define BASE_LUA_TEST_LUDATA(s, idx)  (lua_islightuserdata(s, idx) ? lua_touserdata(s, idx) : NULL)
#endif
