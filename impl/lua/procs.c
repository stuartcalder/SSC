/* Copyright 2021 Stuart Calder */
#include <Base/lua/lua.h>
#include <Base/lua/macros.h>
#include <Base/lua/procs.h>

#define MFAIL_(L)           BASE_LUA_MALLOC_FAIL(L)
#define FILE_MT_			BASE_LUA_FILE_KEY
#define FILE_NEW_(L)        BASE_LUA_FILE_NEW(L)
#define FILE_CHECK_(L, idx) BASE_LUA_FILE_CHECK(L, idx)
#define FILE_TEST_(L, idx)	BASE_LUA_FILE_TEST(L, idx)

typedef Base_Lua_File F_t;

static int fpath_open (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	const size_t fpath_n = strlen(fpath);
	const bool ronly = (bool)(lua_isboolean(L, 2) ? lua_toboolean(L, 2) : true);
	F_t* f = FILE_NEW_(L);
	if (Base_open_filepath(fpath, ronly, &f->file)) {
		*f = BASE_LUA_FILE_NULL_LITERAL;
		lua_pushnil(L);
	} else {
		if (!(f->fpath = (char*)malloc(fpath_n + 1)))
			return luaL_error(L, "malloc failed.");
		memcpy(f->fpath, fpath, fpath_n + 1);
		f->fpath_n = fpath_n;
		f->readonly = (uint8_t)ronly;
		luaL_getmetatable(L, FILE_MT_);
		lua_setmetatable(L, -2);
	}
	return 1;
}

static int fpath_create (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	const size_t fpath_n = strlen(fpath);
	F_t* f = FILE_NEW_(L);
	if (Base_create_filepath(fpath, &f->file)) {
		*f = BASE_LUA_FILE_NULL_LITERAL;
		lua_pushnil(L);
	} else {
		if (!(f->fpath = (char*)malloc(fpath_n + 1)))
			return luaL_error(L, "malloc failed.");
		memcpy(f->fpath, fpath, fpath_n + 1);
		f->fpath_n = fpath_n;
		f->readonly = UINT8_C(0);
		luaL_getmetatable(L, FILE_MT_);
		lua_setmetatable(L, -2);
	}
	return 1;
}

static int fpath_size (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	size_t sz;
	if (Base_get_filepath_size(fpath, &sz))
		lua_pushnil(L);
	else
		lua_pushinteger(L, (lua_Integer)sz);
	return 1;
}

static int get_file_size (lua_State* L) {
	F_t* f = FILE_CHECK_(L, 1);
	size_t sz;
	if (f->file == BASE_NULL_FILE || Base_get_file_size(f->file, &sz))
		lua_pushnil(L);
	else
		lua_pushinteger(L, (lua_Integer)sz);
	return 1;
}

static int close_file (lua_State* L) {
	F_t* f = FILE_CHECK_(L, 1);
	if (f->file == BASE_NULL_FILE || Base_close_file(f->file))
		lua_pushnil(L);
	else {
		Base_secure_zero(f->fpath, f->fpath_n);
		free(f->fpath);
		*f = BASE_LUA_FILE_NULL_LITERAL;
		lua_pushinteger(L, 1);
	}
	return 1;
}

static int fpath_exists (lua_State* L) {
	const char* fpath = luaL_checkstring(L, 1);
	lua_pushboolean(L, Base_filepath_exists(fpath));
	return 1;
}

static int file_is_open (lua_State* L) {
	F_t* f = FILE_CHECK_(L, 1);
	lua_pushboolean(L, f->file != BASE_NULL_FILE);
	return 1;
}

static int file_fpath (lua_State* L) {
	F_t* f = FILE_CHECK_(L, 1);
	if (f->fpath)
		lua_pushstring(L, f->fpath);
	else
		lua_pushnil(L);
	return 1;
}

static int file_readonly (lua_State* L) {
	F_t* f = FILE_CHECK_(L, 1);
	lua_pushboolean(L, (bool)f->readonly);
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

#define UDATA_FAIL_(L, idx, f) luaL_error(L, "%s: Invalid pointer for arg %d", f, idx)

static int std_memcpy (lua_State* L) {
	void *dest, *src;
	size_t n;
	if (!(dest = lua_touserdata(L, 1)))
		return UDATA_FAIL_(L, 1, "memcpy");
	if (!(src = lua_touserdata(L, 2)))
		return UDATA_FAIL_(L, 2, "memcpy");
	n = (size_t)luaL_checkinteger(L, 3);
	memcpy(dest, src, n);
	return 0;
}

static int std_memset (lua_State* L) {
	void*  p;
	int    b;
	size_t n;
	if (!(p = lua_touserdata(L, 1)))
		return UDATA_FAIL_(L, 1, "memset");
	b = (int)((lua_Unsigned)luaL_checkinteger(L, 2) & (lua_Unsigned)0xffu);
	n = (size_t)luaL_checkinteger(L, 3);
	memset(p, b, n);
	return 0;
}

static int secure_zero (lua_State* L) {
	void*  p;
	size_t n;
	if (!(p = lua_touserdata(L, 1)))
		return UDATA_FAIL_(L, 1, "secure_zero");
	n = (size_t)luaL_checkinteger(L, 2);
	Base_secure_zero(p, n);
	return 0;
}

static const luaL_Reg file_methods[] = {
	{"size"   , &get_file_size},
	{"close"  , &close_file},
	{"__close", &close_file},
	{"__gc"   , &close_file},
	{"is_open", &file_is_open},
	{"fpath"  , &file_fpath},
	{"readonly", &file_readonly},
	{NULL, NULL}
};

static const luaL_Reg free_procs[] = {
	{"fpath_open"  , &fpath_open},
	{"fpath_create", &fpath_create},
	{"fpath_size"  , &fpath_size},
	{"fpath_exists", &fpath_exists},
	{"fpath_del"   , &fpath_del},
	{"memcpy"      , &std_memcpy},
	{"memset"      , &std_memset},
	{"secure_zero" , &secure_zero},
	{NULL, NULL}
};

int luaopen_Base_Procs (lua_State* L) {
	/* Create file metatable. */
	luaL_newmetatable(L, FILE_MT_);
	/* Populate it with metamethods. */
	luaL_setfuncs(L, file_methods, 0);
	/* Metatable __index -> itself. */
	BASE_LUA_MT_SELF_INDEX(L);
	/* Create a table for free procedures */
	luaL_newlib(L, free_procs);
	return 1;
}
