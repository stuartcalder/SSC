/* Copyright (c) 2020-2022 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information.
 */
#include <Base/macros.h>
#include <Base/lua/lua.h>
#include <Base/lua/macros.h>
#include <Base/lua/procs.h>	/* Module 1 */
#include <Base/lua/sbuffer.h>	/* Module 2 */
#include <Base/lua/mmap.h>	/* Module 3 */
#define NUM_SUBMODULES_		3

/* We load in the other modules of the table we created on the stack. */
#define LOAD_SUBMODULE_(L, submodule) BASE_LUA_LOAD_SUBMODULE(L, Base, submodule)

int luaopen_Base (lua_State* L) {
	lua_createtable(L, 0, NUM_SUBMODULES_);
	/* Load the submodules. */
	LOAD_SUBMODULE_(L, Procs);
	LOAD_SUBMODULE_(L, SBuffer);
	LOAD_SUBMODULE_(L, MMap);
	return 1;
}
