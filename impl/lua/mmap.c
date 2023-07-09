/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include <stdbool.h>

#include <Base/lua/lua.h>
#include <Base/lua/mmap.h>
#include <Base/lua/procs.h>

#define FILE_MT_            BASE_LUA_FILE_MT
#define FILE_NEW_(L)        BASE_LUA_FILE_NEW(L)
#define FILE_CHECK_(L, Idx) BASE_LUA_FILE_CHECK(L, Idx)
#define FILE_TEST_(L, Idx)  BASE_LUA_FILE_TEST(L, Idx)
#define FILE_NULL_          BASE_LUA_FILE_NULL_LITERAL

#define MMAP_MT_            BASE_LUA_MMAP_MT
#define MMAP_NEW_(L)        BASE_LUA_MMAP_NEW(L)
#define MMAP_CHECK_(L, Idx) BASE_LUA_MMAP_CHECK(L, Idx)
#define MMAP_TEST_(L, Idx)  BASE_LUA_MMAP_TEST(L, Idx)
#define MMAP_NULL_          BASE_LUA_MMAP_NULL_LITERAL

typedef Base_Lua_File File_t; /* Base Lua File Userdata. */
typedef Base_Lua_MMap MMap_t; /* Base Lua MMap Userdata. */

static int new_mmap(lua_State* L)
{
  File_t* f = FILE_TEST_(L, 1); /* Initial file is optional. */
  if (f && (f->file == BASE_FILE_NULL_LITERAL))
    return luaL_error(L, "Tried to create new MMap with invalid file!");
  const bool ronly = lua_isboolean(L, 2) ? lua_toboolean(L, 2) : true; /* Defaults to readonly. */
  MMap_t* map = MMAP_NEW_(L);
  *map = MMAP_NULL_;
  if (f) {
    map->file = f->file;
    if (Base_get_file_size(map->file, &map->size) || Base_MMap_map(map, ronly))
    {
      lua_pushnil(L);
      return 1;
    } 
    f->file = BASE_FILE_NULL_LITERAL; /* We take ownership. */
  }
  luaL_getmetatable(L, MMAP_MT_);
  lua_setmetatable(L, -2);
  return 1;
}
  
static int mmap_ptr(lua_State* L)
{
  MMap_t* map = MMAP_CHECK_(L, 1);
  lua_pushlightuserdata(L, map->ptr);
  return 1;
}

static int mmap_size(lua_State* L)
{
  MMap_t* map = MMAP_CHECK_(L, 1);
  lua_pushinteger(L, (lua_Integer)map->size);
  return 1;
}

static int mmap_is_mapped(lua_State* L)
{
  MMap_t* map = MMAP_CHECK_(L, 1);
  lua_pushboolean(L, map->ptr != BASE_NULL);
  return 1;
}

static int mmap_readonly(lua_State* L)
{
  MMap_t* map = MMAP_CHECK_(L, 1);
  lua_pushboolean(L, map->readonly);
  return 1;
}

static int mmap_del(lua_State* L)
{
  MMap_t* map = MMAP_CHECK_(L, 1);
  if (map->ptr) {
    if (Base_MMap_unmap(map))
      return luaL_error(L, "Base_MMap_unmap failed!");
    if (Base_close_file(map->file))
      return luaL_error(L, "Base_close_file failed!");
    *map = MMAP_NULL_;
  }
  return 0;
}

static const luaL_Reg mmap_methods[] = {
  {"ptr"      , mmap_ptr},
  {"size"     , mmap_size},
  {"is_mapped", mmap_is_mapped},
  {"readonly" , mmap_readonly},
  {"del"      , mmap_del},
  {"__gc"     , mmap_del},
#if BASE_LUA >= BASE_LUA_5_4
  {"__close"  , mmap_del},
#endif
  {BASE_NULL  , BASE_NULL}
};

static const luaL_Reg free_procs[] = {
  {"new"     , new_mmap},
  {BASE_NULL , BASE_NULL}
};

int luaopen_Base_MMap(lua_State* L)
{
  if (luaL_newmetatable(L, MMAP_MT_)) {
    luaL_setfuncs(L, mmap_methods, 0);
    BASE_LUA_MT_SELF_INDEX(L);
  }
  luaL_newlib(L, free_procs);
  return 1;
}
