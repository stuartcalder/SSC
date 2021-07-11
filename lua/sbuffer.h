#if !defined(BASE_LUA_SBUFFER_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_SBUFFER_H

#include <stdbool.h>
#include <Base/errors.h>
#include <Base/macros.h>
#include <Base/mlock.h>
#include <Base/operations.h>
#include <Base/lua/macros.h>

/* Base_Lua_SBuffer convenience macros. */
#define BASE_LUA_SBUFFER_KEY			"Base.SBuffer" /*FIXME: Deprecated. */
#define BASE_LUA_SBUFFER_MT				"Base.SBuffer"
#define BASE_LUA_SBUFFER_NEW(s)			(Base_Lua_SBuffer*)lua_newuserdatauv(s, sizeof(Base_Lua_SBuffer), 0)
#define BASE_LUA_SBUFFER_CHECK(s, idx)	(Base_Lua_SBuffer*)luaL_checkudata(s, idx, BASE_LUA_SBUFFER_MT)
#define BASE_LUA_SBUFFER_TEST(s, idx)	(Base_Lua_SBuffer*)luaL_testudata(s, idx, BASE_LUA_SBUFFER_MT)
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

#define BASE_LUA_SBUFFER_NULL_LITERAL (Base_Lua_SBuffer){NULL, 0, 0}

BASE_BEGIN_DECLS
BASE_API int luaopen_Base_SBuffer (lua_State *s);
BASE_END_DECLS

#endif
