/* Copyright (c) 2020-2023 Stuart Steven Calder
 * See accompanying LICENSE file for licensing information. */
#if !defined(SSC_LUA_SECUREBUFFER_H) && defined(SSC_EXTERN_LUA)
#define SSC_LUA_SECUREBUFFER_H

#include "../Macro.h"
#include "../Error.h"
#include "../MemLock.h"
#include "../Operation.h"

#include "Macro.h"

/* SSC_Lua_SecureBuffer convenience macros. */
#define SSC_LUA_SECUREBUFFER_MT             "SSC_SecureBuffer"
#define SSC_LUA_SECUREBUFFER_KEY            SSC_LUA_SECUREBUFFER_MT /* FIXME: Deprecated. */
#define SSC_LUA_SECUREBUFFER_NEW(L)         SSC_LUA_NEW_UD(L, SSC_Lua_SecureBuffer)
#define SSC_LUA_SECUREBUFFER_CHECK(L, idx)  SSC_LUA_CHECK_UD(L, idx, SSC_Lua_SecureBuffer, SSC_LUA_SECUREBUFFER_MT)
#define SSC_LUA_SECUREBUFFER_TEST(L, idx)   SSC_LUA_TEST_UD(L, idx, SSC_Lua_SecureBuffer, SSC_LUA_SECUREBUFFER_MT)

#define SSC_LUA_SECUREBUFFER_MEM_IS_ALIGNED UINT8_C(0x01)
#define SSC_LUA_SECUREBUFFER_MEM_IS_LOCKED  UINT8_C(0x02)

typedef struct {
  uint8_t* p; /* Data. */
  size_t   n; /* Number of bytes of data. */
  #ifdef SSC_MLOCK_H
  uint8_t  f; /* Flags. */
  #endif
} SSC_Lua_SecureBuffer;
#define SSC_LUA_SECUREBUFFER_NULL_LITERAL SSC_COMPOUND_LITERAL(SSC_Lua_SecureBuffer, 0)

SSC_BEGIN_C_DECLS

SSC_API int
luaopen_SSC_SecureBuffer(lua_State* L);

SSC_END_C_DECLS

#endif
