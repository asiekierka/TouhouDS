#include "game.h"

#include "../common/fifo.h"
#include "tcommon/filehandle.h"
#include "tcommon/text.h"

#include "boss.h"
#include "charinfo.h"
#include "conversation.h"
#include "collision.h"
#include "gameinfo.h"
#include "item.h"
#include "level.h"
#include "osd.h"
#include "player.h"
#include "spritelist.h"
#include "vram_handler.h"
#include "lua/thlua.h"
#include "menu/pausemenu.h"
#include "remote/remote.h"

#define DEFAULT_SHOT_EXPLOSION_COOLDOWN 8

#ifdef PROFILE

int numSprites = 0;
int framesPassed = 0;
int maxFrameSkip = 0;

void profileVBlank() {
	framesPassed++;
}

#endif

using namespace std;

Game::Game(bool practice, bool remotePlay, u8 startLife) {
    level = NULL;
    osd = NULL;
    pauseMenu = NULL;

    for (int n = 0; n <= MAX_PLAYERS; n++) {
    	players[n] = NULL;
    }

    conversation = NULL;
    boss = NULL;
    swapScreens = false;
    vramHandlersL = 0;
    frame = 1;
    menu = 0;
    darkness = 0;
    restart = quit = false;
    stageEnd = false;

    removeListIndex = 0;
    sprites[SPRITE_default]    = new SpriteList(256);
    sprites[SPRITE_remote]     = new SpriteList(512);
    sprites[SPRITE_player]     = new SpriteList(16);
    sprites[SPRITE_playerShot] = new SpriteList(512);
    sprites[SPRITE_item]       = new SpriteList(2048);
    sprites[SPRITE_enemy]      = new SpriteList(1024);
    sprites[SPRITE_enemyShot]  = new SpriteList(4096);

    selectedGame = NULL;
    selectedCharacter = NULL;
    route = 0;
    stage = 1;
    score = 0;
    hiScore = 0;
    startingLives = startLife;

    this->remotePlay = remotePlay;
    this->practice = practice;

    text = new Text();
    text->SetBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    text->SetMargins(5, 5, 5, 5);

    soundManager.Init("snd");
    VideoInit();
}

Game::~Game() {
    DestroyLevel();

   	if (players[0] && --players[0]->refcount == 0) {
   		delete players[0];
   	}

    if (text) delete text;
    if (osd) delete osd;
    if (pauseMenu) delete pauseMenu;

    for (int n = 0; n < NUM_SPRITE_TYPES; n++) {
    	delete sprites[n];
    }

    videoSetMode(MODE_5_3D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256);
	videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256);
    lcdSwap();
    fadeBlack(16);

    init3D();
}

void Game::DestroyLevel() {
    CleanGarbage();

    SetConversation(NULL);

    for (int n = 0; n < NUM_SPRITE_TYPES; n++) {
    	SpriteList* list = sprites[n];

    	u32 index = 0;
		Sprite* sprite;
		while (!SL_Done(*list, index)) {
			sprite = SL_Next(*list, index);
			if (sprite && sprite != players[0]) {
				if (--sprite->refcount == 0) {
					delete sprite;
				}
			}
		}

		SL_Clear(*list);
    }
    for (int n = 1; n <= MAX_PLAYERS; n++) {
    	players[n] = NULL;
    }

    boss = NULL;

    //Destroy level
    if (level) {
        delete level;
        level = NULL;
    }

    for (int n = 0; n < vramHandlersL; n++) {
    	delete vramHandlers[n];
    }
    vramHandlersL = 0;

    shotExplodedCooldown = 0;
    soundManager.StopAll();
    soundManager.RemoveUserSounds();
}

int Game::Start(GameInfo* gi, CharInfo* ci, u8 r, u8 s, bool restart) {
	fadeBlack(1);
	darkness = 120;

	error = 0;
    selectedGame = gi;
    selectedCharacter = ci;
    route = r;
    stage = s;

    DestroyLevel();

    if (restart) {
        srand(time(0));
    }

    bool oldPlayerExists = (players[0] != NULL);

    u32 pts = 237;
    u16 sp = 237;
    u16 ep = 237;
    u16 g = 237;
    u8 l = 237;
    u8 b = 237;

    if (oldPlayerExists) {
        pts = players[0]->GetPoints();
        sp = players[0]->GetShotPower();
        ep = players[0]->GetExtraPoints();
        g = players[0]->GetGraze();
        l = players[0]->GetLives();
        b = players[0]->GetBombs();

		if (--players[0]->refcount == 0) {
			delete players[0];
		}
		players[0] = NULL;
	}

    runtime.Reset();

    resetCompilerLog();

    char scriptFolder[256];
    sprintf(scriptFolder, "games/%s/%s/stage%d", gi->GetId(), gi->GetRouteId(route), stage);

	char charaScriptFolder[256];
	sprintf(charaScriptFolder, "chara/%s", ci->GetId());

	char scriptIniPath[256];
	sprintf(scriptIniPath, "%s.ini", scriptFolder);
	if (!fexists(scriptIniPath)) {
		error = COMPILE_DIR_NOT_FOUND;
	}

    if (error == 0) error = runtime.Compile(scriptFolder, charaScriptFolder);

    if (error == 0) error = runtime.CompileLuaBase();
    if (error == 0) error = runtime.CompileLuaFolder("script");
    if (error == 0) error = runtime.CompileLuaFolder(charaScriptFolder);
    if (error == 0) error = runtime.CompileLuaFolder(scriptFolder);

    bool finished = (practice && stageEnd);
    if (finished || error == COMPILE_DIR_NOT_FOUND) {
        if (finished || (!restart && stage > 1)) {
        	error = 0;
            stage = 255; //Finished all stages=255
            OnGameEnd(); //No more stages
        } else {
            compilerLog(EL_error, __FILENAME__, "Empty or non-existing folder: %s", scriptFolder);
        }
    } else if (error == 0) {
    	SpellBookTemplate* main = runtime.GetSpellTemplate(runtime.GetFileId("main"), "main");

    	luaGameInit(this);

		stageEnd = false;
		frame = (swapScreens ? 0 : 1);

		texmgr.Reset();
		memset(textures, 0, sizeof(Texture*));
		texBomb1 = textures[NUM_TEXTURE_TYPES] = NULL;
		texBomb2 = textures[NUM_TEXTURE_TYPES+1] = NULL;
		texOverlay = textures[NUM_TEXTURES-1] = NULL;

		level = new Level(this, score, scriptFolder);

		if (main) {
			level->AddThread(main->ToSpellBook(this, main->spells, main->spellsL));
		}

		texmgr.AddTexture(TEXTURE_SIZE_256, TEXTURE_SIZE_256, GL_RGBA,
			(GL_TEXTURE_PARAM_ENUM)(TEXGEN_TEXCOORD|GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T),
			NULL, 256*192*2, NULL, 0
		);

		players[0] = ci->CreatePlayer(this, 0);
		AddSprite(players[0]);

		if (oldPlayerExists && !restart) {
			players[0]->SetPoints(pts);
			players[0]->SetShotPower(sp);
			players[0]->SetExtraPoints(ep);
			players[0]->SetGraze(g);
			players[0]->SetLives(l);
			players[0]->SetBombs(b);
	    }

		if (remotePlay) {
			remote.CreateRemotePlayers(this, players, 1, ci->GetId());
			for (int n = 1; n <= MAX_PLAYERS; n++) {
				if (players[n]) AddSprite(players[n]);
			}
		}
    }

	unfadeBlack(1);
    return error;
}

void Game::ResetSprites(u16* spriteI) {
    DC_FlushRange(spriteI, 256*32*sizeof(u16));
    dmaCopy(spriteI, SPRITE_GFX, 256*32*sizeof(u16));

    for (int n = 0; n < 128; n++) {
        mainSprites[n].attribute[0] = ATTR0_DISABLED;
        subSprites[n].attribute[0]  = ATTR0_DISABLED;
    }

    //Reserve 256x192 rect for screen capture
    //Last attribute in SpriteEntry contains tileIndex: ((ty<<5)|(tx))
    //each tile is 8 pixels, for 4*8*8=256 by 3*8*8=192 pixels total
   	int i = 0;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 4; x++) {
            subSprites[i].attribute[0] = ATTR0_BMP | ATTR0_SQUARE | (64 * y);
            subSprites[i].attribute[1] = ATTR1_SIZE_64 | (64 * x);
            subSprites[i].attribute[2] = ATTR2_ALPHA(1) | ATTR2_PRIORITY(3) | (8*(y<<5)) | (8*x);
            i++;
        }
    }

    //Main screen sprites
    i = 0;
    osdSprites = mainSprites + i;
    int numOsdSprites = 64;
    for (int n = 0; n < numOsdSprites; n++) {
        int x = 8 * (n < numOsdSprites/2 ? n : n - numOsdSprites/2);
        int y =     (n < numOsdSprites/2 ? 0 : 192 - 8);
        mainSprites[i].attribute[0] = ATTR0_DISABLED | ATTR0_BMP | ATTR0_SQUARE | (y);
        mainSprites[i].attribute[1] = ATTR1_SIZE_8 | (x);
        mainSprites[i].attribute[2] = ATTR2_ALPHA(1) | ATTR2_PRIORITY(1);
        i++;
    }

    //UpdateOAM();
}

void Game::VideoInit() {
    resetVideo();
    lcdMainOnTop();

	vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_MAIN_BG_0x06000000);
    vramSetBankC(VRAM_C_SUB_BG_0x06200000);
    vramSetBankD(VRAM_D_SUB_SPRITE);
	vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_MAIN_SPRITE);

    main_bg = (u16*)BG_BMP_RAM(2);
    u16* sub_bg = (u16*)BG_BMP_RAM_SUB(0);
    memset(main_bg, 0, 256*192);
    memset(sub_bg, 0, 256*192);

    //Backgrounds
    REG_BG0CNT = BG_PRIORITY(1);

	int bg1 = bgInit(1, BgType_Text4bpp, BgSize_T_256x256, 7, 0);
	bgSetPriority(bg1, 0);
	BG_PALETTE[0] = RGB15(0, 0, 0);
	BG_PALETTE[255] = RGB15(31, 31, 31);

	bg3 = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 2, 0);
	bgSetPriority(bg3, 3);

	int sbg2 = bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
	bgSetPriority(sbg2, 2);

    //Sprites
    FileHandle* fh = fhOpen("img/game/osd.dta");
    if (fh) {
        u16* dta = new u16[fh->length];
        fh->ReadFully(dta);

        for (u32 n = 0; n < MIN(256*32, fh->length/2); n++) {
            if (n > 256*16 || (dta[n] & BIT(15))) {
                osdMenuI[n] = dta[n];
            } else {
                osdMenuI[n] = BIT(15);
            }
        }
        memcpy(osdGameI, osdMenuI, 256*32*sizeof(u16));

        fhClose(fh);
        delete[] dta;

        //Draw Score/Hi-Score texts
        text->PushState();

        text->ClearBuffer();
        text->SetMargins(128, 0, 0, 64-8);
        text->SetPen(128, 7);
        text->PrintString("score");
        text->SetMargins(128, 8, 0, 64-16);
        text->SetPen(128, 15);
        text->PrintString("hi-score");

        for (int y = 0; y < 16; y++) {
            for (int x = 128; x < 256; x++) {
                osdGameI[(y<<8)|x] = RGB15(0, 0, 0) | BIT(15);
            }
        }
        text->BlitToScreen(osdGameI, 256, 192, 128, 0, 128, 0, 128, 16);

        text->PopState();
    }

    memset(osdGameI + 256*16, 0, 256*16*sizeof(u16));

    //Reset Sprites
    memset(mainSprites, 0, SPRITE_COUNT * sizeof(SpriteEntry));
    memset(subSprites, 0, SPRITE_COUNT * sizeof(SpriteEntry));
    for (int n = 0; n < SPRITE_COUNT; n++) {
    	mainSprites[n].isHidden = true;
    	subSprites[n].isHidden = true;
    }
    memset(SPRITE_GFX, 0, SPRITE_COUNT * sizeof(SpriteEntry));
    ResetSprites(osdGameI);
    UpdateOAM();

    //Other stuff
    if (osd) delete osd;
    osd = new OSD(this, osdSprites);

    if (pauseMenu) delete pauseMenu;
    pauseMenu = new PauseMenu(this, mainSprites + 64);

    //Set video mode
	consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 7, 0, true, true);

	videoSetMode(MODE_5_3D | DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256);
	videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256);
    init3D();
    glFlush(0);
    swiWaitForVBlank();

    consoleClear();
    iprintf("\n"); //Move console down so it doesn't print under the performance info

	glMaterialf(GL_AMBIENT,  RGB15( 0,  0,  0));
	glMaterialf(GL_DIFFUSE,  RGB15(31, 31, 31));
	glMaterialf(GL_SPECULAR, RGB15( 8,  8,  8)); // | BIT(15));
	glMaterialf(GL_EMISSION, RGB15( 0,  0, 16));
    glMaterialShinyness();

	glLight(0, RGB15(0, 0, 31), 0, 0, floattov10(.90));
}

void Game::MenuUpdate() {
    if (swapScreens) {
        for (int n = 64; n < SPRITE_COUNT; n++) {
            mainSprites[n].isHidden = true;
        }
    } else {
        if (menu < 0) { //create menu
            menu = abs(menu);
            dmaCopy(osdMenuI, SPRITE_GFX, 256*32*sizeof(u16));
            soundManager.StopSFX();
        }

        s32 oldMenu = menu;
        menu = pauseMenu->Update(menu);

        if (menu == 0) { //menu disposed
            dmaCopy(osdGameI, SPRITE_GFX, 256*32*sizeof(u16));
            soundManager.StopSFX();

            if (oldMenu == 3) { //3=MODE_RESULTS
                stageEnd = true;
            }
        }
    }
}

ITCM_CODE
void Game::UpdateSprites(bool drawOnly) {
	#ifdef PROFILE
		numSprites = 0;
	#endif

	bool swapScreens = this->swapScreens;

    for (int n = 0; n < NUM_SPRITE_TYPES; n++) {
    	glPolyFmt(TH_DEFAULT_POLY_FMT | POLY_ID(n));
    	if (n == SPRITE_remote) {
    		glPolyFmt(TH_BASE_POLY_FMT|POLY_ID(n)|POLY_ALPHA(1+15));
    	}

    	SpriteList* list = sprites[n];
    	if ((frame+(n<<2)) & 63) {
    		SL_Compact(*list);
    	}

    	Sprite** rawlist = list->values;
    	u32 spritesL = list->size;
    	for (u32 index = 0; index < spritesL; index++) {
    		Sprite* sprite = rawlist[index];
        	if (!sprite) {
        		continue;
        	}

			#ifdef PROFILE
				numSprites++;
			#endif

        	if (!drawOnly) {
       			sprite->Update();
       			if (sprite->listIndex < 0) {
       				continue;
       			}
    			updateColNodes(sprite, &colGrid);
        	}

            if (sprite->dieOutsideBounds || sprite->drawData.clip) {
            	int x = sprite->x;
            	int y = sprite->y;
            	int sz = inttof32(INV_VERTEX_SCALE(
            			MAX(sprite->drawData.vw, sprite->drawData.vh)));

            	if (x + sz < 0 || x - sz >= inttof32(LEVEL_WIDTH)) {
            		if (sprite->dieOutsideBounds && sprite->wasOnScreen) sprite->Kill();
            		continue;
            	}
            	if (swapScreens) {
            		if (y - sz >= inttof32(LEVEL_HEIGHT)) {
                		if (sprite->dieOutsideBounds && sprite->wasOnScreen) sprite->Kill();
            			continue;
            		} else if (y + sz < inttof32(LEVEL_HEIGHT>>1)) {
            			continue;
            		}
        			sprite->wasOnScreen = true;
        		} else {
            		if (y + sz < 0) {
                		if (sprite->dieOutsideBounds && sprite->wasOnScreen) sprite->Kill();
            			continue;
            		} else if (y - sz >= inttof32(LEVEL_HEIGHT>>1)) {
            			continue;
            		}
        			sprite->wasOnScreen = true;
        		}
            }

			sprite->Draw();
        }

        if (n == SPRITE_remote) {
        	//Set remote sprites' power to 0
        	spritesL = sprites[n]->size;
        	for (u32 index = 0; index < spritesL; index++) {
            	if (rawlist[index]) {
            		rawlist[index]->power = 0;
            	}
            }
        }
    }

#ifdef PROFILE
    iprintf("\x1b[s\x1b[0;12H%04dKB mem\x1b[u", HEAP_SIZE>>10);
	iprintf("\x1b[s\x1b[0;23H%04d shot\x1b[u", numSprites);
#endif

}

void Game::Update() {
    if (restart || stageEnd) {
        if (!stageEnd) {
        	if (!practice) {
        		selectedGame->SaveScore(selectedGame->GetRouteId(route),
        				selectedCharacter->GetId(), stage, score);
        	}
            score = 0;
        }

        restart = false;
        int result = Start(selectedGame, selectedCharacter, route,
        		(stageEnd ? stage+1 : stage), !stageEnd);
        if (result != 0 || stage == 255) {
        	error = result;
        	quit = true;
        }
        if (quit) {
            return;
        }
    }

    shotExploded = false;
    if (score > hiScore) hiScore = score;

    //Read input
    scanKeys();

    u32 down = keysDown();
    //u32 held = keysHeld();

    if (down & KEY_SELECT) {
        REG_IPC_FIFO_TX = MSG_TOGGLE_BACKLIGHT;
    }
    if (down & KEY_LID) {
        u16 power_cr = REG_POWERCNT;
        REG_POWERCNT = 0;

        do {
            swiWaitForVBlank();
            scanKeys();
        } while (keysHeld() & KEY_LID);

        REG_POWERCNT = power_cr;
        menu = -1;
    }
    if (down & KEY_START) {
        //Open menu
        menu = -1;
    }

    //Update Level & Sprites
    if (level) {
    	level->Update();
    }
    UpdateSprites(false);

	autoCollectItems(players[0], sprites[SPRITE_item]);
	collide(sprites, &colGrid, players[0]->y <= ITEM_GET_LINE, players[0]->IsInvincible());
	CleanGarbage();

    if (darkness > 0) darkness--;
    if (frame == BANNER_TIME) darkness = 60;

    if (shotExplodedCooldown > 0) {
        shotExplodedCooldown--;
    } else {
        if (shotExploded) {
            shotExplodedCooldown = DEFAULT_SHOT_EXPLOSION_COOLDOWN;
            soundManager.PlaySound("enemy_damaged.wav", 5, false, 10);
        }
    }

    if (remotePlay) {
    	remote.Update(this, frame);
    }
}

void Game::CleanGarbage() {
    //Delete sprites that have been destroyed this frame
	for (u32 n = 0; n < removeListIndex; n++) {
		Sprite* sprite = removeList[n];
		sprite->refcount--;

		if (sprite->refcount == 0) {
			delete sprite;
		} else {
			if (sprite->luaLink) {
				delete sprite->luaLink;
				sprite->luaLink = NULL;
			}
		}
	}
	removeListIndex = 0;
}

Text* Game::GetText() {
    return text;
}

void Game::OnPlayerKilled() {
    if (level) {
        level->OnPlayerKilled();
    }
}
void Game::OnBombUsed() {
    if (level) {
        level->OnBombUsed();
    }
}
void Game::OnPlayerDead() {
	if (!practice) {
		selectedGame->SaveScore(selectedGame->GetRouteId(route), selectedCharacter->GetId(), stage, score);
	}
    score = 0;
    menu = -4; //gameover
}

void Game::OnGameEnd() {
    DestroyLevel();
    quit = true;
}

ITCM_CODE
void Game::AddSprite(Sprite* s) {
    SL_Append(*sprites[s->type], s);
    //updateColNode(s, &colGrid);
}

ITCM_CODE
void Game::RemoveSprite(Sprite* s) {
	if (s->listIndex < 0) return; //You are already dead

	//Remove ColNodes
	u32 colNodesL = s->colNodes.size();
	for (u32 n = 0; n < colNodesL; n++) {
		s->colNodes[n]->listNode.Remove();
	}

	//Remove Sprite
	SL_Remove(*sprites[s->type], s->listIndex);

	//Add to garbage if sprite is not a Player
    if (s->type != SPRITE_player) {
    	if (removeListIndex >= REMOVE_LIST_SIZE) {
    		iprintf("Remove list overflow: %d\n", removeListIndex);
    		waitForAnyKey();
    	}

        removeList[removeListIndex] = s;
        removeListIndex++;
    }
}

void Game::EndStage() {
    menu = -3; //3=MODE_RESULTS

    if (level) level->OnFinished();
}

Boss* Game::GetBoss() {
    return boss;
}
void Game::SetBoss(Boss* b) {
    if (boss) RemoveSprite(boss);

    boss = b;
    if (b) AddSprite(b);
}

Conversation* Game::GetConversation() {
	return conversation;
}
void Game::SetConversation(Conversation* c) {
	if (conversation) {
		delete conversation;
	}
	conversation = c;
}

u32 Game::GetHiScore() {
    return hiScore;
}
void Game::SetHiScore(u32 hs) {
    hiScore = hs;
}

bool Game::CalculateAngle(s32* out, s32 x, s32 y, AngleTarget target) {
    switch (target) {
        case AT_none:
            break;
        case AT_player:
            *out = CalculateAngle(x, y, players[0]->x, players[0]->y);
            return true;
        case AT_boss:
            if (boss) {
                *out = CalculateAngle(x, y, boss->x, boss->y);
                return true;
            }
            break;
        case AT_center:
            *out = CalculateAngle(x, y, inttof32(LEVEL_WIDTH/2), inttof32(LEVEL_HEIGHT/2));
            return true;
        case AT_top:
            *out = 0;
            return true;
        case AT_bottom:
            *out = (DEGREES_IN_CIRCLE>>1);
            return true;
        case AT_left:
            *out = (DEGREES_IN_CIRCLE>>1)+(DEGREES_IN_CIRCLE>>2);
            return true;
        case AT_right:
            *out = (DEGREES_IN_CIRCLE>>2);
            return true;
        case AT_enemy:
            Sprite* closest = NULL;
            u32 closestDist = INT_MAX;

            u32 dist, dx, dy;
            Sprite* sprite;
            SpriteList* list = sprites[SPRITE_enemy];
            u32 index = 0;
            while (!SL_Done(*list, index)) {
                sprite = SL_Next(*list, index);
                if (!sprite) continue;

                dx = abs(x-sprite->x);
                dy = abs(y-sprite->y);
                dist = dx + dy - (MIN(dx, dy)>>1);
                if (dist < closestDist) {
                    closest = sprite;
                    closestDist = dist;
                }
            }
            if (closest) {
                *out = CalculateAngle(x, y, closest->x, closest->y);
                return true;
            }
            break;
    }
    return false;
}
s32 Game::CalculateAngle(s32 x1, s32 y1, s32 x2, s32 y2) {
    return atan2(y2 - y1, x2 - x1);
}

void Game::ConvertSpritesToPoints(u8 type) {
    if (type == SPRITE_default || type == SPRITE_remote || type == SPRITE_player
    		|| type == SPRITE_item)
    {
        log(EL_warning, __FILENAME__, "ConvertSpritesToPoints :: Can't convert sprites of type (%d)", type);
    	return;
    }

    SpriteList* list = sprites[type];
    Sprite* sprite;
    u32 index = 0;
    while (!SL_Done(*list, index)) {
    	sprite = SL_Next(*list, index);
    	if (!sprite || sprite == boss) {
    		continue;
    	}

		Item* item = new Item(this, IT_magnetPoint, 0);
		item->x = sprite->x;
		item->y = sprite->y;

		sprite->Kill();
		AddSprite(item);
    }
}

Texture* Game::LoadTexture(const char* folder, const char* textureId,
		GL_TEXTURE_PARAM_ENUM param)
{
    char* format = strstr(textureId, ":");
    if (!format) {
    	return NULL;
    }

	format++;
	char id[format - textureId];
	strncpy(id, textureId, format - textureId);
	id[format - textureId - 1] = '\0';

	char dtaPath[MAXPATHLEN];
	char palPath[MAXPATHLEN];
	sprintf(dtaPath, "%s/%s.dta", folder, id);
	sprintf(palPath, "%s/%s.pal", folder, id);

	return loadTexture(&texmgr, format, dtaPath, palPath, param);
}
void Game::SetTexPlayer(const char* playerId, const char* textureId) {
    char folder[MAXPATHLEN];
    sprintf(folder, "chara/%s", playerId);
    textures[TEX_player] = LoadTexture(folder, textureId);
}
void Game::SetTexBomb(const char* bombId, const char* textureId, bool isPrimary) {
    char folder[MAXPATHLEN];
    sprintf(folder, "bomb/%s", bombId);

    Texture** tex = (isPrimary ? &texBomb1 : &texBomb2);
    *tex = LoadTexture(folder, textureId);
}

GameInfo* Game::GetSelectedGame() {
    return selectedGame;
}
CharInfo* Game::GetSelectedCharacter() {
    return selectedCharacter;
}
u8 Game::GetStage() {
    return stage;
}

//After adding the vramHandler, Game is responsible for its deletion
void Game::AddVRAMHandler(VRAMHandler* h) {
	if (vramHandlersL >= MAX_VRAM_HANDLERS) {
		log(EL_warning, __FILE__, "Max number of VRAM handlers reached");
		return;
	}

	vramHandlers[vramHandlersL] = h;
	vramHandlersL++;
}

void Game::Restart() {
    restart = true;
}
void Game::Quit() {
    quit = true;
}

void Game::DrawBackground() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspectivef32(35, divf32(inttof32(SCREEN_WIDTH), inttof32(SCREEN_HEIGHT)),
        inttof32(1), inttof32(500));

    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3f(1, 1, 1);

    MATRIX_SCALE = VERTEX_SCALE_FACTOR;
	MATRIX_SCALE = VERTEX_SCALE_FACTOR;
	MATRIX_SCALE = inttof32(1);
	glTranslate3f32(0, 0, inttof32(-1));

    if (swapScreens) {
    	glTranslate3f32(0, inttof32(8), 0);
    } else {
    	glTranslate3f32(0, inttof32(-192+16+8), 0);
    }

    MATRIX_SCALE = INV_VERTEX_SCALE_FACTOR;
	MATRIX_SCALE = INV_VERTEX_SCALE_FACTOR;
	MATRIX_SCALE = INV_VERTEX_SCALE_FACTOR;

  	glPolyFmt(TH_DEFAULT_POLY_FMT | POLY_ID(63));
  	setActiveTexture(textures[TEX_background]);

    Rect uv(level->bgScrollX >> 8, (level->bgScrollY >> 8) & 2047,
    		inttot16(LEVEL_WIDTH), inttot16(LEVEL_HEIGHT<<1));
    u16 horizonColor = RGB15(0, 0, 0);
  	int a = -VERTEX_SCALE(LEVEL_WIDTH>>1);
  	int c =  VERTEX_SCALE(LEVEL_WIDTH>>1);
  	int b = -VERTEX_SCALE(SCREEN_HEIGHT>>1);
  	int d =  VERTEX_SCALE(LEVEL_HEIGHT>>1);
  	int z1 = -VERTEX_SCALE(200);
  	int z2 = -VERTEX_SCALE(290);
  	int z3 = -VERTEX_SCALE(380);

	if (swapScreens) {
		glBegin(GL_QUAD);
			//glNormal(NORMAL_PACK(0, 0, floattov10(1)));
			glColor(RGB15(31, 31, 31));
			glTexCoord2t16(uv.x, uv.y);
			glVertex3v16(a, b, z1);
			glColor(RGB15(31, 31, 31));
			glTexCoord2t16(uv.x + uv.w, uv.y);
			glVertex3v16(c, b, z1);
			glColor(RGB15(31, 31, 31));
			glTexCoord2t16(uv.x + uv.w, uv.y + (uv.h>>1));
			glVertex3v16(c, d/2, z2);
			glColor(RGB15(31, 31, 31));
			glTexCoord2t16(uv.x, uv.y + (uv.h>>1));
			glVertex3v16(a, d/2, z2);
		glEnd();
	} else {
		glBegin(GL_QUAD);
			//glNormal(NORMAL_PACK(0, 0, floattov10(1)));
			glTexCoord2t16(uv.x, uv.y + (uv.h>>1));
			glColor(RGB15(31, 31, 31));
			glVertex3v16(a, d/2, z2);
			glColor(RGB15(31, 31, 31));
			glTexCoord2t16(uv.x + uv.w, uv.y + (uv.h>>1));
			glVertex3v16(c, d/2, z2);
			glColor(horizonColor);
			glTexCoord2t16(uv.x + uv.w, uv.y + uv.h);
			glVertex3v16(c, d*6/4, z3);
			glColor(horizonColor);
			glTexCoord2t16(uv.x, uv.y + uv.h);
			glVertex3v16(a, d*6/4, z3);
		glEnd();
	}

    glPopMatrix(1);
}

void Game::Run() {

#ifdef PROFILE
	irqSet(IRQ_VBLANK, profileVBlank);
	irqEnable(IRQ_VBLANK);

	int sum = 0;
	int frames = 0;
	int lines = 0;
	int startLines = 0;
	int maxFrameTime = 0;
	std::vector<u32> perfLog;

#endif

	swapScreens = true;
    while (!quit) {
    	if (swapScreens) {
            vramSetBankC(VRAM_C_LCD);
            vramSetBankD(VRAM_D_SUB_SPRITE);
            setupCapture(2);
        } else {
            vramSetBankC(VRAM_C_SUB_BG_0x06200000);
            vramSetBankD(VRAM_D_LCD);
            setupCapture(3);
        }
        if (swapScreens) {
            //Hide behind level-bg
            bgSetPriority(bg3, 3);
        } else {
            //Bring to front
            bgSetPriority(bg3, 1);
        }

    	if (conversation && conversation->IsFinished()) SetConversation(NULL);

#ifdef PROFILE
    	startLines = REG_VCOUNT;
    	framesPassed = 0;
#endif

		glReset();
	   	DrawBackground();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	    glOrthof32(0, VERTEX_SCALE(256), VERTEX_SCALE(192-16), 0, inttof32(1), inttof32(500));

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glColor3f(1, 1, 1);

	    MATRIX_SCALE = VERTEX_SCALE_FACTOR;
		MATRIX_SCALE = VERTEX_SCALE_FACTOR;
		MATRIX_SCALE = inttof32(1);

		glPushMatrix();
	    if (swapScreens) {
	        //Hide behind level-bg
	    	bgSetPriority(bg3, 3);
	        glTranslate3f32(0, inttof32(-192+16), inttof32(-10));
	    } else {
	        //Bring to front
	    	bgSetPriority(bg3, 0);
	        glTranslate3f32(0, inttof32(0), inttof32(-10));
	    }

	    //Draw Sprites
	    glViewport(0, 8, 255, 191-8);

	    //Update
		if (menu == 0 && !conversation) {
			Update();
			frame++;
		} else {
			if (menu != 0) {
				MenuUpdate();
			} else if (conversation) {
				conversation->Update();
			}
			UpdateSprites(true);
		}
		soundManager.Update();

		//Continue Drawing
		glPopMatrix(1);

	    glViewport(0, 0, 255, 191);
	    glTranslate3f32(0, 0, inttof32(-5));

		if (conversation && swapScreens) {
			conversation->Draw();
		}
	    if (menu != 0 || frame <= BANNER_TIME || darkness != 0) {
	    	int alpha = MIN(30, MAX(menu != 0 || frame <= BANNER_TIME ? 15 : 0, darkness>>2));

	        //Draw dark overlay
	      	glPolyFmt(TH_BASE_POLY_FMT | POLY_ID(62) | POLY_ALPHA(1+alpha));
	        drawQuad(textures[TEX_playerFx], 0, 0, VERTEX_SCALE(256), VERTEX_SCALE(192),
	                Rect(8, 0, 7, 7));
	    }

		glPopMatrix(1);

		if (swapScreens) {
			osd->DrawBottom(players[0]);
		} else {
			osd->DrawTop();
		}

#ifdef PROFILE
		if (stageEnd) {
			perfLog.clear();
		}
        if (menu == 0 && !conversation) {
        	lines = REG_VCOUNT;
        	int fp = framesPassed;
        	maxFrameSkip = MAX(maxFrameSkip, fp);

        	u32 inc;
        	if (lines < startLines) {
        		inc = lines + (263 - startLines);
        		fp -= 1;
        	} else {
        		inc = lines - startLines;
        	}
        	inc += 263 * MAX(0, fp);
        	perfLog.push_back((inc&0xFFFF)|(numSprites<<16));

        	sum += inc;
        	frames++;
			if ((frames & 15) == 0) {
				int frameTime = sum / frames;
				maxFrameTime = MAX(maxFrameTime, frameTime);
				iprintf("\x1b[s\x1b[0;0H%03d:%03d spd\x1b[u", MIN(999, frameTime),
						MIN(999, maxFrameTime));
				sum = 0;
				frames = 0;
				maxFrameSkip = 0;
			}

			if (keysDown() & KEY_SELECT) {
				iprintf("\x1b[s\x1b[0;0Hdumping perf. log\x1b[u");
				FILE* file = fopen("perflog.txt", "w");
				if (file) {
					for (u32 n = 0; n < perfLog.size(); n++) {
						fprintf(file, "%d;%d\n", perfLog[n]&0xFFFF, perfLog[n]>>16);
					}
					perfLog.clear();
					fclose(file);
				}
				iprintf("\x1b[s\x1b[0;0H                 \x1b[u");
			}
        }
#endif

        glFlush(0);
        swiWaitForVBlank();

        //Update VRAM & remove dead handlers
    	if (menu == 0 && !conversation) {
			int deadVRAMHandlers = 0;
			for (int n = 0; n < vramHandlersL; n++) {
				if (deadVRAMHandlers > 0) {
					vramHandlers[n - deadVRAMHandlers] = vramHandlers[n];
				}
				if (!vramHandlers[n]->Update()) {
					delete vramHandlers[n];
					deadVRAMHandlers++;
				}
			}
			vramHandlersL -= deadVRAMHandlers;
    	}

        //Update Sprites
        UpdateOAM();

        //Swap Screens
        lcdSwap();
        swapScreens = !swapScreens;
    }

    if (!practice) {
    	selectedGame->SaveScore(selectedGame->GetRouteId(route), selectedCharacter->GetId(), stage, score);
    }
    score = 0;
}

void Game::UpdateOAM() {
	memcpy(OAM, mainSprites, SPRITE_COUNT * sizeof(SpriteEntry));
	memcpy(OAM_SUB, subSprites, SPRITE_COUNT * sizeof(SpriteEntry));
}
