#include "lua.h"
#include "lua/procs.h"		/* Mod 1 */
#include "lua/sbuffer.h"	/* Mod 2 */
#include "lua/mmap.h"		/* Mod 3 */
#define NUM_MODULES_ 3

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
