/* Copyright (c) 2020-2024 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include <stdbool.h>

#include "../../Lua/Lua.h"
#include "../../Lua/MemMap.h"
#include "../../Lua/Proc.h"

#define FILE_MT_              SSC_LUA_FILE_MT
#define FILE_NEW_(L)          SSC_LUA_FILE_NEW(L)
#define FILE_CHECK_(L, Idx)   SSC_LUA_FILE_CHECK(L, Idx)
#define FILE_TEST_(L, Idx)    SSC_LUA_FILE_TEST(L, Idx)
#define FILE_NULL_            SSC_LUA_FILE_NULL_LITERAL

#define MEMMAP_MT_            SSC_LUA_MEMMAP_MT
#define MEMMAP_NEW_(L)        SSC_LUA_MEMMAP_NEW(L)
#define MEMMAP_CHECK_(L, Idx) SSC_LUA_MEMMAP_CHECK(L, Idx)
#define MEMMAP_TEST_(L, Idx)  SSC_LUA_MEMMAP_TEST(L, Idx)
#define MEMMAP_NULL_          SSC_LUA_MEMMAP_NULL_LITERAL

typedef SSC_Lua_File   File_t;   /* SSC Lua File Userdata. */
typedef SSC_Lua_MemMap MemMap_t; /* SSC Lua MemMap Userdata. */

static int memmap_new(lua_State* L)
{
  File_t* f = FILE_TEST_(L, 1); /* Initial file is optional. */
  if (f && (f->file == SSC_FILE_NULL_LITERAL))
    return luaL_error(L, "Tried to create new MemMap with invalid file!");
  const bool ronly = lua_isboolean(L, 2) ? lua_toboolean(L, 2) : true; /* Defaults to readonly. */
  MemMap_t* map = MEMMAP_NEW_(L);
  *map = MEMMAP_NULL_;
  if (f) {
    map->file = f->file;
    if (SSC_File_getSize(map->file, &map->size) || SSC_MemMap_map(map, ronly)) {
      lua_pushnil(L);
      return 1;
    }
    f->file = SSC_FILE_NULL_LITERAL; /* We take ownership. */
  }
  luaL_getmetatable(L, MEMMAP_MT_);
  lua_setmetatable(L, -2);
  return 1;
}
  
static int memmap_ptr(lua_State* L)
{
  MemMap_t* map = MEMMAP_CHECK_(L, 1);
  lua_pushlightuserdata(L, map->ptr);
  return 1;
}

static int memmap_size(lua_State* L)
{
  MemMap_t* map = MEMMAP_CHECK_(L, 1);
  lua_pushinteger(L, (lua_Integer)map->size);
  return 1;
}

static int memmap_is_mapped(lua_State* L)
{
  MemMap_t* map = MEMMAP_CHECK_(L, 1);
  lua_pushboolean(L, map->ptr != SSC_NULL);
  return 1;
}

static int memmap_readonly(lua_State* L)
{
  MemMap_t* map = MEMMAP_CHECK_(L, 1);
  lua_pushboolean(L, map->readonly);
  return 1;
}

static int memmap_del(lua_State* L)
{
  MemMap_t* map = MEMMAP_CHECK_(L, 1);
  if (map->ptr) {
    if (SSC_MemMap_unmap(map))
      return luaL_error(L, "SSC_MemMap_unmap failed!");
    if (SSC_File_close(map->file))
      return luaL_error(L, "SSC_close_file failed!");
    *map = MEMMAP_NULL_;
  }
  return 0;
}

static const luaL_Reg memmap_methods[] = {
  {"ptr"      , memmap_ptr},
  {"size"     , memmap_size},
  {"is_mapped", memmap_is_mapped},
  {"readonly" , memmap_readonly},
  {"del"      , memmap_del},
  {"__gc"     , memmap_del},
#if SSC_LUA >= SSC_LUA_5_4
  {"__close"  , memmap_del},
#endif
  {SSC_NULL   , SSC_NULL}
};

static const luaL_Reg free_procs[] = {
  {"new"     , memmap_new},
  {SSC_NULL  , SSC_NULL}
};

int luaopen_SSC_MemMap(lua_State* L)
{
  if (luaL_newmetatable(L, MEMMAP_MT_)) {
    luaL_setfuncs(L, memmap_methods, 0);
    SSC_LUA_MT_SELF_INDEX(L);
  }
  luaL_newlib(L, free_procs);
  return 1;
}
