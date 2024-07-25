#include "spell_decls.h"
#include "common_spells.h"
#include "../parser/compiler.h"
#include "../parser/runtime.h"

#define CHECK_PARAM_TYPE_ERROR 1
#define CHECK_PARAM_CALL_ERROR 2

EnumType textureEnum("tex_enemy|tex_bullet|tex_bullet2|tex_item|tex_player|tex_player_fx|tex_background|tex_boss");
EnumType itemTypeEnum("point|point_large|power|power_large|extra|extra_large|life|bomb|full_power|magnet_point");
EnumType angleTargetEnum("player|boss|enemy|center|top|bottom|left|right");
EnumType charaAnimationTypeEnum("idle|left|right|tween_left|tween_right");
EnumType drawAngleModeEnum("auto|manual|rotate");

SpellDecl** spellDecls = NULL;
int spellDeclsL = 0;

SpellDecl* sd_loop = NULL;
SpellDecl* sd_for = NULL;

void initSpellDecls() {
	if (spellDecls) {
		for (int n = 0; n < spellDeclsL; n++) {
			delete spellDecls[n];
		}
		delete[] spellDecls;
	}

	spellDecls = new SpellDecl*[128];
	spellDeclsL = 0;

	spellDecls[spellDeclsL++] = sd_loop = new SpellDecl("_loop", NULL);
	sd_loop->SetValidContexts(SPT_spell|SPT_object|SPT_boss|SPT_conversation|SPT_animation);
	sd_loop->SetTypesig(0, SPT_null);
	sd_loop->SetTypesig(1, SPT_intConst);

	spellDecls[spellDeclsL++] = sd_for = new SpellDecl("_for", NULL);
	sd_for->SetTypesig(0, SPT_float, SPT_float, SPT_float, SPT_null);

	//level spells
	WaitSpell::CreateDecls(spellDecls, spellDeclsL);
	EndStageSpell::CreateDecls(spellDecls, spellDeclsL);
	BGMSpell::CreateDecls(spellDecls, spellDeclsL);
	SFXSpell::CreateDecls(spellDecls, spellDeclsL);
	BGControlSpell::CreateDecls(spellDecls, spellDeclsL);

	//conv spells
	SaySpell::CreateDecls(spellDecls, spellDeclsL);
	ConvImageSpell::CreateDecls(spellDecls, spellDeclsL);

	//object spells
	ObjectImageSpell::CreateDecls(spellDecls, spellDeclsL);
	AdvanceSpell::CreateDecls(spellDecls, spellDeclsL);
	AdvanceSideSpell::CreateDecls(spellDecls, spellDeclsL);
	DieSpell::CreateDecls(spellDecls, spellDeclsL);
	DropSpell::CreateDecls(spellDecls, spellDeclsL);
	MoveSpell::CreateDecls(spellDecls, spellDeclsL);
	MoveToSpell::CreateDecls(spellDecls, spellDeclsL);
	SpeedToSpell::CreateDecls(spellDecls, spellDeclsL);
	TurnToSpell::CreateDecls(spellDecls, spellDeclsL);
	HitBoxSpell::CreateDecls(spellDecls, spellDeclsL);
	PowerSpell::CreateDecls(spellDecls, spellDeclsL);
	SpeedSpell::CreateDecls(spellDecls, spellDeclsL);
	AngleSpell::CreateDecls(spellDecls, spellDeclsL);
	VisibilitySpell::CreateDecls(spellDecls, spellDeclsL);
	SizeSpell::CreateDecls(spellDecls, spellDeclsL);
	DrawAngleSpell::CreateDecls(spellDecls, spellDeclsL);

	//boss spells
	BossNameSpell::CreateDecls(spellDecls, spellDeclsL);
	BossNumSpellcards::CreateDecls(spellDecls, spellDeclsL);
	BossCallSpell::CreateDecls(spellDecls, spellDeclsL);

	//create spells
	AnimationSpell::CreateDecls(spellDecls, spellDeclsL);
	CharaAnimationSpell::CreateDecls(spellDecls, spellDeclsL);
	FireSpell::CreateDecls(spellDecls, spellDeclsL);
	CreateSpell::CreateDecls(spellDecls, spellDeclsL);
	CallSpell::CreateDecls(spellDecls, spellDeclsL);
	ForkSpell::CreateDecls(spellDecls, spellDeclsL);
	ConversationSpell::CreateDecls(spellDecls, spellDeclsL);

	//Lua
	LuaSpell::CreateDecls(spellDecls, spellDeclsL);
}

SpellDecl* getSpellDecl(const char* name) {
	for (int n = 0; n < spellDeclsL; n++) {
		if (strcmp(name, spellDecls[n]->GetName()) == 0) {
			return spellDecls[n];
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------

EnumType::EnumType(const char* opts) {
	optionString = strdup(opts);

	optionsL = 1;
	for (int n = 0; optionString[n] != '\0'; n++) {
		if (optionString[n] == '|') optionsL++;
	}

	options = new char*[optionsL];

	int t = 0;
	options[t++] = optionString;
	for (int n = 0; optionString[n] != '\0'; n++) {
		if (optionString[n] == '|') {
			optionString[n] = '\0';
			options[t++] = &optionString[n+1];
		}
	}
}
EnumType::~EnumType() {
	if (optionString) free(optionString);
	delete[] options;
}

bool EnumType::IsMember(const char* str) {
	if (!str) return false;

	for (int n = 0; n < optionsL; n++) {
		if (strcmp(options[n], str) == 0) {
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------

SpellDecl::SpellDecl(const char* n, Spell* (*cf)(Game*, u16, Param**, u8)) {
	name = strdup(n);
	validContexts = SPT_spell|SPT_object|SPT_boss;
	createFunc = cf;

	typeSignaturesL = 0;
	SetTypesig(0, PT_null);
}
SpellDecl::~SpellDecl() {
	free(name);
}

const char* SpellDecl::GetName() {
	return name;
}

bool SpellDecl::IsValidContext(u32 c) {
	return validContexts & c;
}
void SpellDecl::SetValidContexts(u32 c) {
	validContexts = c;
}

void SpellDecl::SetTypesig(int index, ...) {
    va_list args;
    va_start(args, index);

    SpellParamType pt;
    int t = 0;
    while ((pt = (SpellParamType)va_arg(args, int)) != SPT_null) {
    	if (t >= 7) {
    		log(EL_error, __FILENAME__, "Too many type parameters in typesig");
    		return;
    	}
    	typeSignatures[index][t++] = pt;
    }
    typeSignatures[index][t++] = SPT_null;
    va_end(args);

    typeSignaturesL = MAX(typeSignaturesL, index+1);
}

bool SpellDecl::TypeCheck(Runtime* runtime, SpellBookTemplate* sbt, SpellTemplate* spell) {
	bool printErrors = false;

	for (int sig = 0; sig < typeSignaturesL; sig++) {
		bool ok = true;

		int typeParamIndex = 0;
		int paramIndex = 0;
        while (typeSignatures[sig][typeParamIndex] != SPT_null) {
        	if (TypeCheckParam(runtime, sbt, spell, sig, &typeParamIndex, paramIndex, printErrors) != 0) {
				ok = false;
        		break;
        	}
        	paramIndex++;
        }
        if (ok || printErrors) {
        	//Quit when OK or the first error has been printed
        	return ok;
        }

        //Go for a second round, but now printing errors
        if (!printErrors && sig == typeSignaturesL - 1) {
        	printErrors = true;
        	sig = -1;
        }
	}
	return false;
}

//Checks if @param is acceptable to use as a param of type @type
int SpellDecl::TypeCheckParam(Runtime* runtime, SpellBookTemplate* sbt,
		SpellTemplate* spell, int sig, int* typeParamIndex, int n, bool printErrors)
{
	int tpi = *typeParamIndex;
    SpellParamType type = typeSignatures[sig][tpi];
    Param* param = spell->params[n];
    ParamType pt = param->type;

    bool ok = false;
	switch (type) {

		//Primitives
		case SPT_int:
			ok |= pt==PT_random || pt==PT_variable;
		case SPT_intConst:
			ok |= pt==PT_int;
			break;
		case SPT_float:
			ok |= pt==PT_random || pt==PT_variable;
		case SPT_floatConst:
			ok |= pt==PT_int || pt==PT_float;
			break;
		case SPT_string:
			ok |= pt==PT_variable;
		case SPT_stringConst:
			ok |= pt==PT_string;
			break;

		//Enums
		case SPT_texture:
			ok |= pt==PT_variable;
			ok |= pt==PT_identifier && textureEnum.IsMember(param->normal.strval);
			break;
		case SPT_itemType:
			ok |= pt==PT_variable;
			ok |= pt==PT_identifier && itemTypeEnum.IsMember(param->normal.strval);
			break;
		case SPT_angleTarget:
			ok |= pt==PT_variable;
			ok |= pt==PT_identifier && angleTargetEnum.IsMember(param->normal.strval);
			break;
		case SPT_charaAnimationType:
			ok |= pt==PT_variable;
			ok |= pt==PT_identifier && charaAnimationTypeEnum.IsMember(param->normal.strval);
			break;
		case SPT_drawAngleMode:
			ok |= pt==PT_variable;
			ok |= pt==PT_identifier && drawAngleModeEnum.IsMember(param->normal.strval);
			break;

		//Objects
		case SPT_spell:
		case SPT_object:
		case SPT_boss:
		case SPT_conversation:
		case SPT_animation:
			ok |= pt==PT_variable;
			if (pt == PT_identifier) {
				int t = SPELL;
				if (type == SPT_spell) t = SPELL;
				else if (type == SPT_object) t = OBJECT;
				else if (type == SPT_boss) t = BOSS;
				else if (type == SPT_conversation) t = CONVERSATION;
				else if (type == SPT_animation) t = ANIMATION;
				else compilerLog(EL_error, __FILENAME__, "Internal Error: unknown type of define: %d", type);

				SpellBookTemplate* target = runtime->GetSpellTemplate(sbt->fileId, param->normal.strval, true);
				if (target) { //Non-existing targets are detected in checkCall
					ok |= target->type == t;
				}
			}
			break;

		//Special
		case SPT_callFlag:
			if (typeSignatures[sig][tpi+1] != SPT_null) {
				tpi++;
				if (TypeCheckParam(runtime, sbt, spell, sig, &tpi, n, printErrors) == 0) {
					n++;
					ok = TypeCheckCall(runtime, sbt->fileId, spell, sig, n);

					//Finished
					while (typeSignatures[sig][tpi] != SPT_null) {
						tpi++;
					}
					tpi--;

					if (!ok) {
						if (printErrors) {
							char buffer[512];
							spell->ToString(buffer);
							compilerLog(EL_error, __FILENAME__,
									"Invalid reference (typo?):\n"
									"  here: %s\n", buffer);
						}
						*typeParamIndex = tpi;
						return CHECK_PARAM_CALL_ERROR;
					}
				}
			}
			break;

		default:
			if (printErrors) {
				compilerLog(EL_error, __FILENAME__, "Internal Error: unknown type of parameter: %d", type);
				return CHECK_PARAM_TYPE_ERROR;
			}
	}

	*typeParamIndex = tpi + 1;

	if (!ok) {
		if (printErrors) {
			char buffer[512];
			spell->ToString(buffer);

			compilerLog(EL_error, __FILENAME__,
					"Type error in define \"%s\" (%d):\n  here: %s\n"
					"Check type + number of params + typo's.",
					sbt->name, n, buffer);
		}
		return CHECK_PARAM_TYPE_ERROR;
	}
	return 0;
}

bool SpellDecl::TypeCheckCall(Runtime* runtime, int fileId, SpellTemplate* spell,
		int sig, int paramIndex)
{
	int paramsLeft = spell->paramsL - paramIndex;
	Param* nameParam = spell->params[paramIndex-1];
    SpellParamType spellType = typeSignatures[sig][paramIndex-1];
	char* name = nameParam->AsString();
	if (!name) {
		//Spell that is to be called isn't a constant
		return true;
	}

	SpellBookTemplate* target = runtime->GetSpellTemplate(fileId, name, true);
	if (!target) {
		//Can't find spell, for some reason

		//conv is a partial function, the default conversation doesn't have to exist
		if (spellType == SPT_conversation) return true;

		//...for other types, invalid references are not allowed
		return false;
	}

	if (paramsLeft < target->GetNumberOfVariables()) { //Too many vars isn't a problem
        return false;
	}
	return true;
}

Spell* SpellDecl::ToSpell(Game* game, u16 fileId, Param** globals, u8 globalsL) {
	if (!createFunc) return NULL;
	return (*createFunc)(game, fileId, globals, globalsL);
}

//-----------------------------------------------------------------------------
