/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#if !defined(BASE_LUA_SBUFFER_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_SBUFFER_H

#include <Base/errors.h>
#include <Base/macros.h>
#include <Base/mlock.h>
#include <Base/operations.h>

#include <Base/lua/macros.h>

/* Base_Lua_SBuffer convenience macros. */
#define BASE_LUA_SBUFFER_MT		"Base_SBuffer"
#define BASE_LUA_SBUFFER_KEY		BASE_LUA_SBUFFER_MT /* FIXME: Deprecated. */
#define BASE_LUA_SBUFFER_NEW(L)		BASE_LUA_NEW_UD(L, Base_Lua_SBuffer)
#define BASE_LUA_SBUFFER_CHECK(L, idx)	BASE_LUA_CHECK_UD(L, idx, Base_Lua_SBuffer, BASE_LUA_SBUFFER_MT)
#define BASE_LUA_SBUFFER_TEST(L, idx)	BASE_LUA_TEST_UD(L, idx, Base_Lua_SBuffer, BASE_LUA_SBUFFER_MT)

#define BASE_LUA_SBUFFER_MEM_IS_ALIGNED UINT8_C(0x01)
#define BASE_LUA_SBUFFER_MEM_IS_LOCKED	UINT8_C(0x02)

typedef struct {
	uint8_t* p; /* Data. */
	size_t   n; /* Number of bytes of data. */
#ifdef BASE_MLOCK_H
	uint8_t  f; /* Flags. */
#endif
} Base_Lua_SBuffer;
#define BASE_LUA_SBUFFER_NULL_LITERAL BASE_COMPOUND_LITERAL(Base_Lua_SBuffer, 0)

BASE_BEGIN_C_DECLS
BASE_API int luaopen_Base_SBuffer (lua_State* L);
BASE_END_C_DECLS

#endif
