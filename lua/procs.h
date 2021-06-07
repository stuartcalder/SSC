#if !defined(BASE_LUA_PROCS_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_PROCS_H

#include "macros.h"
#include "lua/macros.h"

#include <stdbool.h>
#include "errors.h"
#include "files.h"
#include "operations.h"

#define BASE_LUA_PROCS_KEY   "Procs"
#define BASE_LUA_PROCS_MT    "Base." BASE_LUA_PROCS_KEY
#define BASE_LUA_FILE_MT     "Base.File"
#define BASE_LUA_FILE_NEW(s)        (Base_Lua_File*)lua_newuserdatauv(s, sizeof(Base_Lua_File), 0)
#define BASE_LUA_FILE_CHECK(s, idx) (Base_Lua_File*)lua_checkudata(s, idx, BASE_LUA_FILE_MT)
#define BASE_LUA_FILE_TEST(s, idx)  (Base_Lua_File*)luaL_testudata(s, idx, BASE_LUA_FILE_MT)

typedef struct {
	Base_File_t f;
	uint8_t     valid;
} Base_Lua_File;

BASE_BEGIN_DECLS
BASE_API int luaopen_Base_Procs (lua_State* s);
BASE_END_DECLS

#endif
