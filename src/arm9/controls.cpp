#include "controls.h"
#include <ctype.h>
#include "tcommon/parser/ini_parser.h"

Controls controls;

Controls::Controls() {
    fireButton = KEY_A;
    focusButton = KEY_B;
    bombButton = KEY_R;
}
Controls::~Controls() {
}

void Controls::SetButton(u32* button, char* string) {
    for (char* t = string; *t != '\0'; t++) {
        *t = tolower(*t);
    }

    if (strcmp(string, "a") == 0) {
        *button = KEY_A;
    } else if (strcmp(string, "b") == 0) {
        *button = KEY_B;
    } else if (strcmp(string, "x") == 0) {
        *button = KEY_X;
    } else if (strcmp(string, "y") == 0) {
        *button = KEY_Y;
    } else if (strcmp(string, "l") == 0) {
        *button = KEY_L;
    } else if (strcmp(string, "r") == 0) {
        *button = KEY_R;
    } else if (strcmp(string, "start") == 0) {
        *button = KEY_START;
    } else if (strcmp(string, "select") == 0) {
        *button = KEY_SELECT;
    }
}

void Controls::ButtonToString(char* out, u32 button) {
    switch (button) {
        case KEY_A: strcpy(out, "A"); return;
        case KEY_B: strcpy(out, "B"); return;
        case KEY_X: strcpy(out, "X"); return;
        case KEY_Y: strcpy(out, "Y"); return;
        case KEY_L: strcpy(out, "L"); return;
        case KEY_R: strcpy(out, "R"); return;
        case KEY_START: strcpy(out, "START"); return;
        case KEY_SELECT: strcpy(out, "SELECT"); return;
    }
}

void Controls::Load() {
    IniFile* iniFile = new IniFile();

    IniRecord* r;
    if (iniFile->Load("controls.ini")) {
        r = iniFile->GetRecord("fire");
        if (r) SetButton(&fireButton, r->AsString());
        r = iniFile->GetRecord("focus");
        if (r) SetButton(&focusButton, r->AsString());
        r = iniFile->GetRecord("bomb");
        if (r) SetButton(&bombButton, r->AsString());
    }

    delete iniFile;
}

void Controls::Save() {
    char temp[64];

    FILE* file = fopen("controls.ini", "w");
    if (file) {
        ButtonToString(temp, fireButton);
        fprintf(file, "fire=%s\n", temp);
        ButtonToString(temp, focusButton);
        fprintf(file, "focus=%s\n", temp);
        ButtonToString(temp, bombButton);
        fprintf(file, "bomb=%s\n", temp);

        fclose(file);
    }
}
