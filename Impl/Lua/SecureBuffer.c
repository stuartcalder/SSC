/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include <SSC/Lua/Lua.h>
#include <SSC/Lua/SecureBuffer.h>

#define MT_             SSC_LUA_SECUREBUFFER_MT
#define NEW_(L)         SSC_LUA_SECUREBUFFER_NEW(L)
#define CHECK_(L, Idx)  SSC_LUA_SECUREBUFFER_CHECK(L, Idx)
#define TEST_(L, Idx)   SSC_LUA_SECUREBUFFER_TEST(L, Idx)
#define NULL_LITERAL_   SSC_LUA_SECUREBUFFER_NULL_LITERAL

#define IS_LOCKED_      SSC_LUA_SECUREBUFFER_MEM_IS_LOCKED
#define IS_ALIGNED_     SSC_LUA_SECUREBUFFER_MEM_IS_ALIGNED

#define MALLOC_FAIL_(L) SSC_LUA_MALLOC_FAIL(L)

typedef SSC_Lua_SecureBuffer SecureBuffer_t;

static int securebuffer_new(lua_State* L)
{
  const size_t n = (size_t)luaL_checkinteger(L, 1);
  SecureBuffer_t* sb = NEW_(L);
  sb->n = n;
#ifdef SSC_MEMLOCK_H
  sb->f = UINT8_C(0);
  if (lua_isboolean(L, 2) ? lua_toboolean(L, 2) : 0) {
    sb->f |= IS_ALIGNED_;
    if (!(sb->p = (uint8_t*)SSC_alignedMalloc(SSC_MemLock_Global.page_size, sb->n)))
      return luaL_error(L, "%s failed!", "SSC_alignedMalloc");
    switch (SSC_MemLock_lock(sb->p, sb->n)) {
      case 0:
        sb->f |= IS_LOCKED_;
        break;
      case SSC_MEMLOCK_ERR_OVER_MEMLIMIT:
        /* Fail to lock silently when we cannot lock anymore. */
        break;
      case SSC_MEMLOCK_ERR_LOCK_OP:
        return luaL_error(L, "%s failed with error `%s'.", "SSC_MemLock", "MLock Op");
 #ifdef SSC_EXTERN_MEMLOCK_THREADSAFE
      case SSC_MEMLOCK_ERR_MTX_OP:
        return luaL_error(L, "%s failed with error `%s'.", "SSC_MemLock", "Mutex Op");
 #endif
      default:
        return luaL_error(L, "%s failed with error `%s'.", "SSC_MemLock", "Invalid SSC_MemLock_lock Retcode");
    }
  }
  else {
    if (!(sb->p = (uint8_t*)malloc(sb->n)))
      return MALLOC_FAIL_(L);
  }
#else /* Memory-Locking disabled. */
  if (!(sb->p = (uint8_t*)malloc(sb->n)))
    return MALLOC_FAIL_(L);
#endif
  luaL_getmetatable(L, MT_);
  lua_setmetatable(L, -2);
  return 1;
}

static int securebuffer_del(lua_State* L)
{
  SecureBuffer_t* sb = CHECK_(L, 1);
  if (sb->p) {
    memset(sb->p, 0, sb->n);
#ifdef SSC_MEMLOCK_H
    if ((sb->f & IS_LOCKED_) && SSC_MemLock_unlock(sb->p, sb->n))
      return luaL_error(L, "SSC_MemLock_unlock failed!");
 #ifdef SSC_ALIGNED_FREE_IS_POSIX_FREE
    free(sb->p);
 #else
    if (sb->f & IS_ALIGNED_)
      SSC_alignedFree(sb->p);
    else
      free(sb->p);
 #endif
#else
    free(sb->p);
#endif
    *sb = NULL_LITERAL_;
  }
  return 0;
}

static int securebuffer_ptr(lua_State* L)
{
  SecureBuffer_t* sb = CHECK_(L, 1);
  if (sb->p)
    lua_pushlightuserdata(L, sb->p);
  else
    lua_pushnil(L);
  return 1;
}

static int securebuffer_size(lua_State* L)
{
  SecureBuffer_t* sb = CHECK_(L, 1);
  if (sb->p)
    lua_pushinteger(L, (lua_Integer)sb->n);
  else
    lua_pushnil(L);
  return 1;
}

static int securebuffer_has_memlock(lua_State* L)
{
#ifdef SSC_MEMLOCK_H
  lua_pushboolean(L, 1);
#else /* No memorylocking. */
  lua_pushboolean(L, 0);
#endif
  return 1;
}

#ifdef SSC_MEMLOCK_H
static int securebuffer_is_memlocked(lua_State* L)
{
  SecureBuffer_t* sb = CHECK_(L, 1);
  lua_pushboolean(L, (sb->p != NULL) && (sb->f & IS_LOCKED_));
  return 1;
}
static int securebuffer_is_aligned(lua_State* L)
{
  SecureBuffer_t* sb = CHECK_(L, 1);
  lua_pushboolean(L, (sb->p != NULL) && (sb->f & IS_ALIGNED_));
  return 1;
}
#else
static const lua_CFunction securebuffer_is_memlocked = securebuffer_has_memlock;
static const lua_CFunction securebuffer_is_aligned   = securebuffer_has_memlock;
#endif

#ifdef SSC_EXTERN_DEBUG
static int securebuffer_debug (lua_State* L)
{
  lua_createtable(L, 0, 1); /* 1: Table of tables to return. */
  {
    lua_createtable(L, 0, 3); /* 2: Table holding SSC_MemLock_g. */
    {
      lua_pushinteger(L, (lua_Integer)SSC_MemLock_g.page_size);
      lua_setfield(L, -2, "page_size");
    } {
      lua_pushinteger(L, (lua_Integer)SSC_MemLock_g.limit);
      lua_setfield(L, -2, "limit");
    } {
      lua_pushinteger(L, (lua_Integer)SSC_MemLock_g.n);
      lua_setfield(L, -2, "n");
    }
    lua_setfield(L, -2, "SSC_MemLock_g");
  }
  return 1;
}
#endif

static const luaL_Reg securebuffer_methods[] = {
  {"ptr"       , securebuffer_ptr},
  {"size"      , securebuffer_size},
  {"has_memlock" , securebuffer_has_memlock},
  {"is_memlocked", securebuffer_is_memlocked},
  {"is_aligned", securebuffer_is_aligned},
  {"del"       , securebuffer_del},
  {"__gc"      , securebuffer_del},
#if SSC_LUA >= SSC_LUA_5_4
  {"__close"   , securebuffer_del},
#endif
#ifdef SSC_EXTERN_DEBUG
  {"debug"     , securebuffer_debug},
#endif
  {SSC_NULL    , SSC_NULL}
};

static const luaL_Reg free_procs[] = {
  {"new"      , securebuffer_new},
  {"has_memlock", securebuffer_has_memlock},
  {SSC_NULL   , SSC_NULL}
};

int luaopen_SSC_SBuffer(lua_State* L)
{
#ifdef SSC_MEMLOCK_H
  switch (SSC_MemLock_Global_init()) {
  case 0:
    break;
  case SSC_MEMLOCK_ERR_GET_LIMIT:
    return luaL_error(L, "%s failed! Error: %s", "SSC_MemLock_Global_init", "GET_LIMIT");
  case SSC_MEMLOCK_ERR_SET_LIMIT:
    return luaL_error(L, "%s failed! Error: %s", "SSC_MemLock_Global_init", "SET_LIMIT");
 #ifdef SSC_EXTERN_MEMLOCK_THREADSAFE
  case SSC_MEMLOCK_ERR_MTX_INIT:
    return luaL_error(L, "%s failed! Error: %s", "SSC_MemLock_Global_init", "MTX_INIT");
 #endif
  default:
    return luaL_error(L, "%s failed! Error: %s", "SSC_MemLock_Global_init", "RETCODE");
  }
#endif
  if (luaL_newmetatable(L, MT_)) {
    luaL_setfuncs(L, securebuffer_methods, 0);
    SSC_LUA_MT_SELF_INDEX(L);
  }
  luaL_newlib(L, free_procs);
  return 1;
}
