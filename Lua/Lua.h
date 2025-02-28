/* Copyright (C) 2020-2025 Stuart Calder
 * See accompanying LICENSE file for licensing information. */
#if !defined(SSC_LUA_H) && defined(SSC_EXTERN_LUA)
#define SSC_LUA_H

#include "Macro.h"

SSC_BEGIN_C_DECLS

SSC_API int
luaopen_SSC(lua_State* L);

SSC_END_C_DECLS

#endif /* ~if !defined(SSC_LUA_H) && !defined(SSC_EXTERN_NO_LUA) */
