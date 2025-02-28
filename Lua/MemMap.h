/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#if !defined(SSC_LUA_MEMMAP_H) && defined(SSC_EXTERN_LUA)
#define SSC_LUA_MEMMAP_H

#include "Macro.h"
#include "../MemMap.h"
#include "../Operation.h"

/* SSC_Lua_MemMap convenience macros. */
#define SSC_LUA_MEMMAP_MT            "SSC_MemMap"
#define SSC_LUA_MEMMAP_KEY           SSC_LUA_MEMMAP_MT /* FIXME: Deprecated. */
#define SSC_LUA_MEMMAP_NEW(L)        SSC_LUA_NEW_UD(L, SSC_MemMap)
#define SSC_LUA_MEMMAP_CHECK(L, idx) SSC_LUA_CHECK_UD(L, idx, SSC_MemMap, SSC_LUA_MEMMAP_MT)
#define SSC_LUA_MEMMAP_TEST(L, idx)  SSC_LUA_TEST_UD(L, idx, SSC_MemMap, SSC_LUA_MEMMAP_MT)

/* FIXME: We no longer really need `SSC_Lua_MemMap`.
 * typedef this for now until we remove every usage of `SSC_Lua_MemMap`. */
typedef SSC_MemMap SSC_Lua_MemMap;
#define SSC_LUA_MEMMAP_NULL_LITERAL SSC_MEMMAP_NULL_LITERAL

SSC_BEGIN_C_DECLS

SSC_API int
luaopen_SSC_MemMap(lua_State* L);

SSC_END_C_DECLS

#endif /* ~ !defined(SSC_LUA_MEMMAP_H) ... */
