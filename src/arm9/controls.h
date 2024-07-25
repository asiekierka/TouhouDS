#ifndef CONTROLS_H
#define CONTROLS_H

#include "thcommon.h"

class Controls {
    private:

        void SetButton(u32* button, char* string);
        void ButtonToString(char* out, u32 button);

    public:
        u32 fireButton;
        u32 focusButton;
        u32 bombButton;

        Controls();
        virtual ~Controls();

        void Load();
        void Save();
};

extern Controls controls;

#endif
