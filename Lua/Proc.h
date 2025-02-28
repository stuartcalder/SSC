/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#if !defined(SSC_LUA_PROC_H) && defined(SSC_EXTERN_LUA)
#define SSC_LUA_PROC_H

#include "Macro.h"
#include "../File.h"
#include "../Operation.h"

/* SSC_Lua_File convenience macros. */
#define SSC_LUA_FILE_MT            "SSC_File"
#define SSC_LUA_FILE_KEY           SSC_LUA_FILE_MT /* FIXME: Deprecated. */
#define SSC_LUA_FILE_NEW(L)        SSC_LUA_NEW_UD(L, SSC_Lua_File)
#define SSC_LUA_FILE_CHECK(L, idx) SSC_LUA_CHECK_UD(L, idx, SSC_Lua_File, SSC_LUA_FILE_MT)
#define SSC_LUA_FILE_TEST(L, idx)  SSC_LUA_TEST_UD(L, idx, SSC_Lua_File, SSC_LUA_FILE_MT)

typedef struct {
  SSC_File_t file;
  size_t     file_n;
  char*      fpath;
  size_t     fpath_n;
  uint8_t    readonly;
} SSC_Lua_File;
#define SSC_LUA_FILE_NULL_LITERAL SSC_COMPOUND_LITERAL(SSC_Lua_File, SSC_FILE_NULL_LITERAL, 0, SSC_NULL, 0, 0)

SSC_BEGIN_C_DECLS

SSC_API int
luaopen_SSC_Proc(lua_State* L);

SSC_END_C_DECLS

#endif
