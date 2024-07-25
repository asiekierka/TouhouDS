#ifndef CHARA_ANIM_H
#define CHARA_ANIM_H

#include "thcommon.h"

class CharaAnimationSet {
	private:
		SpellBook* a_idle;
		SpellBook* a_left;
		SpellBook* a_right;
		SpellBook* a_tweenLeft;
		SpellBook* a_tweenRight;

		void SetAnimation(SpellBook*& a, SpellBook* b);

	public:

		CharaAnimationSet();
		virtual ~CharaAnimationSet();

		SpellBook* GetAnimation(CharaAnimationType type);

		void SetAnimation(CharaAnimationType type, SpellBook* sb);
};

#endif
