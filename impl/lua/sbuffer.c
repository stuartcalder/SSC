/* Copyright 2021 Stuart Calder */
#include <Base/lua/lua.h>
#include <Base/lua/sbuffer.h>

#define MT_				BASE_LUA_SBUFFER_MT
#define NEW_(L)				BASE_LUA_SBUFFER_NEW(L)
#define CHECK_(L, idx)			BASE_LUA_SBUFFER_CHECK(L, idx)
#define TEST_(L, idx)			BASE_LUA_SBUFFER_TEST(L, idx)
#define NULL_LITERAL_			BASE_LUA_SBUFFER_NULL_LITERAL

#define IS_LOCKED_			BASE_LUA_SBUFFER_MEM_IS_LOCKED
#define IS_ALIGNED_			BASE_LUA_SBUFFER_MEM_IS_ALIGNED
#define HAS_BITS_(byte, bits)		((byte) & (bits))
#define SET_BITS_(byte, bits)		do { (byte) |= (bits);    } while (0)
#define CLEAR_BITS_(byte, bits)		do { (byte) &= (~(bits)); } while (0)

#define MALLOC_FAIL_(L)			BASE_LUA_MALLOC_FAIL(L)

typedef Base_Lua_SBuffer		SB_t;

static int sbuffer_new (lua_State* L) {
	const size_t n = (size_t)luaL_checkinteger(L, 1);
	SB_t* sb = NEW_(L);
#ifdef BASE_MLOCK_H
	sb->n = n;
	sb->f = UINT8_C(0);
	if (lua_isboolean(L, 2) ? lua_toboolean(L, 2) : 0) {
		SET_BITS_(sb->f, IS_ALIGNED_);
		if (!(sb->p = (uint8_t*)Base_aligned_malloc(Base_MLock_g.page_size, sb->n)))
			return luaL_error(L, "%s failed!", "Base_aligned_malloc");
		switch (Base_mlock(sb->p, sb->n)) {
			case 0:
				SET_BITS_(sb->f, IS_LOCKED_);
				break;
			case BASE_MLOCK_ERR_OVER_MEMLIMIT:
				/* Fail to lock silently when we cannot lock anymore. */
				break;
			case BASE_MLOCK_ERR_LOCK_OP:
				return luaL_error(L, "%s failed with error `%s'.", "Base_MLock", "MLock Op");
#  ifdef BASE_EXTERN_MLOCK_THREADSAFE
			case BASE_MLOCK_ERR_MTX_OP:
				return luaL_error(L, "%s failed with error `%s'.", "Base_MLock", "Mutex Op");
#  endif
			default:
				return luaL_error(L, "%s failed with error `%s'.", "Base_MLock", "Invalid Base_mlock Retcode");
		}
	} else {
		if (!(sb->p = (uint8_t*)malloc(sb->n)))
			return MALLOC_FAIL_(L);
	}
#else /* Memory-Locking disabled. */
	sb->n = n;
	if (!(sb->p = (uint8_t*)malloc(sb->n)))
		return MALLOC_FAIL_(L);
#endif
	luaL_getmetatable(L, MT_);
	lua_setmetatable(L, -2);
	return 1;
}

static int sbuffer_del (lua_State* L) {
	SB_t* sb = CHECK_(L, 1);
	if (sb->p) {
		memset(sb->p, 0, sb->n);
#ifdef BASE_MLOCK_H
		if (HAS_BITS_(sb->f, IS_LOCKED_) && Base_munlock(sb->p, sb->n))
			return luaL_error(L, "Base_munlock failed!");
		if (HAS_BITS_(sb->f, IS_ALIGNED_))
			Base_aligned_free(sb->p);
		else
			free(sb->p);
#else
		free(sb->p);
#endif
		*sb = NULL_LITERAL_;
	}
	return 0;
}

static int sbuffer_ptr (lua_State* L) {
	SB_t* sb = CHECK_(L, 1);
	if (sb->p)
		lua_pushlightuserdata(L, sb->p);
	else
		lua_pushnil(L);
	return 1;
}

static int sbuffer_size (lua_State* L) {
	SB_t* sb = CHECK_(L, 1);
	if (sb->p)
		lua_pushinteger(L, (lua_Integer)sb->n);
	else
		lua_pushnil(L);
	return 1;
}

static int sbuffer_has_mlock (lua_State* L) {
#ifdef BASE_MLOCK_H
	lua_pushboolean(L, 1);
#else /* No memorylocking. */
	lua_pushboolean(L, 0);
#endif
	return 1;
}

#ifdef BASE_MLOCK_H
static int sbuffer_is_mlocked (lua_State* L) {
	SB_t* sb = CHECK_(L, 1);
	lua_pushboolean(L, (sb->p != NULL) && HAS_BITS_(sb->f, IS_LOCKED_));
	return 1;
}
static int sbuffer_is_aligned (lua_State* L) {
	SB_t* sb = CHECK_(L, 1);
	lua_pushboolean(L, (sb->p != NULL) && HAS_BITS_(sb->f, IS_ALIGNED_));
	return 1;
}
#else
static const lua_CFunction sbuffer_is_mlocked = sbuffer_has_mlock;
static const lua_CFunction sbuffer_is_aligned = sbuffer_has_mlock;
#endif

#ifdef BASE_EXTERN_DEBUG
static int sbuffer_debug (lua_State* L) {
	lua_createtable(L, 0, 1); /* 1: Table of tables to return. */
	{
		lua_createtable(L, 0, 3); /* 2: Table holding Base_MLock_g. */
		{
			lua_pushinteger(L, (lua_Integer)Base_MLock_g.page_size);
			lua_setfield(L, -2, "page_size");
		} {
			lua_pushinteger(L, (lua_Integer)Base_MLock_g.limit);
			lua_setfield(L, -2, "limit");
		} {
			lua_pushinteger(L, (lua_Integer)Base_MLock_g.n);
			lua_setfield(L, -2, "n");
		}
		lua_setfield(L, -2, "Base_MLock_g");
	}
	return 1;
}
#endif

static const luaL_Reg sbuffer_methods[] = {
	{"ptr"       , sbuffer_ptr},
	{"size"      , sbuffer_size},
	{"has_mlock" , sbuffer_has_mlock},
	{"is_mlocked", sbuffer_is_mlocked},
	{"is_aligned", sbuffer_is_aligned},
	{"del"       , sbuffer_del},
	{"__gc"      , sbuffer_del},
#if BASE_LUA >= BASE_LUA_5_4
	{"__close"   , sbuffer_del},
#endif
#ifdef BASE_EXTERN_DEBUG
	{"debug"     , sbuffer_debug},
#endif
	{NULL        , NULL}
};

static const luaL_Reg free_procs[] = {
	{"new"      , sbuffer_new},
	{"has_mlock", sbuffer_has_mlock},
	{NULL       , NULL}
};

int luaopen_Base_SBuffer (lua_State* L) {
#ifdef BASE_MLOCK_H
	switch (Base_MLock_g_init()) {
	case 0:
		break;
	case BASE_MLOCK_ERR_GET_LIMIT:
		return luaL_error(L, "%s failed! Error: %s", "Base_MLock_g_init", "GET_LIMIT");
	case BASE_MLOCK_ERR_SET_LIMIT:
		return luaL_error(L, "%s failed! Error: %s", "Base_MLock_g_init", "SET_LIMIT");
#  ifdef BASE_EXTERN_MLOCK_THREADSAFE
	case BASE_MLOCK_ERR_MTX_INIT:
		return luaL_error(L, "%s failed! Error: %s", "Base_MLock_g_init", "MTX_INIT");
#  endif
	default:
		return luaL_error(L, "%s failed! Error: %s", "Base_MLock_g_init", "RETCODE");
	}
#endif
	if (luaL_newmetatable(L, MT_)) {
		luaL_setfuncs(L, sbuffer_methods, 0);
		BASE_LUA_MT_SELF_INDEX(L);
	}
	luaL_newlib(L, free_procs);
	return 1;
}
