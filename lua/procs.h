#if !defined(BASE_LUA_PROCS_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_PROCS_H

#include <Base/lua/macros.h>
#include <Base/files.h>
#include <Base/operations.h>

/* Base_Lua_File convenience macros. */
#define BASE_LUA_FILE_MT		"Base_File"
#define BASE_LUA_FILE_KEY		BASE_LUA_FILE_MT /* FIXME: Deprecated. */
#define BASE_LUA_FILE_NEW(L)		BASE_LUA_NEW_UD(L, Base_Lua_File)
#define BASE_LUA_FILE_CHECK(L, idx)	BASE_LUA_CHECK_UD(L, idx, Base_Lua_File, BASE_LUA_FILE_MT)
#define BASE_LUA_FILE_TEST(L, idx)	BASE_LUA_TEST_UD(L, idx, Base_Lua_File, BASE_LUA_FILE_MT)

typedef struct {
	Base_File_t	file;
	size_t          file_n;
	char*		fpath;
	size_t		fpath_n;
	uint8_t		readonly;
} Base_Lua_File;

#define BASE_LUA_FILE_NULL_LITERAL (Base_Lua_File){BASE_NULL_FILE, 0, NULL, 0, 0}

BASE_BEGIN_C_DECLS
BASE_API int luaopen_Base_Procs (lua_State* L);
BASE_END_C_DECLS

#endif
