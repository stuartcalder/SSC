#include "lua/macros.h"
#include "lua/procs.h"

#define MFAIL_(L)           BASE_LUA_MALLOC_FAIL(L)
#define FILE_MT_            BASE_LUA_FILE_MT
#define FILE_NEW_(L)        BASE_LUA_FILE_NEW(L)
#define FILE_CHECK_(L, idx) BASE_LUA_FILE_CHECK(L, idx)
#define FILE_TEST_(L, idx)	BASE_LUA_FILE_TEST(L, idx)

typedef Base_Lua_File F_t;

static int open_fpath (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	const size_t fpath_n = strlen(fpath);
	const bool  ronly = lua_isboolean(L, 2) ? lua_toboolean(L, 2) : true;
	F_t* f = FILE_NEW_(L);
	#if 1
	if (Base_open_filepath(fpath, ronly, &f->file)) {
		f->file = BASE_NULL_FILE;
		f->fpath = NULL;
		f->fpath_n = 0;
		lua_pushnil(L);
	} else {
		if (!(f->fpath = (char*)malloc(fpath_n + 1)))
			return luaL_error(L, "malloc failed.");
		memcpy(f->fpath, fpath, fpath_n + 1);
		f->fpath_n = fpath_n;
		luaL_getmetatable(L, FILE_MT_);
		lua_setmetatable(L, -2);
	}
	#else
	if (Base_open_filepath(fpath, ronly, &f ->f)) {
		f->valid = 0;
		lua_pushnil(L);
	} else {
		f ->valid = 1;
		luaL_getmetatable(L, FILE_MT_);
		lua_setmetatable(L, -2);
	}
	#endif
	return 1;
}

static int create_fpath (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	const size_t fpath_n = strlen(fpath);
	F_t* f = FILE_NEW_(L);
	#if 1
	if (Base_create_filepath(fpath, &f->file)) {
		f->file = BASE_NULL_FILE;
		f->fpath = NULL;
		f->fpath_n = 0;
		lua_pushnil(L);
	} else {
		if (!(f->fpath = (char*)malloc(fpath_n + 1)))
			return luaL_error(L, "malloc failed.");
		memcpy(f->fpath, fpath, fpath_n + 1);
		f->fpath_n = fpath_n;
	}
	#else
	if (Base_create_filepath(fpath, &f->f)) {
		f->valid = 0;
		lua_pushnil(L);
	} else {
		f->valid = 1;
		luaL_getmetatable(L, FILE_MT_);
		lua_setmetatable(L, -2);
	}
	#endif
	return 1;
}

static int get_fpath_size (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	size_t sz;
	if (Base_get_filepath_size(fpath, &sz))
		lua_pushnil(L);
	else
		lua_pushunsigned(L, (lua_Unsigned)sz);
	return 1;
}

static int get_file_size (lua_State* L) {
	F_t* f = FILE_CHECK_(L, 1);
	size_t sz;
	#if 1
	if (f->file == BASE_NULL_FILE || Base_get_file_size(f->file, &sz))
		lua_pushnil(L);
	else
		lua_pushunsigned(L, (lua_Unsigned)sz);
	#else
	if (!f->valid || Base_get_file_size(f->f, &sz))
		lua_pushnil(L);
	else
		lua_pushunsigned(L, (lua_Unsigned)sz);
	#endif
	return 1;
}

static int close_file (lua_State* L) {
	F_t* f = FILE_CHECK_(L, 1);
	#if 1
	if (f->file == BASE_NULL_FILE || Base_close_file(f->file))
		lua_pushnil(L);
	else {
		f->file = BASE_NULL_FILE;
		Base_secure_zero(f->fpath, f->fpath_n);
		free(f->fpath);
		f->fpath = NULL;
		f->fpath_n = 0;
		lua_pushinteger(L, 1);
	}
	#else
	if (!f->valid || Base_close_file(f->f))
		lua_pushnil(L);
	else {
		f->valid = 0;
		f->f = BASE_NULL_FILE;
		lua_pushinteger(L, 1);
	}
	#endif
	return 1;
}

static int fpath_exists (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	lua_pushboolean(L, Base_filepath_exists(fpath));
	return 1;
}

static int file_is_open (lua_State* L) {
	F_t* f = FILE_CHECK_(L, 1);
	#if 1
	lua_pushboolean(L, f->file != BASE_NULL_FILE);
	#else
	lua_pushboolean(L, f->valid && (f->f != BASE_NULL_FILE));
	#endif
	return 1;
}

static int fpath_del (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	if (!Base_filepath_exists(fpath) || remove(fpath))
		lua_pushnil(L);
	else
		lua_pushnumber(L, 1);
	return 1;
}

static const luaL_Reg file_mt[] = {
	{"size"   , &get_file_size},
	{"close"  , &close_file},
	{"__close", &close_file},
	{"__gc"   , &close_file},
	{"is_open", &file_is_open},
	{NULL, NULL}
}

static const luaL_Reg procedures[] = {
	{"fpath_open"  , &open_fpath},
	{"fpath_create", &create_fpath},
	{"fpath_size"  , &get_fpath_size},
	{"fpath_exists", &fpath_exists},
	{"fpath_del"   , &fpath_del},
	{NULL, NULL}
};

int luaopen_Base_Procs (lua_State* L) {
	/* Create file metatable. */
	luaL_newmetatable(L, FILE_MT_);
	/* Populate it with metamethods. */
	luaL_setfuncs(L, file_mt, 0);
	/* File metatable's __index points to itself. */
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	/* Create a table for our free procedures. */
	luaL_newlib(L, procedures);
	return 1;
}
