#if !defined(BASE_LUA_SBUFFER_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_SBUFFER_H

#include <Base/errors.h>
#include <Base/macros.h>
#include <Base/mlock.h>
#include <Base/operations.h>
#include <Base/lua/macros.h>

/* Base_Lua_SBuffer convenience macros. */
#define BASE_LUA_SBUFFER_MT		"Base.SBuffer"
#define BASE_LUA_SBUFFER_KEY		BASE_LUA_SBUFFER_MT /* FIXME: Deprecated. */
#define BASE_LUA_SBUFFER_NEW(L)		BASE_LUA_NEW_UD(L, Base_Lua_SBuffer)
#define BASE_LUA_SBUFFER_CHECK(L, idx)	BASE_LUA_CHECK_UD(L, idx, Base_Lua_SBuffer, BASE_LUA_SBUFFER_MT)
#define BASE_LUA_SBUFFER_TEST(L, idx)	BASE_LUA_TEST_UD(L, idx, Base_Lua_SBuffer, BASE_LUA_SBUFFER_MT)
/* Flags */
#define BASE_LUA_SBUFFER_ISVALID    UINT8_C(0x01)
#define BASE_LUA_SBUFFER_MLOCK      UINT8_C(0x02)
/* Masks */
#define BASE_LUA_SBUFFER_INVALIDATE (~BASE_LUA_SBUFFER_ISVALID)
#define BASE_LUA_SBUFFER_UNLOCK     (~BASE_LUA_SBUFFER_MLOCK)

typedef struct {
	uint8_t* p; /* Data. */
	size_t   n; /* Number of bytes of data. */
#ifdef BASE_MLOCK_H
	uint8_t  lck; /* Boolean: Is the pointer memory locked? */
#endif
} Base_Lua_SBuffer;

#define BASE_LUA_SBUFFER_NULL_LITERAL (Base_Lua_SBuffer){0}

BASE_BEGIN_DECLS
BASE_API int luaopen_Base_SBuffer (lua_State* L);
BASE_END_DECLS

#endif
