#include "lua.h"

#include "lua/sbuffer.h" /* Module 1 */
#include "lua/mmap.h"    /* Module 2 */
#define NUM_MODULES_ 2

#define LOAD_MODULE_(s, module) do { \
	lua_pushcfunction(s, &luaopen_Base_##module); \
	if (lua_pcall(s, 0, 1, 0) != LUA_OK) \
		return luaL_error(s, "Failed to load " BASE_STRINGIFY(module) "."); \
} while (0)

int luaopen_Base (lua_State *s) {
	lua_createtable(s, 0, NUM_MODULES_);
/* Load SBuffer module. */
	LOAD_MODULE_(s, SBuffer);
	lua_setfield(s, -2, BASE_LUA_SBUFFER_KEY);
/* Load MMap module. */
	LOAD_MODULE_(s, MMap);
	lua_setfield(s, -2, BASE_LUA_MMAP_KEY);
	return 1;
}
