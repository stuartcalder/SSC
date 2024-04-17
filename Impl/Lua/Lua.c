/* Copyright (c) 2020-2024 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#include "../../Macro.h"
#include "../../Lua/Lua.h"
#include "../../Lua/Macro.h"
#include "../../Lua/Proc.h"     /* Submodule 1 */
#include "../../Lua/SecureBuffer.h"   /* Submodule 2 */
#include "../../Lua/MemMap.h"      /* Submodule 3 */
#define NUM_SUBMODULES_         3

/* We load in the other modules of the table we created on the stack. */
#define LOAD_SUBMODULE_(L, SubModule) SSC_LUA_LOAD_SUBMODULE(L, SSC, SubModule)

int luaopen_SSC (lua_State* L) {
  lua_createtable(L, 0, NUM_SUBMODULES_);
  /* Load the submodules. */
  LOAD_SUBMODULE_(L, Proc);
  LOAD_SUBMODULE_(L, SecureBuffer);
  LOAD_SUBMODULE_(L, MemMap);
  return 1;
}
