#include "compiler.h"

#include <cstdarg>
#include "../boss.h"
#include "../game.h"
#include "../level.h"
#include "../thlog.h"
#include "../charinfo.h"
#include "../spells/common_spells.h"

SpellBookTemplate* compileDefine(Runtime* runtime, u16 fileId, ParseNode* node) {
    SpellBookTemplate* sbt = NULL;
    if (node->define.type == SPELL || node->define.type == ANIMATION
    		|| node->define.type == CONVERSATION)
    {
        sbt = new SpellBookTemplate(fileId, node->define.type);
    } else if (node->define.type == OBJECT || node->define.type == BOSS) {
        sbt = new ObjectTemplate(fileId, node->define.type);
    }
    if (!sbt) {
        compilerLog(EL_error, __FILENAME__, "Unknown type of define: %d", node->define.type);
        return NULL;
    }

    sbt->SetName(node->define.name);
    node = node->define.spellBook;

    u16 t = 0;
    while (node) {
        ParseNode* value = node->spellBook.value;

        if (value->type == PN_label) {
            if (sbt->type != OBJECT && sbt->type != BOSS) {
                compilerLog(EL_warning, __FILENAME__, "Labels should only be used in object defines");
                delete sbt;
                return NULL;
            }

            ObjectTemplate* ot = (ObjectTemplate*)sbt;
            if (strcmp(value->label.name, ":init:") == 0) {
                ot->initSpellsStart = t;
            } else if (strcmp(value->label.name, ":life:") == 0) {
                ot->lifeSpellsStart = t;
            } else if (strcmp(value->label.name, ":death:") == 0) {
                ot->deathSpellsStart = t;
            }
        } else {
        	SpellTemplate* spell = compileSpell(runtime, value);
        	if (!spell) {
        		delete sbt;
        		return NULL;
        	}
            sbt->AddSpell(spell);
            t++;
        }
        node = node->spellBook.next;
    }

    return sbt;
}

SpellTemplate* compileSpell(Runtime* runtime, ParseNode* node) {
	SpellDecl* decl = getSpellDecl(node->spell.name);
	if (!decl) {
		compilerLog(EL_error, __FILENAME__,
				"Unknown spell: %s\n", node->spell.name);
		return NULL;
	}

    SpellTemplate* st = new SpellTemplate();

    st->SetName(node->spell.name);
    st->decl = decl;

    node = node->spell.params;
    while (node && node->paramList.value) {
    	Param* p = new Param();
    	if (compileParam(p, runtime, node->paramList.value)) {
    		st->AddParam(p);
    	}
        unrefParam(p);
        node = node->paramList.next;
    }

    return st;
}

bool compileParam(Param* retval, Runtime* runtime, ParseNode* node) {
	retval->type = node->param.type;

    if (node->param.type == PT_int) {
    	retval->normal.numval = node->param.intval;
    } else if (node->param.type == PT_float) {
    	retval->normal.numval = floattof32(node->param.floatval);
    } else if (node->param.type == PT_string || node->param.type == PT_identifier) {
    	retval->SetString(node->param.strval);
    } else if (node->param.type == PT_variable) {
        int numval = -1;
    	sscanf(node->param.strval, "$%d", &numval);
    	retval->normal.numval = (numval >= 1 ? numval : -1);
    } else if (node->param.type == PT_random) {
		char* var0 = node->param.strval + 1;
		char* var1 = strchr(var0, ':');
		char* vstep = NULL;
		if (var1) {
			var1[0] = '\0';
			var1++;

			vstep = strchr(var1, ';');
			if (vstep) {
				vstep[0] = '\0';
				vstep++;
			}
		} else {
			compilerLog(EL_error, __FILENAME__, "Invalid random value string: %s", node->param.strval);
	        return false;
		}

        s32 v0 = floattof32(atof(var0));
        s32 v1 = floattof32(atof(var1));
		s32 step = floattof32(0.01f);
		if (vstep) {
			//Don't let step be too small
			step = MAX(step, floattof32(atof(vstep)));
		}

		retval->random.min = MIN(v0, v1);
		retval->random.max = MAX(v0, v1);
		retval->random.step = step;
    }
    return true;
}

bool checkSpellType(Runtime* runtime, SpellBookTemplate* sbt) {
    bool result = true;
    for (int n = 0; n < sbt->spellsL; n++) {
        SpellTemplate* s = sbt->spells[n];
        if (s && !s->decl->TypeCheck(runtime, sbt, s)) {
        	result = false;
        }
    }
    return result;
}

bool checkSpellContext(Runtime* runtime, SpellBookTemplate* sbt) {
	bool result = true;
    for (int n = 0; n < sbt->spellsL; n++) {
        SpellTemplate* s = sbt->spells[n];
        if (s) {
        	u32 context = 0;
        	switch (sbt->type) {
        	case SPELL: context = SPT_spell; break;
        	case OBJECT: context = SPT_object; break;
        	case BOSS: context = SPT_boss; break;
        	case ANIMATION: context = SPT_animation; break;
        	case CONVERSATION: context = SPT_conversation; break;
        	default:
    			compilerLog(EL_error, __FILENAME__,
    					"Unknown type of spellbook: %d\n", sbt->type);
    			break;
        	}

        	if (!s->decl->IsValidContext(context)) {
    			result = false;
    			char buffer[512];
    			s->ToString(buffer);

    			compilerLog(EL_error, __FILENAME__,
    					"Type error in define \"%s\":\n  here: %s\n"
    					"  Command (%s) is not allowed in spellbooks of this type.",
    					sbt->name, buffer, s->name);
        	}
        }
    }
    return result;
}

//==============================================================================
//==============================================================================
//==============================================================================

Param::Param()
: type(PT_int), refcount(1)
{
	normal.numval = 0;
	normal.strval = NULL;
	normal.cachedSpell = NULL;
}
Param::~Param() {
	if (type != PT_random) {
		if (normal.strval) {
			delete[] normal.strval;
		}
	}
}

ITCM_CODE
const Texture* Param::AsTexture(Game* game) {
    if (type == PT_identifier) {
        if (strcmp(normal.strval, "tex_enemy") == 0) {
            return game->textures[TEX_enemy];
        } else if (strcmp(normal.strval, "tex_bullet") == 0) {
            return game->textures[TEX_bullet];
        } else if (strcmp(normal.strval, "tex_bullet2") == 0) {
            return game->textures[TEX_bullet2];
        } else if (strcmp(normal.strval, "tex_item") == 0) {
            return game->textures[TEX_item];
        } else if (strcmp(normal.strval, "tex_player") == 0) {
            return game->textures[TEX_player];
        } else if (strcmp(normal.strval, "tex_player_fx") == 0) {
            return game->textures[TEX_playerFx];
        } else if (strcmp(normal.strval, "tex_background") == 0) {
            return game->textures[TEX_background];
        } else if (strcmp(normal.strval, "tex_boss") == 0) {
            return game->textures[TEX_boss];
        }
    }
    return NULL;
}

ItemType Param::AsItemType() {
    if (type == PT_identifier) {
        if (strcmp(normal.strval, "point") == 0) {
            return IT_point;
        } else if (strcmp(normal.strval, "point_large") == 0) {
            return IT_pointLarge;
        } else if (strcmp(normal.strval, "power") == 0) {
            return IT_power;
        } else if (strcmp(normal.strval, "power_large") == 0) {
            return IT_powerLarge;
        } else if (strcmp(normal.strval, "extra") == 0) {
            return IT_extra;
        } else if (strcmp(normal.strval, "extra_large") == 0) {
            return IT_extraLarge;
        } else if (strcmp(normal.strval, "life") == 0) {
            return IT_life;
        } else if (strcmp(normal.strval, "bomb") == 0) {
            return IT_bomb;
        } else if (strcmp(normal.strval, "full_power") == 0) {
            return IT_fullPower;
        } else if (strcmp(normal.strval, "magnet_point") == 0) {
            return IT_magnetPoint;
        }
    }
    return IT_point;
}

AngleTarget Param::AsAngleTarget() {
    if (type == PT_identifier) {
        if (strcmp(normal.strval, "player") == 0) {
            return AT_player;
        } else if (strcmp(normal.strval, "boss") == 0) {
            return AT_boss;
        } else if (strcmp(normal.strval, "enemy") == 0) {
            return AT_enemy;
        } else if (strcmp(normal.strval, "center") == 0) {
            return AT_center;
        } else if (strcmp(normal.strval, "top") == 0) {
            return AT_top;
        } else if (strcmp(normal.strval, "bottom") == 0) {
            return AT_bottom;
        } else if (strcmp(normal.strval, "left") == 0) {
            return AT_left;
        } else if (strcmp(normal.strval, "right") == 0) {
            return AT_right;
        }
    }
    return AT_none;
}

CharaAnimationType Param::AsCharaAnimationType() {
    if (type == PT_identifier) {
        if (strcmp(normal.strval, "idle") == 0) {
            return CAT_idle;
        } else if (strcmp(normal.strval, "left") == 0) {
            return CAT_left;
        } else if (strcmp(normal.strval, "right") == 0) {
            return CAT_right;
        } else if (strcmp(normal.strval, "tween_left") == 0) {
            return CAT_tweenLeft;
        } else if (strcmp(normal.strval, "tween_right") == 0) {
            return CAT_tweenRight;
        }
    }
    return CAT_idle;
}

DrawAngleMode Param::AsDrawAngleMode() {
    if (type == PT_identifier) {
        if (strcmp(normal.strval, "auto") == 0) {
            return DAM_auto;
        } else if (strcmp(normal.strval, "manual") == 0) {
            return DAM_manual;
        } else if (strcmp(normal.strval, "rotate") == 0) {
            return DAM_rotate;
        }
    }
    return DAM_auto;
}

void Param::ToString(char* out) {
    switch (type) {
		case PT_null: sprintf(out, "NULL"); break;
        case PT_int: sprintf(out, "%d", normal.numval); break;
        case PT_float: sprintf(out, "%.2f", f32tofloat(normal.numval)); break;
        case PT_string: sprintf(out, "\"%s\"", normal.strval); break;
        case PT_identifier: sprintf(out, "%s", normal.strval); break;
        case PT_random: sprintf(out, "~%.2f:%.2f;%.2f", f32tofloat(random.min), f32tofloat(random.max), f32tofloat(random.step)); break;
        case PT_variable: sprintf(out, "$%d", normal.numval); break;
    }
}

ITCM_CODE
int Param::AsInt() {
	switch (type) {
	case PT_int: return normal.numval;
	case PT_float: return f32toint(normal.numval);
	case PT_random: return f32toint(AsFixed());
	default: return 0;
	}
}

ITCM_CODE
int Param::AsFixed() {
	switch (type) {
	case PT_float: return normal.numval;
	case PT_int: return inttof32(normal.numval);
	case PT_random: return random.min + (rand() % f32toint(divf32(random.max-random.min, random.step))) * random.step;
	default: return 0;
	}
}

ITCM_CODE
char* Param::AsString() {
    if (type == PT_string || type == PT_identifier) {
        return normal.strval;
    }
    return NULL;
}

ITCM_CODE
SpellBookTemplate* Param::AsSpell(Game* game, u16 fileId) {
    if (type == PT_identifier) {
    	if (normal.cachedSpell) {
    		return normal.cachedSpell;
    	}
    	return normal.cachedSpell = game->runtime.GetSpellTemplate(fileId, normal.strval);
    }
    return NULL;
}

void Param::SetString(const char* str) {
	if (type == PT_random) {
		compilerLog(EL_error, __FILENAME__, "Trying to set the string value of a random variable: %s", str);
		return;
	}
    int strL = strlen(str);

    if (normal.strval) delete[] normal.strval;

    normal.strval = new char[strL + 1];
    strncpy(normal.strval, str, strL);
    normal.strval[strL] = '\0';
}

//==============================================================================

SpellTemplate::SpellTemplate() {
    decl = NULL;
    name = NULL;
    params = new Param*[0];
    paramsL = 0;
}
SpellTemplate::~SpellTemplate() {
    if (name) delete[] name;
    if (params) {
        for (int n = 0; n < paramsL; n++) {
            if (params[n]) {
            	unrefParam(params[n]);
            }
        }
        delete[] params;
    }
}

void SpellTemplate::SetName(const char* s) {
    if (name) delete[] name;

    int nameL = strlen(s) + 1;
    name = new char[nameL];
    strcpy(name, s);
    name[nameL-1] = '\0';
}

void SpellTemplate::AddParam(Param* param) {
    //Copy old array values into a bigger array
    Param** array = new Param*[paramsL + 1];
    for (u16 n = 0; n < paramsL; n++) {
        array[n] = params[n];
    }

    //Delete old array
    delete[] params;
    params = array;

    //Add new element to the array
    params[paramsL] = refParam(param);
    paramsL++;
}

void SpellTemplate::ToString(char* out) {
    strcpy(out, name);
    for (int n = 0; n < paramsL; n++) {
        //Append a space
        out = out + strlen(out);
        out[0] = ' ';

        //Move pointer to string end
        out++;

        //Append param string
        if (params[n]) {
            params[n]->ToString(out);
        } else {
            strcat(out, "NULL");
        }
    }
}

ITCM_CODE
Spell* SpellTemplate::ToSpell(Game* game, u16 fileId, Param** globals, u8 globalsL) {
    //globals could be NULL
    Param* p[paramsL];

    //Resolve variables
    for (int n = 0; n < paramsL; n++) {
        p[n] = resolveVar(params[n], globals, globalsL);
    }

    Spell* spell;
    if ((spell = decl->ToSpell(game, fileId, p, paramsL)) != NULL) {
    	return spell;
    }
   	compilerLog(EL_error, __FILENAME__, "Invalid spell: %s", name);
    return NULL;
}

//==============================================================================

SpellBookTemplate::SpellBookTemplate(u16 fid, int t) {
    fileId = fid;
    type = t;

    name = NULL;
    spells = new SpellTemplate*[0];
    spellsL = 0;
}
SpellBookTemplate::~SpellBookTemplate() {
    if (name) delete[] name;

    if (spells) {
        for (u16 n = 0; n < spellsL; n++) {
            if (spells[n]) delete spells[n];
        }
        delete[] spells;
    }
}

void SpellBookTemplate::SetName(char* s) {
    if (name) delete[] name;

    int nameL = strlen(s) + 1;
    name = new char[nameL];
    strcpy(name, s);
    name[nameL-1] = '\0';
}

void SpellBookTemplate::AddSpell(SpellTemplate* spell) {
    //Copy old array values into a bigger array
    SpellTemplate** array = new SpellTemplate*[spellsL + 1];
    for (int n = 0; n < spellsL; n++) {
        array[n] = spells[n];
    }

    //Delete old array
    delete[] spells;
    spells = array;

    //Add new element to the array
    spells[spellsL] = spell;
    spellsL++;
}

u8 SpellBookTemplate::GetNumberOfVariables() {
	int maxVal = 0;
	for (int n = 0; n < spellsL; n++) {
		SpellTemplate* spell = spells[n];
		for (int x = 0; x < spell->paramsL; x++) {
			if (spell->params[x]->type == PT_variable) {
		        maxVal = MAX(maxVal, spell->params[x]->normal.numval);
			}
		}
	}
	return maxVal;
}

ITCM_CODE
SpellBook* SpellBookTemplate::ToSpellBook(Game* game,
    SpellTemplate** spells, int spellsL, Param** params, u8 paramsL)
{
    Spell* compiledSpells[spellsL];
    int loopcount = 1;

    for (int n = 0; n < spellsL; n++) {
        if (spells[n]->decl != sd_loop && spells[n]->decl != sd_for) {
        	compiledSpells[n] = spells[n]->ToSpell(game, fileId, params, paramsL);
        	continue;
        }

        compiledSpells[n] = NULL;
        if (spells[n]->decl == sd_loop) {
            if (n < spellsL-1) {
                compilerLog(EL_error, __FILENAME__,
                		"Loop instruction before end of spell list: _loop %d",
                		spells[n]->params[0]->AsInt());
            } else {
                loopcount = (spells[n]->paramsL > 0 ? spells[n]->params[0]->AsInt() : -1);
            }
            continue;
        } else if (spells[n]->decl == sd_for) {
        	compiledSpells[n] = new ForSpell(game, fileId, spells[n]->params, spells[n]->paramsL, spells[n+1], params, paramsL);
        	n++;
        	compiledSpells[n] = NULL;
            continue;
        } else {
            compilerLog(EL_error, __FILENAME__,
            		"Unsupported non-regular spell: %s", spells[n]->decl->GetName());
        }
    }
    return new SpellBook(compiledSpells, spellsL, loopcount);
}

//==============================================================================

ObjectTemplate::ObjectTemplate(u16 fileId, int type)
:	SpellBookTemplate(fileId, type), initSpellsStart(0), lifeSpellsStart(0),
 	deathSpellsStart(0)
{
}

ObjectTemplate::~ObjectTemplate() {

}

ITCM_CODE
Sprite* ObjectTemplate::ToObject(Game* game, SpriteType spriteType, Param** params, u8 paramsL) {
    Sprite* sprite = new Sprite(game, spriteType);
    InitObject(game, sprite, params, paramsL);
    return sprite;
}

ITCM_CODE
Boss* ObjectTemplate::ToBoss(Game* game, Param** params, u8 paramsL) {
    Boss* boss = new Boss(game);
    InitObject(game, boss, params, paramsL);
    return boss;
}

ITCM_CODE
void ObjectTemplate::InitObject(Game* game, Sprite* sprite, Param** params, u8 paramsL) {
	if (lifeSpellsStart - initSpellsStart) {
		sprite->SetInitSpellBook(ToSpellBook(game, spells + initSpellsStart,
			lifeSpellsStart - initSpellsStart, params, paramsL));
	}
	if (deathSpellsStart - lifeSpellsStart) {
		sprite->SetLifeSpellBook(ToSpellBook(game, spells + lifeSpellsStart,
			deathSpellsStart - lifeSpellsStart, params, paramsL));
	}
	if (spellsL - deathSpellsStart) {
		sprite->SetDeathSpellBook(ToSpellBook(game, spells + deathSpellsStart,
			spellsL - deathSpellsStart, params, paramsL));
	}
}
