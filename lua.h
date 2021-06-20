#if !defined(BASE_LUA_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_H

#include "macros.h"
#include "lua/macros.h"

BASE_BEGIN_DECLS
BASE_API int luaopen_Base (lua_State *s);
BASE_END_DECLS

#endif /* ~if !defined(BASE_LUA_H) && !defined(BASE_EXTERN_NO_LUA) */
