#if !defined(BASE_LUA_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_H

#include <Base/macros.h>
#include <Base/lua/macros.h>

BASE_BEGIN_DECLS
BASE_API int luaopen_Base (lua_State* L);
BASE_END_DECLS

#endif /* ~if !defined(BASE_LUA_H) && !defined(BASE_EXTERN_NO_LUA) */
