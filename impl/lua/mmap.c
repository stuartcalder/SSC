/* Copyright 2021 Stuart Calder */
#include <Base/lua/lua.h>
#include <Base/mmap.h>
#include <Base/lua/mmap.h>
#include <Base/lua/procs.h>

#define MFAIL_(s)           BASE_LUA_MALLOC_FAIL(s)
#define FILE_KEY_			BASE_LUA_FILE_KEY
#define FILE_NEW_(s)        BASE_LUA_FILE_NEW(s)
#define FILE_CHECK_(s, idx) BASE_LUA_FILE_CHECK(s, idx)
#define FILE_TEST_(s, idx)  BASE_LUA_FILE_TEST(s, idx)
#define MMAP_KEY_			BASE_LUA_MMAP_KEY
#define MMAP_NEW_(s)        BASE_LUA_MMAP_NEW(s)
#define MMAP_CHECK_(s, idx) BASE_LUA_MMAP_CHECK(s, idx)
#define MMAP_TEST_(s, idx)  BASE_LUA_MMAP_TEST(s, idx)
typedef Base_Lua_File LF_t; /* Base Lua File Userdata. */
typedef Base_MMap     LM_t; /* Base Lua MMap Userdata. */

static int new_mmap (lua_State* L) {
	LF_t* lf = FILE_TEST_(L, 1); /* Initial file is optional. */
	if (lf && (lf->file == BASE_NULL_FILE))
		return luaL_error(L, "Tried to create new MMap with invalid file!");
	const bool ronly = lua_isboolean(L, 2) ? lua_toboolean(L, 2) : true; /* Defaults readonly. */
	LM_t* map = MMAP_NEW_(L);
	*map = BASE_MMAP_NULL_LITERAL;
	if (lf) {
		map->file = lf->file;
		if (Base_get_file_size(map->file, &map->size) ||
		    Base_MMap_map(map, ronly))
		{
			lua_pushnil(L);
		}
	}
	luaL_getmetatable(L, MMAP_KEY_);
	lua_setmetatable(L, -2);
	return 1;
}

static int ptr_mmap (lua_State* L) {
	LM_t* map = MMAP_CHECK_(L, 1);
	lua_pushlightuserdata(L, map->ptr);
	return 1;
}

static int size_mmap (lua_State* L) {
	LM_t* map = MMAP_CHECK_(L, 1);
	lua_pushinteger(L, (lua_Integer)map->size);
	return 1;
}

static int is_mapped (lua_State* L) {
	LM_t* map = MMAP_CHECK_(L, 1);
	lua_pushboolean(L, (bool)map->ptr);
	return 1;
}

static int readonly_mmap (lua_State* L) {
	LM_t* map = MMAP_CHECK_(L, 1);
	lua_pushboolean(L, (bool)map->readonly);
	return 1;
}

static int del_mmap (lua_State* L) {
	LM_t* map = MMAP_CHECK_(L, 1);
	if (map->ptr) {
		if (Base_MMap_unmap(map))
			return luaL_error(L, "Base_MMap_unmap failed!");
		*map = BASE_MMAP_NULL_LITERAL;
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
	/* Create MMap metatable. */
	luaL_newmetatable(L, MMAP_KEY_);
	/* Populate it with metamethods. */
	luaL_setfuncs(L, mmap_methods, 0);
	/* MMap's metatable points to itself. */
	BASE_LUA_MT_SELF_INDEX(L);
	luaL_newlib(L, free_procs);
	return 1;
}
