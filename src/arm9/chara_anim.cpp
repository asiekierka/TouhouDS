#include "chara_anim.h"
#include "spells/spellbook.h"

CharaAnimationSet::CharaAnimationSet() {
	a_idle = NULL;
	a_left = a_right = NULL;
	a_tweenLeft = a_tweenRight = NULL;
}
CharaAnimationSet::~CharaAnimationSet() {
	if (a_idle) delete a_idle;
	if (a_left) delete a_left;
	if (a_right) delete a_right;
	if (a_tweenLeft) delete a_tweenLeft;
	if (a_tweenRight) delete a_tweenRight;
}

SpellBook* CharaAnimationSet::GetAnimation(CharaAnimationType type) {
	switch (type) {
	case CAT_left: return a_left;
	case CAT_right: return a_right;
	case CAT_tweenLeft: return a_tweenLeft;
	case CAT_tweenRight: return a_tweenRight;
	default: return a_idle;
	}
}

void CharaAnimationSet::SetAnimation(CharaAnimationType type, SpellBook* sb) {
	switch (type) {
	case CAT_left: SetAnimation(a_left, sb); break;
	case CAT_right: SetAnimation(a_right, sb); break;
	case CAT_tweenLeft: SetAnimation(a_tweenLeft, sb); break;
	case CAT_tweenRight: SetAnimation(a_tweenRight, sb); break;
	default: SetAnimation(a_idle, sb); break;
	}
}

void CharaAnimationSet::SetAnimation(SpellBook*& a, SpellBook* b) {
	if (a) {
		delete a;
	}
	a = b;
}
