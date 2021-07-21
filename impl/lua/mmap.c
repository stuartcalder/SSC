/* Copyright 2021 Stuart Calder */
#include <Base/lua/lua.h>
#include <Base/mmap.h>
#include <Base/lua/mmap.h>
#include <Base/lua/procs.h>

#define FILE_MT_		BASE_LUA_FILE_MT
#define FILE_NEW_(L)		BASE_LUA_FILE_NEW(L)
#define FILE_CHECK_(L, idx)	BASE_LUA_FILE_CHECK(L, idx)
#define FILE_TEST_(L, idx)	BASE_LUA_FILE_TEST(L, idx)
#define FILE_NULL_		BASE_LUA_FILE_NULL_LITERAL
#define MMAP_MT_		BASE_LUA_MMAP_MT
#define MMAP_NEW_(L)		BASE_LUA_MMAP_NEW(L)
#define MMAP_CHECK_(L, idx)	BASE_LUA_MMAP_CHECK(L, idx)
#define MMAP_TEST_(L, idx)	BASE_LUA_MMAP_TEST(L, idx)
#define MMAP_NULL_		BASE_MMAP_NULL_LITERAL
typedef Base_Lua_File File_t_; /* Base Lua File Userdata. */
typedef Base_MMap     MMap_t_; /* Base MMap     Userdata. */

static int new_mmap (lua_State* L) {
	File_t_* f = FILE_TEST_(L, 1); /* Initial file is optional. */
	if (f && (f->file == BASE_NULL_FILE))
		return luaL_error(L, "Tried to create new MMap with invalid file!");
	const bool ronly = lua_isboolean(L, 2) ? lua_toboolean(L, 2) : true; /* Defaults readonly. */
	MMap_t_* map = MMAP_NEW_(L);
	*map = MMAP_NULL_;
	if (f) {
		map->file = f->file;
		if (Base_get_file_size(map->file, &map->size) ||
		    Base_MMap_map(map, ronly))
		{
			lua_pushnil(L);
			return 1;
		} else {
			f->file = BASE_NULL_FILE;
		}
	}
	luaL_getmetatable(L, MMAP_MT_);
	lua_setmetatable(L, -2);
	return 1;
}

static int ptr_mmap (lua_State* L) {
	MMap_t_* map = MMAP_CHECK_(L, 1);
	lua_pushlightuserdata(L, map->ptr);
	return 1;
}

static int size_mmap (lua_State* L) {
	MMap_t_* map = MMAP_CHECK_(L, 1);
	lua_pushinteger(L, (lua_Integer)map->size);
	return 1;
}

static int is_mapped (lua_State* L) {
	MMap_t_* map = MMAP_CHECK_(L, 1);
	lua_pushboolean(L, (bool)map->ptr);
	return 1;
}

static int readonly_mmap (lua_State* L) {
	MMap_t_* map = MMAP_CHECK_(L, 1);
	lua_pushboolean(L, (bool)map->readonly);
	return 1;
}

static int del_mmap (lua_State* L) {
	MMap_t_* map = MMAP_CHECK_(L, 1);
	if (map->ptr) {
		if (Base_MMap_unmap(map))
			return luaL_error(L, "Base_MMap_unmap failed!");
		*map = MMAP_NULL_;
	}
	return 0;
}

static const luaL_Reg mmap_methods[] = {
	{"ptr", &ptr_mmap},
	{"size", &size_mmap},
	{"is_mapped", &is_mapped},
	{"del", &del_mmap},
	{"readonly", &readonly_mmap},
	{"__gc", &del_mmap},
	{"__close", &del_mmap},
	{NULL, NULL}
};

static const luaL_Reg free_procs[] = {
	{"new", &new_mmap},
	{NULL, NULL}
};

int luaopen_Base_MMap (lua_State* L) {
	if (luaL_newmetatable(L, MMAP_MT_)) {
		luaL_setfuncs(L, mmap_methods, 0);
		BASE_LUA_MT_SELF_INDEX(L);
	}
	luaL_newlib(L, free_procs);
	return 1;
}
