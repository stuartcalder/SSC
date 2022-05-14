/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#if !defined(BASE_LUA_MACROS_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_MACROS_H

#include <Base/macros.h>

#define BASE_LUA_VERSION_MAJOR(Major) (100000L * (Major))
#define BASE_LUA_VERSION_MINOR(Minor) (100L    * (Minor))
#define BASE_LUA_5_4 (BASE_LUA_VERSION_MAJOR(5) + BASE_LUA_VERSION_MINOR(4))
#define BASE_LUA_5_3 (BASE_LUA_VERSION_MAJOR(5) + BASE_LUA_VERSION_MINOR(3))

#if defined(BASE_EXTERN_LUA_5_4)
#  define BASE_LUA BASE_LUA_5_4
#elif defined(BASE_EXTERN_LUA_5_3)
#  define BASE_LUA BASE_LUA_5_3
#else
#  error "Unsupported Lua version!"
#endif

#ifdef __has_include
# if   (BASE_LUA == BASE_LUA_5_4)
#  if (__has_include(<lua-5.4/lua.h>) && \
       __has_include(<lua-5.4/lualib.h>) && \
       __has_include(<lua-5.4/lauxlib.h>))
#   include <lua-5.4/lua.h>
#   include <lua-5.4/lualib.h>
#   include <lua-5.4/lauxlib.h>
#  elif (__has_include(<lua5.4/lua.h>) && \
         __has_include(<lua5.4/lualib.h>) && \
	 __has_include(<lua5.4/lauxlib.h>))
#   include <lua5.4/lua.h>
#   include <lua5.4/lualib.h>
#   include <lua5.4/lauxlib.h>
#  else
#   error "Error: No appropriate headers found for Lua 5.4"
#  endif
# elif (BASE_LUA == BASE_LUA_5_3)
#  if (__has_include(<lua-5.3/lua.h>) && \
       __has_include(<lua-5.3/lualib.h>) && \
       __has_include(<lua-5.3/lauxlib.h>))
#   include <lua-5.3/lua.h>
#   include <lua-5.3/lualib.h>
#   include <lua-5.3/lauxlib.h>
#  elif (__has_include(<lua5.3/lua.h>) && \
         __has_include(<lua5.3/lualib.h>) && \
	 __has_include(<lua5.3/lauxlib.h>))
#   include <lua5.3/lua.h>
#   include <lua5.3/lualib.h>
#   include <lua5.3/lauxlib.h>
#  else
#   error "Error: No appropriate headers found for Lua 5.3"
#  endif
# else
#  error "Invalid Lua version!"
# endif
#else /* !defined(__has_include) */
# ifdef __OpenBSD__
#  if   (BASE_LUA == BASE_LUA_5_4)
#   include <lua-5.4/lua.h>
#   include <lua-5.4/lualib.h>
#   include <lua-5.4/lauxlib.h>
#  elif (BASE_LUA == BASE_LUA_5_3)
#   include <lua-5.3/lua.h>
#   include <lua-5.3/lualib.h>
#   include <lua-5.3/lauxlib.h>
#  else
#   error "Unsupported Lua version!"
#  endif
# else
#  if (BASE_LUA == BASE_LUA_5_4)
#   include <lua5.4/lua.h>
#   include <lua5.4/lualib.h>
#   include <lua5.4/lauxlib.h>
#  elif (BASE_LUA == BASE_LUA_5_3)
#   include <lua5.3/lua.h>
#   include <lua5.3/lualib.h>
#   include <lua5.3/lauxlib.h>
#  else
#   error "Unsupported Lua version!"
#  endif
# endif
#endif

#define BASE_LUA_MALLOC_FAIL(L)      	luaL_error(L, "malloc failed.")
#define BASE_LUA_STACK_FAIL(L, N)    	luaL_stack(L, N, "Stack failed to grow.")
#define BASE_LUA_NUM_METHODS(RegArray) ((sizeof(RegArray) / sizeof(luaL_Reg)) - 1)
#define BASE_LUA_MT_SELF_INDEX(L) do { \
	lua_pushvalue(L, -1); \
	lua_setfield(L, -2, "__index"); \
} while (0)
#define BASE_LUA_LOAD_SUBMODULE(L, MainModule, SubModule) do { \
  lua_pushcfunction(L, luaopen_##MainModule##_##SubModule); \
  if (lua_pcall(L, 0, 1, 0) != LUA_OK) \
    return luaL_error(L, ""); \
  lua_setfield(L, -2, BASE_STRINGIFY(SubModule)); \
} while (0)
#define BASE_LUA_LOAD_MODULE(L, Module) do { \
	lua_pushcfunction(L, luaopen_##Module); \
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) \
		return luaL_error(L, "Failed to load main module %s.", BASE_STRINGIFY(Module)); \
} while (0)
#define BASE_LUA_LOAD_FREE_PROC(L, C_Proc, Lua_Func_Str) do { \
  lua_pushcfunction(L, C_Proc); \
  lua_setfield(L, -2, Lua_Func_Str); \
} while (0)

#if (BASE_LUA >= BASE_LUA_5_4)
# define BASE_LUA_NEW_UD(L, Type) (Type*)lua_newuserdatauv(L, sizeof(Type), 0)
#elif (BASE_LUA == BASE_LUA_5_3)
# define BASE_LUA_NEW_UD(L, Type) (Type*)lua_newuserdata(L, sizeof(Type))
#else
# error "Unsupported Lua version!"
#endif

#define BASE_LUA_CHECK_UD(L, Idx, Type, Mt) (Type*)luaL_checkudata(L, Idx, Mt)
#define BASE_LUA_TEST_UD(L, Idx, Type, Mt)  (Type*)luaL_testudata(L, Idx, Mt)
#define BASE_LUA_UD_FAIL(L, Idx, Func)      luaL_error(L, "%s: Invalid pointer for arg %d", Func, Idx)
#endif
