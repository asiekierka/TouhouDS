#include "thlua.h"
#include "../game.h"
#include "../item.h"
#include "../sprite.h"
#include "../player.h"

int sprite_meta_index(lua_State* L);

static const struct luaL_reg glua_sprite_f[] = {
	{"new", &glue_sprite_new},

	{NULL, NULL}
};
const struct luaL_reg glua_sprite_m[] = {
	{"__gc", &glue_sprite_gc},

	{"setPos", &glue_sprite_setPos},
	{"setSpeed", &glue_sprite_setSpeed},
	{"setSpeedInc", &glue_sprite_setSpeedInc},
	{"setAngle", &glue_sprite_setAngle},
	{"setAngleInc", &glue_sprite_setAngleInc},
	{"setPower", &glue_sprite_setPower},
	{"setColCircle", &glue_sprite_setColCircle},
	{"setColSegment", &glue_sprite_setColSegment},
	{"removeColNode", &glue_sprite_removeColNode},
	{"setDrawSize", &glue_sprite_setDrawSize},
	{"setDrawAngle", &glue_sprite_setDrawAngle},
	{"setVisible", &glue_sprite_setVisible},
	{"setImage", &glue_sprite_setImage},
	{"setInvincibleTime", &glue_sprite_setInvincibleTime},
	{"setZ", &glue_sprite_setZ},
	{"setDieOutsideBounds", &glue_sprite_setDieOutsideBounds},
	{"getX", &glue_sprite_getX},
	{"getY", &glue_sprite_getY},
	{"getSpeed", &glue_sprite_getSpeed},
	{"getSpeedInc", &glue_sprite_getSpeedInc},
	{"getAngle", &glue_sprite_getAngle},
	{"getAngleInc", &glue_sprite_getAngleInc},
	{"getPower", &glue_sprite_getPower},
	{"getDrawWidth", &glue_sprite_getDrawWidth},
	{"getDrawHeight", &glue_sprite_getDrawHeight},
	{"getDrawAngle", &glue_sprite_getDrawAngle},
	{"getDrawAngleMode", &glue_sprite_getDrawAngleMode},
	{"isVisible", &glue_sprite_isVisible},
	{"isDestroyed", &glue_sprite_isDestroyed},
	{"getInvincibleTime", &glue_sprite_getInvincibleTime},
	{"getZ", &glue_sprite_getZ},
	{"getDieOutsideBounds", &glue_sprite_getDieOutsideBounds},
	{"advance", &glue_sprite_advance},
	{"advanceSide", &glue_sprite_advanceSide},
	{"die", &glue_sprite_die},
	{"drop", &glue_sprite_drop},
	{"slowMove", &glue_sprite_slowMove},
	{"slowAccel", &glue_sprite_slowAccel},
	{"slowTurn", &glue_sprite_slowTurn},
	{"slowTurnTo", &glue_sprite_slowTurnTo},
	{NULL, NULL}
};

int glua_register_sprite(lua_State* L) {
	luaL_newmetatable(L, "SpriteMeta");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	luaL_openlib(L, NULL, glua_sprite_m, 0);

	luaL_openlib(L, "SpriteMeta", glua_sprite_m, 0);
	luaL_openlib(L, "Sprite", glua_sprite_f, 0);
	return 3;
}

int glue_sprite_new(lua_State* L) {
	int numargs = lua_gettop(L);
	int objectRef = LUA_REFNIL;

	if (numargs == 0 || !lua_istable(L, 1)) {
		lua_newtable(L);
	} else {
		lua_pushvalue(L, 1);
	}

	int type = SPRITE_enemy;
	if (numargs >= 2) {
		type = lua_tointeger(L, 2);
	}

	int x = inttof32(128);
	int y = inttof32(128);
	if (numargs >= 4) {
		x = lua_tonumber(L, 3);
		y = lua_tonumber(L, 4);
	}

	//Top of the stack contains a table

	lua_pushvalue(L, -1);
	lua_pushcclosure(L, &sprite_meta_index, 1);
	lua_setfield(L, -2, "__index");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__newindex");

	lua_pushcfunction(L, &glue_sprite_gc);
	lua_setfield(L, -2, "__gc");

	//Sprite** obj = (Sprite**)lua_newuserdata(L, sizeof(Sprite*));
	//*obj = new Sprite(cur_game, type, cur_game->textures[TEX_enemy],
	//		x, y, 32, 32, UV(0, 0, 32, 32));
	void* usrmem = lua_newuserdata(L, sizeof(Sprite));
	Sprite* obj = new(usrmem) Sprite(cur_game, (SpriteType)type,
			cur_game->textures[TEX_enemy],
			x, y, 32, 32, UV(0, 0, 32, 32));
	cur_game->AddSprite(obj);

	lua_pushvalue(L, -2);
	lua_setmetatable(L, -2);

	lua_pushvalue(L, -1);
	objectRef = luaL_ref(L, LUA_OBJREF_TABLE);
	obj->refcount++;

	obj->luaLink = new LuaObjectLink(L, objectRef);

	LuaMethodLink* animLink = new LuaMethodLink(L, objectRef, "animator");
	animLink->looping = true;
	obj->luaAnimatorLink = animLink;

	return 1;
}

int glue_sprite_gc(lua_State* L) {
	return glue_generic_sprite_gc<Sprite>(L);
}

int sprite_meta_index(lua_State* L) {
	return luaMetaIndex(L, glua_sprite_m);
}

void sprite_setPos(Sprite* obj, s32 x, s32 y) {
	obj->x = x;
	obj->y = y;
}

void sprite_setSpeed(Sprite* obj, s32 spd) {
	obj->SetSpeed(spd);
}

void sprite_setSpeedInc(Sprite* obj, s32 spdi) {
	obj->SetSpeedInc(spdi);
}

void sprite_setAngle(Sprite* obj, s32 a) {
	obj->SetAngle((a>>6) & (DEGREES_IN_CIRCLE-1));
}

void sprite_setAngleInc(Sprite* obj, s32 ai) {
	obj->SetAngleInc((ai>>6) & (DEGREES_IN_CIRCLE-1));
}

void sprite_setPower(Sprite* obj, s32 pwr) {
	obj->power = pwr;
}

void sprite_setColCircle(Sprite* obj, int id, s32 rad) {
	obj->SetColCircle(id, rad);
}

void sprite_setColSegment(Sprite* obj, int id, bool autoRotate, s32 dx, s32 dy, s32 thickness) {
	obj->SetColSegment(id, autoRotate, dx, dy, thickness);
}

void sprite_removeColNode(Sprite* obj, int id) {
	obj->RemoveColNode(id);
}

void sprite_setDrawSize(Sprite* obj, s32 w, s32 h) {
	obj->drawData.vw = w >> 6;
	obj->drawData.vh = h >> 6;
	obj->drawData.OnSizeChanged();
}

void sprite_setDrawAngle(Sprite* obj, int drawAngleMode, s32 angle) {
	obj->SetDrawAngleMode((DrawAngleMode)drawAngleMode);
	if (drawAngleMode == DAM_rotate) {
		drawAngleMode = DAM_manual;
		angle = obj->GetDrawAngle() + angle;
	}
	if (drawAngleMode == DAM_manual) {
		obj->SetDrawAngle(angle);
	}
}

void sprite_setVisible(Sprite* obj, bool v) {
	if (obj) {
		obj->drawData.visible = v;
	}
}

void sprite_setImage(Sprite* obj, int texType, int x, int y, int w, int h) {
	if (texType < 0 || texType > NUM_TEXTURE_TYPES) {
		log(EL_warning, "LUA", "Invalid texture type: %d", texType);
		return;
	}

	Texture* texture = cur_game->textures[texType];
	obj->drawData.visible = (texture != NULL);
	obj->drawData.texture = texture;
	obj->drawData.uv = UV(x, y, w, h);
	obj->drawData.vw = VERTEX_SCALE(w);
	obj->drawData.vh = VERTEX_SCALE(h);
	obj->drawData.OnSizeChanged();
}

void sprite_setInvincibleTime(Sprite* obj, int time) {
	obj->invincible = MIN(0xFFFF, MAX(0, time));
}

void sprite_setZ(Sprite* obj, int z) {
	obj->z = z;
}

void sprite_setDieOutsideBounds(Sprite* obj, bool d) {
	obj->dieOutsideBounds = d;
}

s32 sprite_getX(Sprite* obj) {
	return obj->x;
}

s32 sprite_getY(Sprite* obj) {
	return obj->y;
}

s32 sprite_getSpeed(Sprite* obj) {
	return obj->GetSpeed();
}

s32 sprite_getSpeedInc(Sprite* obj) {
	return obj->GetSpeedInc();
}

s32 sprite_getAngle(Sprite* obj) {
	return obj->GetAngle() << 6;
}

s32 sprite_getAngleInc(Sprite* obj) {
	return obj->GetAngleInc() << 6;
}

s32 sprite_getPower(Sprite* obj) {
	return obj->power;
}

s32 sprite_getDrawWidth(Sprite* obj) {
	return obj->drawData.vw << 6;
}

s32 sprite_getDrawHeight(Sprite* obj) {
	return obj->drawData.vh << 6;
}

s32 sprite_getDrawAngle(Sprite* obj) {
	return obj->GetDrawAngle();
}

int sprite_getDrawAngleMode(Sprite* obj) {
	return obj->GetDrawAngleMode();
}

bool sprite_isVisible(Sprite* obj) {
	return obj->drawData.visible;
}

bool sprite_isDestroyed(Sprite* obj) {
	return obj->listIndex < 0;
}

int sprite_getInvincibleTime(Sprite* obj) {
	return obj->invincible;
}

int sprite_getZ(Sprite* obj) {
	return obj->z;
}

bool sprite_getDieOutsideBounds(Sprite* obj) {
	return obj->dieOutsideBounds;
}

void sprite_advance(Sprite* obj, s32 dist) {
	obj->Advance(dist);
}

void sprite_advanceSide(Sprite* obj, s32 dist) {
	obj->AdvanceSide(dist);
}

void sprite_die(Sprite* obj) {
	if (obj->listIndex >= 0) {
		obj->Destroy();
	}
}

void sprite_drop(Sprite* obj, int itemType, int amount) {
	//printf("%d %d\n", itemType, amount);

	ItemType type = (ItemType)itemType;

	int chance = 100;

    bool fp = cur_game->players[0]->HasFullPower();
    for (int n = 0; n < amount; n++) {
        if (rand() % 100 < chance) {
            if (fp) {
                if (type == IT_power) {
                	type = IT_extra;
                } else if (type == IT_powerLarge) {
                	type = IT_extraLarge;
                }
            }

            Item* item = new Item(cur_game, type);
            if (obj) {
            	item->x = obj->x;
            	item->y = obj->y;
            } else {
            	item->x = inttof32(SCREEN_WIDTH>>1);
                item->y = 0;
            }
            cur_game->AddSprite(item);
        }
    }

}

void sprite_slowMove(Sprite* obj, s32 targetX, s32 targetY, s32 speed) {
	//TODO: Implement
}

void sprite_slowAccel(Sprite* obj, s32 targetSpeed, s32 accel) {
	//TODO: Implement
}

void sprite_slowTurn(Sprite* obj, s32 targetAngle, s32 turnSpeed) {
	//TODO: Implement
}

void sprite_slowTurnTo(Sprite* obj, s32 targetX, s32 targetY, s32 turnSpeed) {
	//TODO: Implement
}
