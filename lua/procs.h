#if !defined(BASE_LUA_PROCS_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_PROCS_H

#include "macros.h"
#include "files.h"
#include "lua/macros.h"

#define BASE_LUA_FILE_KEY			"Base.File" /*TODO: Remove this macro; it's here for compatability. */
#define BASE_LUA_FILE_MT			"Base.File"
#define BASE_LUA_FILE_NEW(L)		(Base_Lua_File*)lua_newuserdatauv(L, sizeof(Base_Lua_File), 0)
#define BASE_LUA_FILE_CHECK(L, idx)	(Base_Lua_File*)luaL_checkudata(L, idx, BASE_LUA_FILE_MT)
#define BASE_LUA_FILE_TEST(L, idx)	(Base_Lua_File*)luaL_testudata(L, idx, BASE_LUA_FILE_MT)

typedef struct {
	Base_File_t	f;
	uint8_t		valid;
} Base_Lua_File;

BASE_BEGIN_DECLS
BASE_API int luaopen_Base_Procs (lua_State*);
BASE_END_DECLS

#endif
