#include "lua.h"
#include "lua/mmap.h"
#include "lua/procs.h"

#define MFAIL_(s)           BASE_LUA_MALLOC_FAIL(s)
#define FILE_KEY_			BASE_LUA_FILE_KEY
#define FILE_NEW_(s)        BASE_LUA_FILE_NEW(s)
#define FILE_CHECK_(s, idx) BASE_LUA_FILE_CHECK(s, idx)
#define FILE_TEST_(s, idx)  BASE_LUA_FILE_TEST(s, idx)
#define MMAP_KEY_			BASE_LUA_MMAP_KEY
#define MMAP_NEW_(s)        BASE_LUA_MMAP_NEW(s)
#define MMAP_CHECK_(s, idx) BASE_LUA_MMAP_CHECK(s, idx)
#define MMAP_TEST_(s, idx)  BASE_LUA_MMAP_TEST(s, idx)
typedef Base_Lua_File LFile_t;
typedef Base_Lua_MMap LMMap_t;

static int new_mmap (lua_State* s) {
	LFile_t* lf = BASE_LUA_FILE_TEST(s, 1); /* Initial file is optional. */
	if (lf && !lf->valid)
		return luaL_error(s, "Tried to create new MMap with invalid file.");
	const bool ronly = lua_isboolean(s, 2) ? lua_toboolean(s, 2) : true; /* Default readonly. */
	LMMap_t* lmap = MMAP_NEW_(s);
	lmap->m = (Base_MMap){0};
	lmap->valid = 1;
	Base_MMap* m = &lmap->m;
	if (lf) {
		m->file = lf->f;
		if (Base_get_file_size(m->file, &m->size) ||
		    Base_MMap_map(m, ronly))
		{
			lua_pushnil(s);
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
	LMMap_t* lmap = MMAP_CHECK_(s, 1); /* LMMap */
	lua_pushboolean(s, lmap->valid && lmap->m.ptr);
	return 1;
}

static int del_mmap (lua_State* s) {
	LMMap_t* lmap = MMAP_CHECK_(s, 1); /* LMMap */
	if (lmap->valid && lmap->m.ptr) {
		lmap->valid = 0;
		if (!Base_MMap_unmap(&lmap->m))
			return luaL_error(s, "Base_MMap_unmap failed.");
	}
	return 0;
}

static const luaL_Reg mmap_procs[] = {
	{"new", &new_mmap},
	{"is_mapped", &is_mapped},
	{"del", &del_mmap},
	{NULL, NULL}
}

int luaopen_Base_MMap (lua_State* s) {
	luaL_newlib(s, mmap_procs);       /* Lib */
	luaL_newmetatable(s, MMAP_MT_);   /* Lib, MT */
	lua_pushcfunction(s, &del_mmap);  /* Lib, MT, CFunction */
	lua_setfield(s, -2, "__gc");      /* Lib, MT */
	lua_pushnil(s);                   /* Lib, MT, Nil */
	lua_copy(s, -3, -1);              /* Lib, MT, Lib */
	lua_setfield(s, -2, "__index");   /* Lib, MT */
	lua_pop(s);                       /* Lib */
	return 1;
}
