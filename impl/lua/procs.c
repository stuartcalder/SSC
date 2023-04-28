/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include <stdbool.h>
#include <Base/print.h>
#include <Base/lua/lua.h>
#include <Base/lua/macros.h>
#include <Base/lua/procs.h>

#define MT_            BASE_LUA_FILE_MT
#define NEW_(L)        BASE_LUA_FILE_NEW(L)
#define CHECK_(L, idx) BASE_LUA_FILE_CHECK(L, idx)
#define TEST_(L, idx)  BASE_LUA_FILE_TEST(L, idx)
#define NULL_          BASE_LUA_FILE_NULL_LITERAL

#define MFAIL_(L)      BASE_LUA_MALLOC_FAIL(L)

typedef Base_Lua_File File_t;

static int fpath_open (lua_State* L)
{
  size_t fpath_n;
  const char* const fpath = luaL_checklstring(L, 1, &fpath_n);
  const bool ronly = (bool)(lua_isboolean(L, 2) ? lua_toboolean(L, 2) : true);
  File_t* f = NEW_(L);
  if (Base_open_filepath(fpath, ronly, &f->file)) {
    *f = NULL_;
    lua_pushnil(L);
  } else {
    if (Base_get_file_size(f->file, &f->file_n)) {
      *f = NULL_;
      return luaL_error(L, "Base_get_file_size failed.");
    }
    if (!(f->fpath = (char*)malloc(fpath_n + 1))) {
      *f = NULL_;
      return luaL_error(L, "malloc failed.");
    }
    memcpy(f->fpath, fpath, fpath_n + 1);
    f->fpath_n = fpath_n;
    f->readonly = (uint8_t)ronly;
    luaL_getmetatable(L, MT_);
    lua_setmetatable(L, -2);
  }
  return 1;
}

/* Lua Args: (char*)fpath
 * Returns: File_t*, or nil on failure. */
static int fpath_create (lua_State* L)
{
  size_t fpath_n;
  const char* const fpath = luaL_checklstring(L, 1, &fpath_n);
  File_t* const f = NEW_(L);
  if (Base_create_filepath(fpath, &f->file)) {
    *f = NULL_;
    lua_pushnil(L);
  } else {
    f->file_n = BASE_FILES_DEFAULT_NEWFILE_SIZE;
    if (!(f->fpath = (char*)malloc(fpath_n + 1)))
      return BASE_LUA_MALLOC_FAIL(L);
    memcpy(f->fpath, fpath, fpath_n + 1);
    f->fpath_n = fpath_n;
    f->readonly = UINT8_C(0);
    luaL_getmetatable(L, MT_);
    lua_setmetatable(L, -2);
  }
  return 1;
}

/* Lua Args: (char*)fpath
 * Returns: the size of the file described by `fpath`, or nil on failure. */
static int fpath_size (lua_State* L)
{
  const char* const fpath = luaL_checkstring(L, 1);
  size_t sz;
  if (Base_get_filepath_size(fpath, &sz))
    lua_pushnil(L);
  else
    lua_pushinteger(L, (lua_Integer)sz);
  return 1;
}

/* Lua Args: (File_t*)f
 * Returns: the size of the file, or nil on failure. */
static int get_file_size (lua_State* L)
{
  const File_t* const f = CHECK_(L, 1);
  if (f->file != BASE_FILE_NULL_LITERAL)
    lua_pushinteger(L, (lua_Integer)f->file_n);
  else
    lua_pushnil(L);
  return 1;
}

/* Lua Args: (File_t*)f
 * Returns: true if the file is successfully closed, or if it was already closed. */
static int close_file (lua_State* L)
{
  File_t* const f = CHECK_(L, 1);
  int ok = 1;
  if ((f->file != BASE_FILE_NULL_LITERAL) && Base_close_file(f->file))
    ok = 0;
  /* Do not assume that if (f->file == BASE_FILE_NULL_LITERAL)
   * that f->fpath is invalid data. It is possible for
   * other code to take ownership over the file handle,
   * and they will set f->file to BASE_FILE_NULL_LITERAL when
   * they do so. */
  if (f->fpath) {
    memset(f->fpath, 0, f->fpath_n);
    free(f->fpath);
  }
  *f = NULL_;
  lua_pushboolean(L, ok);
  return 1;
}

/* Lua Args: (char*)fpath
 * Returns: true when the filepath exists, false otherwise. */
static int fpath_exists (lua_State* L)
{
  const char* const fpath = luaL_checkstring(L, 1);
  lua_pushboolean(L, Base_filepath_exists(fpath));
  return 1;
}

/* Lua Args: (File_t*)f
 * Return: true when file is valid and open, false otherwise. */
static int file_is_open (lua_State* L)
{
  const File_t* const f = CHECK_(L, 1);
  lua_pushboolean(L, f->file != BASE_FILE_NULL_LITERAL);
  return 1;
}

/* Lua Args: (File_t*)f, (lua_Integer)n
 * Return: true on success; false on failure. */
static int file_set_size (lua_State* L)
{
  File_t* const f = CHECK_(L, 1);
  if (f->file == BASE_FILE_NULL_LITERAL) {
    lua_pushboolean(L, 0);
    return 1;
  }
  const size_t n = (size_t)luaL_checkinteger(L, 2);
  if (Base_set_file_size(f->file, n)) {
    lua_pushboolean(L, 0);
    return 1;
  }
  f->file_n = n;
  lua_pushboolean(L, 1);
  return 1;
}

static int file_fpath (lua_State* L)
{
  const File_t* const f = CHECK_(L, 1);
  if (f->fpath)
    lua_pushlstring(L, f->fpath, f->fpath_n);
  else
    lua_pushnil(L);
  return 1;
}

static int file_readonly (lua_State* L)
{
  const File_t* const f = CHECK_(L, 1);
  lua_pushboolean(L, f->readonly);
  return 1;
}

static int fpath_del (lua_State* L)
{
  const char* const fpath = luaL_checkstring(L, 1);
  if (!Base_filepath_exists(fpath) || remove(fpath))
    lua_pushboolean(L, 0);
  else
    lua_pushboolean(L, 1);
  return 1;
}

#define UDATA_FAIL_(L, idx, f) luaL_error(L, "%s: Invalid pointer for arg %d", f, idx)

static int std_memcpy (lua_State* L)
{
  void *dest;
  void *src;
  size_t n;
  if (!(dest = lua_touserdata(L, 1)))
    return UDATA_FAIL_(L, 1, "memcpy");
  if (!(src = lua_touserdata(L, 2)))
    return UDATA_FAIL_(L, 2, "memcpy");
  n = (size_t)luaL_checkinteger(L, 3);
  memcpy(dest, src, n);
  return 0;
}

static int std_memmove(lua_State* L)
{
  void* dest;
  void* src;
  size_t n;
  if (!(dest = lua_touserdata(L, 1)))
    return UDATA_FAIL_(L, 1, "memmove");
  if (!(src = lua_touserdata(L, 2)))
    return UDATA_FAIL_(L, 2, "memmove");
  n = (size_t)luaL_checkinteger(L, 3);
  memmove(dest, src, n);
  return 0;
}

static int std_memset (lua_State* L)
{
  void*  p;
  int    b;
  size_t n;
  if (!(p = lua_touserdata(L, 1)))
    return UDATA_FAIL_(L, 1, "memset");
  b = (int)((lua_Unsigned)luaL_checkinteger(L, 2) & (lua_Unsigned)0xff);
  n = (size_t)luaL_checkinteger(L, 3);
  memset(p, b, n);
  return 0;
}

static int print_bytes (lua_State* L)
{
  uint8_t* p;
  size_t   n;
  if (!(p = (uint8_t*)lua_touserdata(L, 1)))
    return UDATA_FAIL_(L, 1, "print_bytes");
  n = (size_t)luaL_checkinteger(L, 2);
  Base_print_bytes(p, n);
  return 0;
}

static int ptr_add(lua_State* L)
{
  uint8_t*    p;
  lua_Integer n;
  if (!(p = (uint8_t*)lua_touserdata(L, 1)))
    return UDATA_FAIL_(L, 1, "ptr_add");
  n = luaL_checkinteger(L, 2);
  lua_pushlightuserdata(L, p + n);
  return 1;
}

static int ptr_sub(lua_State* L)
{
  uint8_t*    p;
  lua_Integer n;
  if (!(p = (uint8_t*)lua_touserdata(L, 1)))
    return UDATA_FAIL_(L, 1, "ptr_sub");
  n = luaL_checkinteger(L, 2);
  lua_pushlightuserdata(L, p - n);
  return 1;
}

static const luaL_Reg file_methods[] = {
  {"size"    , get_file_size},
  {"close"   , close_file},
#if BASE_LUA >= BASE_LUA_5_4
  {"__close" , close_file},
#endif
  {"__gc"    , close_file},
  {"is_open" , file_is_open},
  {"fpath"   , file_fpath},
  {"readonly", file_readonly},
  {"set_size", file_set_size},
  {BASE_NULL , BASE_NULL}
};

static const luaL_Reg free_procs[] = {
  {"fpath_open"    , fpath_open},
  {"fpath_create"  , fpath_create},
  {"fpath_size"    , fpath_size},
  {"fpath_exists"  , fpath_exists},
  {"fpath_del"     , fpath_del},
  {"memcpy"        , std_memcpy},
  {"memset"        , std_memset},
  {"memmove"       , std_memmove},
  {"print_bytes"   , print_bytes},
  {"ptr_add"       , ptr_add},
  {"ptr_sub"       , ptr_sub},
  {BASE_NULL       , BASE_NULL}
};

int luaopen_Base_Procs (lua_State* L)
{
  if (luaL_newmetatable(L, MT_)) {
    luaL_setfuncs(L, file_methods, 0);
    BASE_LUA_MT_SELF_INDEX(L);
  }
  luaL_newlib(L, free_procs);
  return 1;
}
