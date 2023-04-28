/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#if !defined(BASE_LUA_MMAP_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_MMAP_H

#include <Base/lua/macros.h>
#include <Base/mmap.h>
#include <Base/operations.h>

/* Base_Lua_MMap convenience macros. */
#define BASE_LUA_MMAP_MT		"Base_MMap"
#define BASE_LUA_MMAP_KEY		BASE_LUA_MMAP_MT /* FIXME: Deprecated. */
#define BASE_LUA_MMAP_NEW(L)		BASE_LUA_NEW_UD(L, Base_MMap)
#define BASE_LUA_MMAP_CHECK(L, idx)	BASE_LUA_CHECK_UD(L, idx, Base_MMap, BASE_LUA_MMAP_MT)
#define BASE_LUA_MMAP_TEST(L, idx)	BASE_LUA_TEST_UD(L, idx, Base_MMap, BASE_LUA_MMAP_MT)

/* FIXME: We no longer really need `Base_Lua_MMap`.
 * typedef this for now until we remove every usage of `Base_Lua_MMap`. */
typedef Base_MMap Base_Lua_MMap;
#define BASE_LUA_MMAP_NULL_LITERAL BASE_MMAP_NULL_LITERAL

BASE_BEGIN_C_DECLS

BASE_API int
luaopen_Base_MMap(lua_State* L);

BASE_END_C_DECLS

#endif /* ~ !defined(BASE_LUA_MMAP_H) ... */
