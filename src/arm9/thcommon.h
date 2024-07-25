#ifndef COMMON_H
#define COMMON_H

#include "tcommon/common.h"
#include "thds.h"
#include "th_linked_list.h"
#include "thlog.h"
#include "trig.h"

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define FPS 30
#define LEVEL_WIDTH 256
#define LEVEL_HEIGHT (384 - 32)
#define DAMAGE_FLASH_DEFAULT 5
#define TH_BASE_POLY_FMT (POLY_CULL_NONE|POLY_FORMAT_LIGHT0)
#define TH_DEFAULT_POLY_FMT (TH_BASE_POLY_FMT|POLY_ALPHA(31))
#define MAX_PLAYERS 4

typedef enum {
	AT_none,
	AT_player,
	AT_boss,
	AT_enemy,
	AT_center,
	AT_top,
	AT_bottom,
	AT_left,
	AT_right
} AngleTarget;

typedef enum {
	IT_null,
	IT_point,
	IT_pointLarge,
	IT_power,
	IT_powerLarge,
	IT_extra,
	IT_extraLarge,
	IT_life,
	IT_bomb,
	IT_fullPower,
	IT_magnetPoint
} ItemType;

typedef enum {
	CAT_idle,
	CAT_left,
	CAT_right,
	CAT_tweenLeft,
	CAT_tweenRight
} CharaAnimationType;

typedef enum {
	DAM_auto,
	DAM_manual,
	DAM_rotate
} DrawAngleMode;

typedef enum {
	TEX_enemy = 0,
	TEX_bullet = 1,
	TEX_bullet2 = 2,
	TEX_item = 3,
	TEX_player = 4,
	TEX_playerFx = 5,
	TEX_explosion = 6,
	TEX_background = 7,
	TEX_boss = 8,
	NUM_TEXTURE_TYPES = 9
} TextureType;

typedef enum {
	SPRITE_default = 0,
	SPRITE_remote = 1,
	SPRITE_player = 2,
	SPRITE_playerShot = 3,
	SPRITE_item = 4,
	SPRITE_enemy = 5,
	SPRITE_enemyShot = 6,
	NUM_SPRITE_TYPES = 7
} SpriteType;

typedef enum {
	VK_left = 1,
	VK_right = 2,
	VK_up = 4,
	VK_down = 8,
	VK_fire = 16,
	VK_focus = 32,
	VK_bomb = 64
} Keys;

class UV {
	public:
		s16 w, h;
		u8 x, y;

		UV(u8 uvx, u8 uvy, s8 uvw, s8 uvh) : w(uvw), h(uvh), x(uvx), y(uvy) {
		}

};

#define MIN_SCRIPT_VERSION "1.0.1"
#define SCRIPT_VERSION "1.1.0"

inline bool isValidScriptVersion(const char* string) {
	return strcmp(MIN_SCRIPT_VERSION, string) <= 0 && strcmp(string, SCRIPT_VERSION) <= 0;
}

#define DEPRECATED(func) func __attribute__ ((deprecated))

#endif
