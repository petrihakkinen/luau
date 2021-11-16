#define lveclib_c
#define LUA_LIB

#include "lstate.h"

#include "lua.h"

#include "lualib.h"

#include <math.h>

static const float* checkvector(lua_State* L, int idx)
{
  const float* p = lua_tovector(L, idx);
  luaL_argexpected(L, p, idx, "vector");
  return p;
}

static int vec_float3(lua_State* L)
{
  float x = (float)luaL_optnumber(L, 1, 0.0);
  float y = (float)luaL_optnumber(L, 2, 0.0);
  float z = (float)luaL_optnumber(L, 3, 0.0);
  lua_pushvector(L, x, y, z);
  return 1;
}

static int vec_dot3(lua_State* L)
{
  const float* a = checkvector(L, 1);
  const float* b = checkvector(L, 2);
  lua_pushnumber(L, a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
  return 1;
}

static int vec_cross3(lua_State* L)
{
  const float* a = checkvector(L, 1);
  const float* b = checkvector(L, 2);
  lua_pushvector(L, a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
  return 1;
}

static int vec_normalize3(lua_State* L)
{
  const float* v = checkvector(L, 1);
  float s = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  lua_pushvector(L, v[0] * s, v[1] * s, v[2] * s);
  return 1;
}

static const luaL_Reg veclib[] =
{
  {"float3", vec_float3},
  {"dot3", vec_dot3},
  {"cross3", vec_cross3},
  {"normalize3", vec_normalize3},
  {NULL, NULL}
};

static int vec_index(lua_State* L)
{
  const float* v = checkvector(L, 1);
  const char* k = luaL_checkstring(L, 2);
  if (k[1] != '\0')
    luaL_error(L, "attempt to index vector with '%s'", k);
  unsigned int i = (k[0] | ' ') - 'x';
  if (i >= 3)
    luaL_error(L, "attempt to index vector with '%s'", k);
  lua_pushnumber(L, v[i]);
  return 1;
}

static int vec_tostring(lua_State* L)
{
  const float* v = checkvector(L, 1);
  lua_pushfstring(L, LUA_NUMBER_FMT ", " LUA_NUMBER_FMT ", " LUA_NUMBER_FMT, v[0], v[1], v[2]);
  return 1;
}

static int vec_add(lua_State* L)
{
  const float* a = checkvector(L, 1);
  const float* b = checkvector(L, 2);
  lua_pushvector(L, a[0] + b[0], a[1] + b[1], a[2] + b[2]);
  return 1;
}

static int vec_sub(lua_State* L)
{
  const float* a = checkvector(L, 1);
  const float* b = checkvector(L, 2);
  lua_pushvector(L, a[0] - b[0], a[1] - b[1], a[2] - b[2]);
  return 1;
}

static int vec_mul(lua_State* L)
{
  const float* a = lua_tovector(L, 1);
  const float* b = lua_tovector(L, 2);
  if (a && b)
  {
    lua_pushvector(L, a[0] * b[0], a[1] * b[1], a[2] * b[2]);
    return 1;
  }
  else if (a && lua_isnumber(L, 2))
  {
    float bs = (float)lua_tonumber(L, 2);
    lua_pushvector(L, a[0] * bs, a[1] * bs, a[2] * bs);
    return 1;
  }
  else if(b && lua_isnumber(L, 1))
  {
    float as = (float)lua_tonumber(L, 1);
    lua_pushvector(L, as * b[0], as * b[1], as * b[2]);
    return 1;
  }
  else
  {
    luaL_argerror(L, 2, "expected vector or number");
  }
}

static int vec_unm(lua_State* L)
{
  const float* v = checkvector(L, 1);
  lua_pushvector(L, -v[0], -v[1], -v[2]);
  return 1;
}

/*
** Open vector library
*/
LUALIB_API int luaopen_vec(lua_State* L)
{
  luaL_register(L, LUA_VECLIBNAME, veclib);

  lua_pushvector(L, 0.0f, 0.0f, 0.0f);
  lua_createtable(L, 0, 0);

  lua_pushstring(L, "__type");
  lua_pushstring(L, "vector");
  lua_settable(L, -3);

  lua_pushstring(L, "__index");
  lua_pushcfunction(L, vec_index);
  lua_settable(L, -3);

  lua_pushstring(L, "__tostring");
  lua_pushcfunction(L, vec_tostring);
  lua_settable(L, -3);

  lua_pushstring(L, "__add");
  lua_pushcfunction(L, vec_add);
  lua_settable(L, -3);

  lua_pushstring(L, "__sub");
  lua_pushcfunction(L, vec_sub);
  lua_settable(L, -3);

  lua_pushstring(L, "__mul");
  lua_pushcfunction(L, vec_mul);
  lua_settable(L, -3);

  lua_pushstring(L, "__unm");
  lua_pushcfunction(L, vec_unm);
  lua_settable(L, -3);

  lua_setmetatable(L, -2);
  lua_pop(L, 1);

  return 1;
}
