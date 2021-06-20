#if !defined(BASE_LUA_MMAP_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_MMAP_H

#include "macros.h"
#include "lua/macros.h"

#include "mmap.h"
#include "operations.h"

/* Base_Lua_MMap convenience macros. */
#define BASE_LUA_MMAP_KEY			"Base.MMap"
#define BASE_LUA_MMAP_NEW(s)        (Base_Lua_MMap*)lua_newuserdatauv(s, sizeof(Base_Lua_MMap), 0)
#define BASE_LUA_MMAP_CHECK(s, idx) (Base_Lua_MMap*)luaL_checkudata(s, idx, BASE_LUA_MMAP_KEY)
#define BASE_LUA_MMAP_TEST(s, idx)  (Base_Lua_MMap*)luaL_testudata(s, idx, BASE_LUA_MMAP_KEY)

typedef struct {
	Base_MMap m;
	uint8_t   valid;
} Base_Lua_MMap;

BASE_BEGIN_DECLS
BASE_API int luaopen_Base_MMap (lua_State* s);
BASE_END_DECLS

#endif
