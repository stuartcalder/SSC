#include "macros.h"
#include "files.h"
#include "lua/macros.h"

#define MFAIL_(s)           BASE_LUA_MALLOC_FAIL(s)
#define SFAIL_(s, n)        BASE_LUA_STACK_FAIL(s, n)
#define FILE_MT_            BASE_LUA_FILE_MT
#define FILE_NEW_(s)        BASE_LUA_FILE_NEW(s)
#define FILE_CHECK_(s, idx) BASE_LUA_FILE_CHECK(s, idx)

typedef Base_Lua_File File_t;
/*
 * File procedures.
 */
static int open_filepath (lua_State* s) {
	SFAIL_(s, 2);
	const char* fpath = luaL_checkstring(s, 1);
	const bool ronly = lua_isboolean(s, 2) ? lua_toboolean(s, 2) : true;
	File_t* lf = FILE_NEW_(s);
	if (Base_open_filepath(fpath, ronly, &lf->f)) {
		lf->valid = 0;
		lua_pushnil(s);
	} else {
		lf->valid = 1;
		luaL_getmetatable(s, FILE_MT_);
		lua_setmetatable(s, -2);
	}
	return 1;
}

static int create_filepath (lua_State* s) {
	SFAIL_(s, 2);
	const char* fpath = luaL_checkstring(s, 1);
	File_t* file = FILE_NEW_(s);
	if (Base_create_filepath(fpath, &(file->f))) {
		file->valid = 0;
		lua_pushnil(s);
	} else {
		file->valid = 1;
		luaL_getmetatable(s, FILE_MT_);
		lua_setmetatable(s, -2);
	}
	return 1;
}

static int get_filepath_size (lua_State* s) {
	SFAIL_(s, 1); /* Push 1 value. */
	const char* fpath = luaL_checkstring(s, 1);
	size_t sz;
	if (Base_get_filepath_size(fpath, &sz))
		lua_pushnil(s);
	else
		lua_pushunsigned(s, (lua_Unsigned)sz);
	return 1;
}

static int get_file_size (lua_State* s) {
	SFAIL_(s, 1);
	File_t* file = FILE_CHECK_(s, 1);
	size_t sz;
	if (!file->valid || Base_get_file_size(&file->f, &sz))
		lua_pushnil(s);
	else
		lua_pushunsigned(s, (lua_Unsigned)sz);
	return 1;
}

static int close_file (lua_State* s) {
	SFAIL_(s, 1);
	File_t* file = FILE_CHECK_(s, 1);
	if (!file->valid || Base_close_file(&file->f))
		lua_pushnil(s);
	else {
		file->valid = 0;
		lua_pushinteger(s, 1);
	}
	return 1;
}

static int filepath_exists (lua_State* s) {
	SFAIL_(s, 1); /* Push 1 value. */
	const char* fpath  = luaL_checkstring(s, 1);
	lua_pushboolean(s, Base_filepath_exists(fpath));
	return 1;
}

static int file_is_open (lua_State* s) {
	SFAIL_(s, 1);
	File_t* file = FILE_CHECK_(s, 1);
	lua_pushboolean(s, file->valid && file->f != BASE_NULL_FILE);
	return 1;
}

#define UDATA_FAIL_(s, idx, f) luaL_error(s, "%s: Invalid pointer for arg %d", f, idx)

/*
 * C std library.
 */
static int base_memcpy (lua_State* s) {
	void*   dest;
	void*   src;
	size_t  n;
	if (!(dest = lua_touserdata(s, 1)))
		return UDATA_FAIL_(s, 1, "memcpy");
	if (!(src = lua_touserdata(s, 2)))
		return UDATA_FAIL_(s, 2, "memcpy");
	n = (size_t)luaL_checkunsigned(s, 3);
	memcpy(dest, src, n);
	return 0;
}

static int base_memset (lua_State* s) {
	void*  p;
	int    b;
	size_t n;
	if (!(p = lua_touserdata(s, 1)))
		return UDATA_FAIL_(s, 1, "memset");
	b = (int)(luaL_checkunsigned(s, 2) & 0xff);
	n = (size_t)luaL_checkunsigned(s, 3);
	memset(p, b, n);
	return 0;
}

static const luaL_Reg procs[] = {
/* File procedures. */
	{"open_filepath"    , &open_filepath},
	{"create_filepath"  , &create_filepath},
	{"get_filepath_size", &get_filepath_size},
	{"get_file_size"    , &get_file_size},
	{"set_file_size"    , &set_file_size},
	{"close_file"       , &close_file},
	{"filepath_exists"  , &filepath_exists},
	{"file_is_open"     , &file_is_open},
/* C std library. */
	{"memcpy"           , &base_memcpy},
	{"memset"           , &base_memset},
	{NULL, NULL}
};

int luaopen_Base_Procs (lua_State* s) {
	SFAIL_(s, 2);
	luaL_newmetatable(s, FILE_MT_);
	lua_pushcfunction(s, &close_file);
	lua_setfield(s, -2, "__gc");
	luaL_newlib(s, procs);
	return 1;
}
