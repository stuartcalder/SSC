/* Copyright 2021 Stuart Calder */
#include <stdbool.h>
#include <Base/lua/lua.h>
#include <Base/lua/sbuffer.h>

typedef Base_Lua_SBuffer SBuf_t;
#define MT_			BASE_LUA_SBUFFER_MT
#define NEW_(L)			BASE_LUA_SBUFFER_NEW(L)
#define CHECK_(L, idx)		BASE_LUA_SBUFFER_CHECK(L, idx)
#define TEST_(L, idx)		BASE_LUA_SBUFFER_TEST(L, idx)
#define MALLOC_FAIL_(L)		BASE_LUA_MALLOC_FAIL(L)

static int new_sbuffer (lua_State* L) {
	/* @n:		The size of the new SBuffer.	*
	 * @sb:		Newly created SBuffer.			*
	 * @lock:   Whether to mlock the buffer.    */
	const size_t n = (size_t)luaL_checkinteger(L, 1);
	SBuf_t* sb = NEW_(L);
	sb->n = n;
#ifdef BASE_MLOCK_H
	const uint8_t lock = lua_isboolean(L, 2) ? ((uint8_t)lua_toboolean(L, 2)) : UINT8_C(0); /* Default don't mlock. */
	if (lock) {
		if (!(sb->p = (uint8_t*)Base_aligned_malloc(Base_MLock_g.page_size, sb->n)))
			return luaL_error(L, "%s failed!", "Base_aligned_malloc");
		switch (Base_mlock(sb->p, sb->n)) {
		case 0:
			sb->lck = UINT8_C(1);
			break;
		case BASE_MLOCK_ERR_OVER_MEMLIMIT:
			/* Fail silently when we cannot lock anymore. */
			break;
		case BASE_MLOCK_ERR_LOCK_OP:
			return luaL_error(L, "%s failed with error `%s'.", "Base_MLock", "MLock Op");
#	ifdef BASE_EXTERN_MLOCK_THREADSAFE
		case BASE_MLOCK_ERR_MTX_OP:
			return luaL_error(L, "%s failed with error `%s'.", "Base_MLock", "Mutex Op");
#	endif
		}
	} else {
		if (!(sb->p = (uint8_t*)malloc(sb->n)))
			return MALLOC_FAIL_(L);
	}
#else /* No memorylocking. */
	if (!(sb->p = (uint8_t*)malloc(sb->n)))
		return MALLOC_FAIL_(L);
#endif
	luaL_getmetatable(L, MT_);
	lua_setmetatable(L, -2);
	return 1;
}

static int del_sbuffer (lua_State* L) {
	SBuf_t* sb = CHECK_(L, 1);
	if (sb->p) {
		Base_secure_zero(sb->p, sb->n);
#ifdef BASE_MLOCK_H
		if (sb->lck) {
			if (Base_munlock(sb->p, sb->n))
				return luaL_error(L, "Base_munlock failed!");
			Base_aligned_free(sb->p);
		} else
			free(sb->p);
#else /* No memorylocking. */
		free(sb->p);
#endif
		*sb = BASE_LUA_SBUFFER_NULL_LITERAL;
	}
	return 0;
}

static int get_sbuffer (lua_State* L) {
	SBuf_t* sb = CHECK_(L, 1);
	if (sb->p) {
		lua_pushlightuserdata(L, sb->p);
		lua_pushinteger(L, (lua_Integer)sb->n);
	} else {
		lua_pushnil(L);
		lua_pushnil(L);
	}
	return 2;
}

static int has_mlock (lua_State* L) {
#ifdef BASE_MLOCK_H
	lua_pushboolean(L, true);
#else /* No memorylocking. */
	lua_pushboolean(L, false);
#endif
	return 1;
}

static int is_mlocked (lua_State* L) {
#ifdef BASE_MLOCK_H
	SBuf_t* sb = CHECK_(L, 1);
	lua_pushboolean(L, sb->p && sb->lck);
#else /* No memorylocking. */
	lua_pushboolean(L, false);
#endif
	return 1;
}

static const luaL_Reg sbuffer_methods[] = {
	{"del"       , &del_sbuffer},
	{"get"       , &get_sbuffer},
	{"has_mlock" , &has_mlock},
	{"is_mlocked", &is_mlocked},
	{"__gc"      , &del_sbuffer},
	{"__close"   , &del_sbuffer},
	{NULL        , NULL}
};

static const luaL_Reg free_procs[] = {
	{"new"      , &new_sbuffer},
	{"has_mlock", &has_mlock},
	{NULL       , NULL}
};

int luaopen_Base_SBuffer (lua_State *L) {
#ifdef BASE_MLOCK_H
	switch (Base_MLock_g_init()) {
	case 0:
		break;
	case BASE_MLOCK_ERR_GET_LIMIT:
		return luaL_error(L, "Base_MLock_g_init failed! Error: %s", "GET_LIMIT");
	case BASE_MLOCK_ERR_SET_LIMIT:
		return luaL_error(L, "Base_MLock_g_init failed! Error: %s", "SET_LIMIT");
#	ifdef BASE_EXTERN_MLOCK_THREADSAFE
	case BASE_MLOCK_ERR_MTX_INIT:
		return luaL_error(L, "Base_MLock_g_init failed! Error: %s", "MTX_INIT");
#	endif
	}
#endif
	if (luaL_newmetatable(L, MT_)) {
		luaL_setfuncs(L, sbuffer_methods, 0);
		BASE_LUA_MT_SELF_INDEX(L);
	}
	luaL_newlib(L, free_procs);
	return 1;
}
