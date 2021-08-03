#if !defined(BASE_LUA_MACROS_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_MACROS_H

#include <Base/macros.h>

#if defined(BASE_EXTERN_LUA_5_4)
#  define BASE_LUA_5_4
#elif defined(BASE_EXTERN_LUA_5_3)
#  define BASE_LUA_5_3
#else
#  error "Unsupported Lua version!"
#endif

#ifdef __OpenBSD__
#  if defined(BASE_LUA_5_4)
#    include <lua-5.4/lua.h>
#    include <lua-5.4/lualib.h>
#    include <lua-5.4/lauxlib.h>
#  elif defined(BASE_LUA_5_3)
#    include <lua-5.3/lua.h>
#    include <lua-5.3/lualib.h>
#    include <lua-5.3/lauxlib.h>
#  else
#    error "Unsupported Lua version!"
#  endif
#else
#  if defined(BASE_LUA_5_4)
#    include <lua5.4/lua.h>
#    include <lua5.4/lualib.h>
#    include <lua5.4/lauxlib.h>
#  elif defined(BASE_LUA_5_3)
#    include <lua5.3/lua.h>
#    include <lua5.3/lualib.h>
#    include <lua5.3/lauxlib.h>
#  else
#    error "Unsupported Lua version!"
#  endif
#endif

#define BASE_LUA_MALLOC_FAIL(L)      	luaL_error(L, "malloc failed.")
#define BASE_LUA_STACK_FAIL(L, n)    	luaL_stack(L, n, "Stack failed to grow.")
#define BASE_LUA_NUM_METHODS(reg_array) ((sizeof(reg_array) / sizeof(luaL_Reg)) - 1)
#define BASE_LUA_MT_SELF_INDEX(L) do { \
	lua_pushvalue(L, -1); \
	lua_setfield(L, -2, "__index"); \
} while (0)
#define BASE_LUA_LOAD_SUBMODULE(L, mainModule, subModule) do { \
	lua_pushcfunction(L, &luaopen_##mainModule##_##subModule); \
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) \
		return luaL_error(L, "Failed to load submodule %s.", BASE_STRINGIFY(subModule)); \
	lua_setfield(L, -2, BASE_STRINGIFY(subModule)); \
} while (0)
#define BASE_LUA_LOAD_MODULE(L, module) do { \
	lua_pushcfunction(L, &luaopen_##module); \
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) \
		return luaL_error(L, "Failed to load main module %s.", BASE_STRINGIFY(module)); \
} while (0)
#define BASE_LUA_LOAD_FREE_PROC(L, c_proc, lua_func_str) do { \
	lua_pushcfunction(L, &c_proc); \
	lua_setfield(L, -2, lua_func_str); \
} while (0)

#if defined(BASE_LUA_5_4)
#  define BASE_LUA_NEW_UD(L, type)	(type*)lua_newuserdatauv(L, sizeof(type), 0)
#elif defined(BASE_LUA_5_3)
#  define BASE_LUA_NEW_UD(L, type)	(type*)lua_newuserdata(L, sizeof(type))
#else
#  error "Unsupported Lua Version!"
#endif

#define BASE_LUA_CHECK_UD(L, idx, type, mt)	(type*)luaL_checkudata(L, idx, mt)
#define BASE_LUA_TEST_UD(L,  idx, type, mt)	(type*)luaL_testudata(L, idx, mt)
#define BASE_LUA_UD_FAIL(L, idx, func)		luaL_error(L, "%s: Invalid pointer for arg %d", func, idx)
#endif
