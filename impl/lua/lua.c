/* Copyright 2021 Stuart Calder */
#include <Base/macros.h>
#include <Base/lua/lua.h>
#include <Base/lua/macros.h>
#include <Base/lua/procs.h>   /* Module 1 */
#include <Base/lua/sbuffer.h> /* Module 2 */
#include <Base/lua/mmap.h>    /* Module 3 */
#define NUM_MODULES_ 3

/* We load in the other modules of the table we
 * created on the stack.
 */
#define LOAD_MODULE_(L, module) do { \
	lua_pushcfunction(L, &luaopen_Base_##module); \
	if (lua_pcall(L, 0, 1, 0) != LUA_OK) \
		return luaL_error(L, "Failed to load %s.", BASE_STRINGIFY(module)); \
	lua_setfield(L, -2, BASE_STRINGIFY(module)); \
} while (0)

int luaopen_Base (lua_State* L) {
	lua_createtable(L, 0, NUM_MODULES_);
/* Load Procs module. */
	LOAD_MODULE_(L, Procs);
/* Load SBuffer module. */
	LOAD_MODULE_(L, SBuffer);
/* Load MMap module. */
	LOAD_MODULE_(L, MMap);
	return 1;
}
