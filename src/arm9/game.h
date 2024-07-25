#ifndef GAME_H
#define GAME_H

#define MAX_VRAM_HANDLERS 16
#define BANNER_TIME (5 * FPS)
#define NUM_TEXTURES (NUM_TEXTURE_TYPES+3)

#define REMOVE_LIST_SIZE 1024

#include "collision.h"
#include "sound.h"
#include "parser/parser_c.h"
#include "parser/runtime.h"

class PauseMenu;

class Game {
    private:
        //Menu's & Resources
        Text* text;
        SpriteEntry* osdSprites;
        PauseMenu* pauseMenu;
        OSD* osd;

        //Vars
        Conversation* conversation;
        Boss* boss;
        Sprite* removeList[REMOVE_LIST_SIZE];
        u32 removeListIndex;
        VRAMHandler* vramHandlers[MAX_VRAM_HANDLERS];
        int vramHandlersL;

        u32  frame;
        int  darkness;
        bool stageEnd;
        bool restart;
        bool quit;

        GameInfo* selectedGame;
        CharInfo* selectedCharacter;
        u8  route;
        u8  stage;
        u32 hiScore;

        void VideoInit();
        void ResetSprites(u16* spriteI);

        void OnGameEnd();
        void CleanGarbage();
        void UpdateSprites(bool drawOnly);
        void Update();
        void MenuUpdate();
        void DrawBackground();
        void UpdateOAM();

    public:
        int  error;
        u32  score;
        bool shotExploded;
        u8   shotExplodedCooldown;
        bool remotePlay;
        bool practice;
        u8   startingLives;

        u16* main_bg;
        int  bg3;
        bool swapScreens;
        s32  menu;
        u16  osdMenuI[256 * 32];
        u16  osdGameI[256 * 32];
        SpriteEntry mainSprites[SPRITE_COUNT];
        SpriteEntry subSprites[SPRITE_COUNT];
        SoundManager soundManager;
        TextureManager texmgr;
        Runtime runtime;
        Level* level;
        Player* players[MAX_PLAYERS+1];
        SpriteList* sprites[NUM_SPRITE_TYPES];
        ColGrid colGrid;

        Texture* textures[NUM_TEXTURES];
        Texture* texBomb1;
        Texture* texBomb2;
        Texture* texOverlay; //Uses main_bg as a full-screen texture

        Game(bool practice, bool remotePlay, u8 startLife);
        virtual ~Game();

        //Returns 0 on success
        int Start(GameInfo* gi, CharInfo* ci, u8 route, u8 stage, bool restart=true);

        void Run();
        void Restart();
        void Quit();
        void DestroyLevel();

        void AddVRAMHandler(VRAMHandler* h);
        void AddSprite(Sprite* s);
        void RemoveSprite(Sprite* s);
        void ConvertSpritesToPoints(u8 type);
        void OnBombUsed(); //Player used a bomb
        void OnPlayerKilled(); //Life lost
        void OnPlayerDead(); //Life lost, no lives left
        void EndStage();

        bool CalculateAngle(s32* out, s32 x, s32 y, AngleTarget target);
        s32 CalculateAngle(s32 x1, s32 y1, s32 x2, s32 y2);
        Text* GetText();
        Boss* GetBoss();
        Conversation* GetConversation();
        u32 GetHiScore();
        GameInfo* GetSelectedGame();
        CharInfo* GetSelectedCharacter();
        u8 GetStage();
        Texture* GetTexture(TextureType type);

        Texture* LoadTexture(const char* folder, const char* textureId,
				GL_TEXTURE_PARAM_ENUM param=TEXGEN_TEXCOORD);
        void SetTexPlayer(const char* playerId, const char* textureId);
        void SetTexBomb(const char* bombId, const char* textureId, bool isPrimary);
        void SetBoss(Boss* b);
        void SetConversation(Conversation* c);
        void SetHiScore(u32 hiScore);
};

#endif
