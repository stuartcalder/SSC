#include <stdbool.h>
#include "lua/sbuffer.h"

typedef Base_Lua_SBuffer SBuf_t;
#define MT_            BASE_LUA_SBUFFER_MT
#define NEW_(s)        BASE_LUA_SBUFFER_NEW(s)
#define CHECK_(s, idx) BASE_LUA_SBUFFER_CHECK(s, idx)
#define TEST_(s, idx)  BASE_LUA_SBUFFER_TEST(s, idx)
#define MFAIL_(s)      BASE_LUA_MALLOC_FAIL(s)
#define SFAIL_(s, n)   BASE_LUA_STACK_FAIL(s, n)

#ifdef BASE_HAS_MEMORYLOCKING
static size_t os_page_size = 0;
#endif

static int new_sbuffer (lua_State* s) {
	const size_t n = (size_t)luaL_checkunsigned(s, 1);
#ifdef BASE_HAS_MEMORYLOCKING
	const bool lock = lua_isboolean(s, 2) ? lua_toboolean(s, 2) : false;
#endif
	lua_settop(s, 0);
	SBuf_t* sb = NEW_(s);
	sb->n = n;
	sb->f = BASE_LUA_SBUFFER_ISVALID;
#ifdef BASE_HAS_MEMORYLOCKING
	if (lock) {
		if (!(sb->p = (uint8_t*)Base_aligned_malloc(os_page_size, sb->n)))
			return luaL_error(s, "Base_aligned_malloc failed.");
		if (Base_mlock(sb->p, sb->n))
			return luaL_error(s, "Base_mlock failed.");
		sb->f |= BASE_LUA_SBUFFER_MLOCK;
	} else {
		if (!(sb->p = (uint8_t*)malloc(sb->n)))
			return MFAIL_(s);
	}
#else
	if (!(sb->p = (uint8_t*)malloc(sb->n)))
		return MFAIL_(s);
#endif
	luaL_getmetatable(s, MT_);
	lua_setmetatable(s, -2);
	return 1;
}

static int del_sbuffer (lua_State* s) {
	SBuf_t* sb = CHECK_(s, 1);
	if (sb->f & BASE_LUA_SBUFFER_ISVALID) {
		sb->f &= BASE_LUA_SBUFFER_INVALIDATE;
		Base_secure_zero(sb->p, sb->n);
#ifdef BASE_HAS_MEMORYLOCKING
		if (sb->f & BASE_LUA_SBUFFER_MLOCK) {
			sb->f &= BASE_LUA_SBUFFER_UNLOCK;
			if (Base_munlock(sb->p, sb->n))
				return luaL_error(s, "Base_munlock failed.");
			Base_aligned_free(sb->p);
		} else
			free(sb->p);
#else
		free(sb->p);
#endif
	}
	return 0;
}

static int get_sbuffer (lua_State* s) {
	SBuf_t* sb = CHECK_(s, 1);
	SFAIL_(s, 2);
	if (sb->f & BASE_LUA_SBUFFER_ISVALID) {
		lua_pushlightuserdata(s, sb->p);
		lua_pushunsigned(s, (lua_Unsigned)sb->n);
	} else {
		lua_pushnil(s);
		lua_pushnil(s);
	}
	return 2;
}

static int has_mlock (lua_State* s) {
	SFAIL_(s, 1);
#ifdef BASE_HAS_MEMORYLOCKING
	lua_pushboolean(s, true);
#else
	lua_pushboolean(s, false);
#endif
	return 1;
}

static const luaL_Reg sbuffer[] = {
	{"new"      , &new_sbuffer},
	{"del"      , &del_sbuffer},
	{"get"      , &get_sbuffer},
	{"has_mlock", &has_mlock},
	{NULL, NULL}
}

int luaopen_Base_SBuffer (lua_State *s) {
#ifdef BASE_HAS_MEMORYLOCKING
	if (!os_page_size)
		os_page_size = Base_get_pagesize();
#endif
	SFAIL_(s, 2);
	luaL_newmetatable(s, MT_);
	lua_pushcfunction(s, &del_sbuffer);
	lua_setfield(s, -2, "__gc");
	luaL_newlib(s, sbuffer);
	return 1;
}
