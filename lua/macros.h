#if !defined(BASE_LUA_MACROS_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_MACROS_H

#include <Base/macros.h>
#include <lua5.4/lua.h>
#include <lua5.4/lualib.h>
#include <lua5.4/lauxlib.h>

#define BASE_LUA_MALLOC_FAIL(L)      	luaL_error(L, "malloc failed.")
#define BASE_LUA_STACK_FAIL(L, n)    	luaL_stack(L, n, "Stack failed to grow.")
#define BASE_LUA_NUM_METHODS(reg_array) ((sizeof(reg_array) / sizeof(luaL_Reg)) - 1)
#define BASE_LUA_MT_SELF_INDEX(L) do { \
	lua_pushvalue(L, -1); \
	lua_setfield(L, -2, "__index"); \
} while (0)
#define BASE_LUA_LOAD_SUBMODULE(L, mainModule, submodule) do { \
	lua_pushcfunction(L, &luaopen_##mainModule##_##submodule); \
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) \
		return luaL_error(L, "Failed to load submodule %s.", BASE_STRINGIFY(submodule)); \
	lua_setfield(L, -2, BASE_STRINGIFY(submodule)); \
} while (0)
#define BASE_LUA_LOAD_MODULE(L, module) do { \
	lua_pushcfunction(L, &luaopen_##module); \
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) \
		return luaL_error(L, "Failed to load main module %s.", BASE_STRINGIFY(module)); \
} while (0)
#define BASE_LUA_NEW_UD(L, type)		(type*)lua_newuserdatauv(L, sizeof(type), 0)
#define BASE_LUA_CHECK_UD(L, idx, type, mt)	(type*)luaL_checkudata(L, idx, mt)
#define BASE_LUA_TEST_UD(L,  idx, type, mt)	(type*)luaL_testudata(L, idx, mt)
#endif
