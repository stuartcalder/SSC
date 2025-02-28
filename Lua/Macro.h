/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#if !defined(SSC_LUA_MACRO_H) && defined(SSC_EXTERN_LUA)
#define SSC_LUA_MACRO_H

#include "../Macro.h"

#define SSC_LUA_VERSION_MAJOR(Major) (100000L * (Major))
#define SSC_LUA_VERSION_MINOR(Minor) (100L    * (Minor))
#define SSC_LUA_5_4 (SSC_LUA_VERSION_MAJOR(5) + SSC_LUA_VERSION_MINOR(4))
#define SSC_LUA_5_3 (SSC_LUA_VERSION_MAJOR(5) + SSC_LUA_VERSION_MINOR(3))

#if defined(SSC_EXTERN_LUA_5_4)
 #define SSC_LUA SSC_LUA_5_4
#elif defined(SSC_EXTERN_LUA_5_3)
 #define SSC_LUA SSC_LUA_5_3
#else
 #error "Unsupported Lua version!"
#endif

#ifdef __has_include
 #if   (SSC_LUA == SSC_LUA_5_4)
  #if (__has_include(<lua-5.4/lua.h>)    &&\
       __has_include(<lua-5.4/lualib.h>) &&\
       __has_include(<lua-5.4/lauxlib.h>))
   #include <lua-5.4/lua.h>
   #include <lua-5.4/lualib.h>
   #include <lua-5.4/lauxlib.h>
  #elif (__has_include(<lua5.4/lua.h>)    &&\
         __has_include(<lua5.4/lualib.h>) &&\
         __has_include(<lua5.4/lauxlib.h>))
   #include <lua5.4/lua.h>
   #include <lua5.4/lualib.h>
   #include <lua5.4/lauxlib.h>
  #elif (__has_include(<lua.h>)    &&\
         __has_include(<lualib.h>) &&\
         __has_include(<lauxlib.h>))
   #include <lua.h>
   #include <lualib.h>
   #include <lauxlib.h>
  #else
   #error "Error: No appropriate headers found for Lua 5.4"
  #endif
 #elif (SSC_LUA == SSC_LUA_5_3)
  #if (__has_include(<lua-5.3/lua.h>) &&\
       __has_include(<lua-5.3/lualib.h>) &&\
       __has_include(<lua-5.3/lauxlib.h>))
   #include <lua-5.3/lua.h>
   #include <lua-5.3/lualib.h>
   #include <lua-5.3/lauxlib.h>
  #elif (__has_include(<lua5.3/lua.h>) &&\
         __has_include(<lua5.3/lualib.h>) &&\
         __has_include(<lua5.3/lauxlib.h>))
   #include <lua5.3/lua.h>
   #include <lua5.3/lualib.h>
   #include <lua5.3/lauxlib.h>
  #elif (__has_include(<lua.h>)    &&\
         __has_include(<lualib.h>) &&\
         __has_include(<lauxlib.h>))
   #include <lua.h>
   #include <lualib.h>
   #include <lauxlib.h>
  #else
   #error "Error: No appropriate headers found for Lua 5.3"
  #endif
 #else
  #if (__has_include(<lua.h>)    &&\
       __has_include(<lualib.h>) &&\
       __has_include(<lauxlib.h>))
   #include <lua.h>
   #include <lualib.h>
   #include <lauxlib.h>
  #else
   #error "Invalid Lua version!"
  #endif
 #endif
#else /* !defined(__has_include) */
 #ifdef __OpenBSD__
  #if   (SSC_LUA == SSC_LUA_5_4)
   #include <lua-5.4/lua.h>
   #include <lua-5.4/lualib.h>
   #include <lua-5.4/lauxlib.h>
  #elif (SSC_LUA == SSC_LUA_5_3)
   #include <lua-5.3/lua.h>
   #include <lua-5.3/lualib.h>
   #include <lua-5.3/lauxlib.h>
  #else
   #error "Unsupported Lua version!"
  #endif
 #else
  #if (SSC_LUA == SSC_LUA_5_4)
   #include <lua5.4/lua.h>
   #include <lua5.4/lualib.h>
   #include <lua5.4/lauxlib.h>
  #elif (SSC_LUA == SSC_LUA_5_3)
   #include <lua5.3/lua.h>
   #include <lua5.3/lualib.h>
   #include <lua5.3/lauxlib.h>
  #else
   #error "Unsupported Lua version!"
  #endif
 #endif
#endif

#define SSC_LUA_MALLOC_FAIL(L)        luaL_error(L, "malloc failed.")
#define SSC_LUA_STACK_FAIL(L, N)      luaL_stack(L, N, "Stack failed to grow.")
#define SSC_LUA_NUM_METHODS(RegArray) ((sizeof(RegArray) / sizeof(luaL_Reg)) - 1)

/* The table at the top of the stack will now index itself. */
#define SSC_LUA_MT_SELF_INDEX(L) do {\
 lua_pushvalue(L, -1);\
 lua_setfield(L, -2, "__index");\
} while (0)

/* Load the @Module. */
#define SSC_LUA_LOAD_MODULE(L, Module) do {\
  lua_pushcfunction(L, luaopen_##Module);\
  if (lua_pcall(L, 0, 1, 0) != LUA_OK)\
    return luaL_error(L, "Failed to load main module %s.", SSC_STRINGIFY(Module));\
} while (0)

/* Load the @SubModule of @MainModule. */
#define SSC_LUA_LOAD_SUBMODULE(L, MainModule, SubModule) do {\
  lua_pushcfunction(L, luaopen_##MainModule##_##SubModule);\
  if (lua_pcall(L, 0, 1, 0) != LUA_OK)\
    return luaL_error(L, "");\
  lua_setfield(L, -2, SSC_STRINGIFY(SubModule));\
} while (0)

/* Load @C_Proc into the table at the top of the stack, and associate
 * it with the name @Lua_Func_Str. */
#define SSC_LUA_LOAD_FREE_PROC(L, C_Proc, Lua_Func_Str) do {\
  lua_pushcfunction(L, C_Proc);\
  lua_setfield(L, -2, Lua_Func_Str);\
} while (0)

#if   SSC_LUA >= SSC_LUA_5_4
 #define SSC_LUA_NEW_UD(L, Type) (Type*)lua_newuserdatauv(L, sizeof(Type), 0)
#elif SSC_LUA == SSC_LUA_5_3
 #define SSC_LUA_NEW_UD(L, Type) (Type*)lua_newuserdata(L, sizeof(Type))
#else
 #error "Unsupported Lua version!"
#endif

/* Ensure there is a Userdata of type @Type with metatable @Mt at @Idx, or raise an error.
 * Return a pointer to the Userdata. */
#define SSC_LUA_CHECK_UD(L, Idx, Type, Mt) (Type*)luaL_checkudata(L, Idx, Mt)
/* Check if there is a Userdata of type @Type with metatable @Mt at @Idx, or return
 * a NULL pointer. */
#define SSC_LUA_TEST_UD(L, Idx, Type, Mt)  (Type*)luaL_testudata(L, Idx, Mt)
#define SSC_LUA_UD_FAIL(L, Idx, FuncStr)   luaL_error(L, "%s: Invalid pointer for arg %d", FuncStr, Idx)
#endif
