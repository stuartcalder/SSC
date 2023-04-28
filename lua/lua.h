/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#if !defined(BASE_LUA_H) && defined(BASE_EXTERN_LUA)
#define BASE_LUA_H

#include <Base/lua/macros.h>

BASE_BEGIN_C_DECLS

BASE_API int
luaopen_Base(lua_State* L);

BASE_END_C_DECLS

#endif /* ~if !defined(BASE_LUA_H) && !defined(BASE_EXTERN_NO_LUA) */
