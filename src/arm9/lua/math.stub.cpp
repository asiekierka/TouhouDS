#include "thlua.h"

static int l_rand(lua_State* L) {
	int numargs = lua_gettop(L);

	s32 min = 0;
	s32 max = INT_MAX;
	if (numargs >= 2) {
		min = lua_tonumber(L, 1);
		max = lua_tonumber(L, 2);
	} else if (numargs >= 1) {
		max = lua_tonumber(L, 1);
	}

	u64 rnd = rand();
	u64 result = rnd * (max-min);

	lua_pushnumber(L, min + (result >> 31L));
	return 1;
}

static const struct luaL_reg glua_math_f[] = {
	{"rand", &l_rand},

	{"abs", &glue_math_abs},
	{"min", &glue_math_min},
	{"max", &glue_math_max},
	{"ceil", &glue_math_ceil},
	{"floor", &glue_math_floor},
	{"round", &glue_math_round},
	{"signum", &glue_math_signum},
	{"sin", &glue_math_sin},
	{"asin", &glue_math_asin},
	{"cos", &glue_math_cos},
	{"acos", &glue_math_acos},
	{"tan", &glue_math_tan},
	{"atan2", &glue_math_atan2},
	{"angleBetween", &glue_math_angleBetween},
	{NULL, NULL}
};
static const struct luaL_reg glua_math_m[] = {
	{NULL, NULL}
};

int glua_register_math(lua_State* L) {
	luaL_openlib(L, "Math", glua_math_f, 0);
	return 1;
}

s32 math_abs(s32 v) {
	return (v >= 0 ? v : -v);
}

s32 math_min(s32 a, s32 b) {
	return (a <= b ? a : b);
}

s32 math_max(s32 a, s32 b) {
	return (a >= b ? a : b);
}

s32 math_ceil(s32 v) {
	if (v < 0) {
		return (v & ~4095);
	} else {
		return (v & 4095 ? (v & ~4095) + 4096 : v);
	}
}

s32 math_floor(s32 v) {
	if (v >= 0) {
		return (v & ~4095);
	} else {
		return (v & 4095 ? (v & ~4095) + 4096 : v);
	}
}

s32 math_round(s32 v) {
	int frac = (v & 4095);

	if (frac == 0) return v;
	else if (frac >= 2048) return v + (4096-frac);
	else return v - frac;
}

s32 math_signum(s32 v) {
	if (v > 0) return 1;
	if (v < 0) return -1;
	return 0;
}

s32 math_sin(s32 a) {
	return fastSin(a>>6); //(512<<12) -> (1<<15)
}

s32 math_asin(s32 a) {
	return asinLerp(a) << 6; //(1<<15) -> (512<<12)
}

s32 math_cos(s32 a) {
	return fastCos(a>>6); //(512<<12) -> (1<<15)
}

s32 math_acos(s32 a) {
	return acosLerp(a) << 6; //(1<<15) -> (512<<12)
}

s32 math_tan(s32 a) {
	return tanLerp(a>>6); //(512<<12) -> (1<<15)
}

s32 math_atan2(s32 dy, s32 dx) {
	return atan2(dy, dx) << 6; //(1<<15) -> (512<<12)
}

s32 math_angleBetween(s32 x0, s32 y0, s32 x1, s32 y1) {
	return atan2(y1-y0, x1-x0) << 6; //(1<<15) -> (512<<12)
}

