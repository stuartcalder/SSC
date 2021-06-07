#include "lua/mmap.h"
#include "lua/procs.h"

#define MFAIL_(s)           BASE_LUA_MALLOC_FAIL(s)
#define SFAIL_(s, n)        BASE_LUA_STACK_FAIL(s, n)
#define FILE_MT_            BASE_LUA_FILE_MT
#define FILE_NEW_(s)        BASE_LUA_FILE_NEW(s)
#define FILE_CHECK_(s, idx) BASE_LUA_FILE_CHECK(s, idx)
#define FILE_TEST_(s, idx)  BASE_LUA_FILE_TEST(s, idx)
#define MMAP_MT_            BASE_LUA_MMAP_MT
#define MMAP_NEW_(s)        BASE_LUA_MMAP_NEW(s)
#define MMAP_CHECK_(s, idx) BASE_LUA_MMAP_CHECK(s, idx)
#define MMAP_TEST_(s, idx)  BASE_LUA_MMAP_TEST(s, idx)
typedef Base_Lua_File LFile_t;
typedef Base_Lua_MMap LMMap_t;

static int new_mmap (lua_State* s) {
	SFAIL_(s, 2);
	LFile_t* lf = FILE_TEST_(s, 1); /* Initial file is optional. */
	if (lf && !lf->valid)
		return luaL_error(s, "Tried to create new MMap with invalid file.");
	const bool ronly = lua_isboolean(s, 2) ? lua_toboolean(s, 2) : true; /* Default readonly. */
	LMMap_t* lmap = MMAP_NEW_(s);
	lmap->m = (const Base_MMap){0};
	lmap->valid = 1;
	Base_MMap* m = &lmap->m;
	if (lf) {
	/*
	 * We copy the contents of the Base_File, but we do not take ownership over it.
	 * It is the LFile_t's responsibility to release its own resources when it's time.
	 */
		m->file = lf->f; 
		if (Base_get_file_size(m->file, &m->size) ||
		    Base_MMap_map(m, ronly))
		{
			lua_pushnil(s);
			return 1;
		}
	} else {
		m->file = BASE_NULL_FILE;
#ifdef BASE_OS_WINDOWS
		m->win_fmapping = BASE_NULL_FILE;
#endif
	}
	return 1;
}

static int is_mapped (lua_State* s) {
	SFAIL_(s, 1);
	LMMap_t* lmap = MMAP_CHECK_(s, 1);
	lua_pushboolean(s, lmap->valid && lmap->m.ptr);
	return 1;
}

static int del_mmap (lua_State* s) {
	LMMap_t* lmap = MMAP_CHECK_(s, 1);
	if (lmap->valid && lmap->m.ptr) {
		lmap->valid = 0;
		if (!Base_MMap_unmap(&lmap->m))
			return luaL_error(s, "Base_MMap_unmap failed.");
	}
	return 0;
}

static int call_mmap (lua_State* s) {
	
}

static const luaL_Reg mmap_procs[] = {
	{"new", &new_mmap},
	{"is_mapped", &is_mapped},
	{"del", &del_mmap},
	{"map", &call_mmap},
	{NULL, NULL}
}

int luaopen_Base_MMap (lua_State* s) {
	SFAIL_(s, 2);
	luaL_newmetatable(s, MMAP_MT_);
	lua_pushcfunction(s, &del_mmap);
	lua_setfield(s, -2, "__gc");
	luaL_newlib(s, mmap_procs);
	return 1;
}
