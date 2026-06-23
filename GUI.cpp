#include "GUI.h"
#include "GUI_Menu.h"
#include "GUI_Game.h"
#include "LogicControl.h"

void InitGUI(UIState& ui) {
    ui.currentScreen = 0;
    ui.menuSelection = 0;
    ui.settingSelection = 0;
    ui.loadSelection = 0;
    ui.saveSelection = 0;
    ui.endGameSelection = 0; 
    ui.nameInput[0] = '\0';
    ui.letterCount = 0;
    ui.shouldExit = false;
    
    ui.p1NameInput[0] = '\0';
    ui.p2NameInput[0] = '\0';
    ui.p1LetterCount = 0;
    ui.p2LetterCount = 0;
    ui.activeInputField = 0;
    
    HideCursor(); 
    ui.cursorNormal = LoadTexture("assets/cursor/GothicCursor-1.png.png");
    ui.cursorClick  = LoadTexture("assets/cursor/GloveCursor-1.png.png");
    ui.bgMenu      = LoadTexture("assets/menu/background.png");
    ui.btnNewGame  = LoadTexture("assets/menu/NewGame.png");
    ui.btnLoadGame = LoadTexture("assets/menu/LoadGame.png");
    ui.btnSettings = LoadTexture("assets/menu/Settings.png");
    ui.btnHelp     = LoadTexture("assets/menu/Help.png");
    ui.btnCredits  = LoadTexture("assets/menu/Credits.png");
    ui.btnExit     = LoadTexture("assets/menu/Exit.png");
    ui.bgSettings  = LoadTexture("assets/menu/bgSettings.png");
    ui.bgSaveLoad  = LoadTexture("assets/menu/bg_saveload.png");
    ui.bgLoadGame = LoadTexture("assets/menu/loadbg.png");
    ui.bgGame      = LoadTexture("assets/board/bg_game.png"); 
    ui.bgSelect = LoadTexture("assets/menu/bg_select.png");
    ui.helpBg     = LoadTexture("assets/menu/helpbg.jpg");
    ui.helpScrollTimer = 0.0f;
    ui.boardFrame = LoadTexture("assets/board/board_frame.png");
    ui.cell       = LoadTexture("assets/board/cell_custom.png");
    ui.playerBadge = LoadTexture("assets/board/playername_badge.png");
    ui.roundBadge = LoadTexture("assets/board/round_badge.png");

    ui.mainFont = LoadFontEx("assets/font/PixelSerif_16px_v02.ttf", 64, 0, 250);

    // Load hero animations. Args: frame_count, frame_duration
    // Hero 0: black_knight (hidden)
    ui.heroIdle[0]    = LoadCharAnim("assets/Character/black_knight/idle.png", 8, 0.2f);
    ui.heroAttack[0]  = LoadCharAnim("assets/Character/black_knight/attack_s1.png", 8, 0.12f);
    ui.heroAttack2[0] = LoadCharAnim("assets/Character/black_knight/attack_s2.png", 8, 0.12f);
    ui.heroAttack3[0] = LoadCharAnim("assets/Character/black_knight/attack_s2.png", 8, 0.12f);
    ui.heroDeath[0]   = LoadCharAnim("assets/Character/black_knight/idle.png", 8, 0.12f);
    ui.heroDrawSize[0]   = { 900.0f, 900.0f };
    ui.heroDrawOffset[0] = { -300.0f, -250.0f };

    // Hero 1: fire_knight
    ui.heroIdle[1]    = LoadCharAnim("assets/Character/fire_knight/idle.png",      8,  0.1f);
    ui.heroAttack[1]  = LoadCharAnim("assets/Character/fire_knight/attack_s1.png", 11, 0.08f);
    ui.heroAttack2[1] = LoadCharAnim("assets/Character/fire_knight/attack_s2.png", 9,  0.08f);
    ui.heroAttack3[1] = LoadCharAnim("assets/Character/fire_knight/attack_s3.png", 18, 0.07f);
    ui.heroDeath[1]   = LoadCharAnim("assets/Character/fire_knight/death.png",     13, 0.12f);
    ui.heroDrawSize[1]   = { 1200.0f, 600.0f };
    ui.heroDrawOffset[1] = { -400.0f, -250.0f };

    // Hero 2: green_archer
    ui.heroIdle[2]    = LoadCharAnim("assets/Character/green_archer/idle.png",      12, 0.08f);
    ui.heroAttack[2]  = LoadCharAnim("assets/Character/green_archer/attack_s1.png", 15, 0.07f);
    ui.heroAttack2[2] = LoadCharAnim("assets/Character/green_archer/attack_s2.png", 12, 0.07f);
    ui.heroAttack3[2] = LoadCharAnim("assets/Character/green_archer/attack_s3.png", 17, 0.06f);
    ui.heroDeath[2]   = LoadCharAnim("assets/Character/green_archer/death.png",     19, 0.15f);
    ui.heroDrawSize[2]   = { 1200.0f, 700.0f };
    ui.heroDrawOffset[2] = { -400.0f, -350.0f };

    // Hero 3: wind_assassin
    ui.heroIdle[3]    = LoadCharAnim("assets/Character/wind_assassin/idle.png",      8,  0.12f);
    ui.heroAttack[3]  = LoadCharAnim("assets/Character/wind_assassin/attack_s1.png", 8,  0.10f);
    ui.heroAttack2[3] = LoadCharAnim("assets/Character/wind_assassin/attack_s2.png", 15, 0.08f);
    ui.heroAttack3[3] = LoadCharAnim("assets/Character/wind_assassin/attack_s3.png", 30, 0.06f);
    ui.heroDeath[3]   = LoadCharAnim("assets/Character/wind_assassin/death.png",     19, 0.15f);
    ui.heroDrawSize[3]   = { 1200.0f, 700.0f };
    ui.heroDrawOffset[3] = { -400.0f, -350.0f };

    // Hero 4: metal_blade
    ui.heroIdle[4]    = LoadCharAnim("assets/Character/metal_blade/idle.png",      8,  0.12f);
    ui.heroAttack[4]  = LoadCharAnim("assets/Character/metal_blade/attack_s1.png", 6,  0.07f);
    ui.heroAttack2[4] = LoadCharAnim("assets/Character/metal_blade/attack_s2.png", 8,  0.12f);
    ui.heroAttack3[4] = LoadCharAnim("assets/Character/metal_blade/attack_s3.png", 11, 0.12f);
    ui.heroDeath[4]   = LoadCharAnim("assets/Character/metal_blade/death.png",     12, 0.12f);
    ui.heroDrawSize[4] = { 1200.0f, 700.0f };
    ui.heroDrawOffset[4] = { -400.0f, -350.0f };

    // Hero 5: water_mage
    ui.heroIdle[5]    = LoadCharAnim("assets/Character/water_mage/idle.png",      8,  0.14f);
    ui.heroAttack[5]  = LoadCharAnim("assets/Character/water_mage/attack_s1.png", 7,  0.1f);
    ui.heroAttack2[5] = LoadCharAnim("assets/Character/water_mage/attack_s2.png", 21, 0.08f);
    ui.heroAttack3[5] = LoadCharAnim("assets/Character/water_mage/attack_s3.png", 32, 0.06f);
    ui.heroDeath[5]   = LoadCharAnim("assets/Character/water_mage/death.png",     16, 0.15f);
    ui.heroDrawSize[5] = { 1200.0f, 700.0f };
    ui.heroDrawOffset[5] = { -400.0f, -350.0f };

    // Load attack effects (sprite sheet shown when placing a piece)
    // black_knight has no effect, use its icon instead
    ui.heroEffect[0] = LoadTexture("assets/Character/black_knight/attack_icon.png");
    ui.heroEffectFrames[0] = 1;
    ui.heroEffect[1] = LoadTexture("assets/Character/fire_knight/attack_effect.png");
    ui.heroEffectFrames[1] = 7;
    ui.heroEffect[2] = LoadTexture("assets/Character/green_archer/attack_effect.png");
    ui.heroEffectFrames[2] = 10;
    ui.heroEffect[3] = LoadTexture("assets/Character/wind_assassin/attack_effect.png");
    ui.heroEffectFrames[3] = 12;
    ui.heroEffect[4] = LoadTexture("assets/Character/metal_blade/attack_effect.png");
    ui.heroEffectFrames[4] = 11;
    ui.heroEffect[5] = LoadTexture("assets/Character/water_mage/attack_effect.png");
    ui.heroEffectFrames[5] = 10;

    ui.cellEffectCount = 0;

    // Load attack icons (used as board pieces)
    ui.heroIcon[0] = LoadTexture("assets/Character/black_knight/attack_icon.png");
    ui.heroIcon[1] = LoadTexture("assets/Character/fire_knight/attack_icon.png");
    ui.heroIcon[2] = LoadTexture("assets/Character/green_archer/attack_icon.png");
    ui.heroIcon[3] = LoadTexture("assets/Character/wind_assassin/attack_icon.png");
    ui.heroIcon[4] = LoadTexture("assets/Character/metal_blade/attack_icon.png");
    ui.heroIcon[5] = LoadTexture("assets/Character/water_mage/attack_icon.png");

    ui.isP1Attacking = false;
    ui.isP2Attacking = false;
    ui.p1AttackVariant = 0;
    ui.p2AttackVariant = 0;
    ui.pendingWin = false;
    ui.pendingWinStatus = 0;
    ui.isP1Dying = false;
    ui.isP2Dying = false;
    ui.isP1Dead  = false;
    ui.isP2Dead  = false;
    ui.winScreenTimer = 0.0f;
    ui.cellSize = 50.0f; 
    ui.cellStartX = (1920.0f - BOARD_SIZE * ui.cellSize) / 2.0f; 
    ui.cellStartY = (1080.0f - BOARD_SIZE * ui.cellSize) / 2.0f; 

    // Square frame, leaving room for the heroes on both sides and badges top/bottom
    float frameW = 780.0f;
    float frameH = 780.0f;

    ui.boardFrameRec.width  = frameW;
    ui.boardFrameRec.height = frameH;
    ui.boardFrameRec.x = (1920.0f - frameW) / 2.0f;          // center horizontally
    ui.boardFrameRec.y = (1080.0f - frameH) / 2.0f - 20.0f;  // shift up for the larger bottom badge

    // Frame border is ~5.5% on every side
    float margin_L = 0.055f, margin_R = 0.055f;
    float margin_T = 0.055f;

    ui.cellSize   = frameW * (1.0f - margin_L - margin_R) / BOARD_SIZE;
    ui.cellStartX = ui.boardFrameRec.x + frameW * margin_L;
    ui.cellStartY = ui.boardFrameRec.y + frameH * margin_T;

    InitAudioDevice();

    ui.bgMusic = LoadMusicStream("assets/music/bgm.mp3");

    // Load attack sound effects per hero, MUST come after InitAudioDevice()
    // because LoadSoundFromWave needs the audio device ready to build the buffer
    for (int v = 0; v < 3; v++) ui.heroAttackSound[0][v] = Sound{}; // black_knight has no sound files
    {
        // Asset index 1-5; order MUST match HERO_MAP
        const char* heroDirs[5] = {
            "fire_knight", "green_archer", "wind_assassin", "metal_blade", "water_mage"
        };
        // One sound per attack variant: 0=normal(s1), 1=block(s2), 2=win(s3)
        const char* variantFile[3] = {
            "attack_sound.wav", "attack_sound_s2.wav", "attack_sound_s3.wav"
        };
        for (int i = 0; i < 5; i++) {
            for (int v = 0; v < 3; v++) {
                char path[160];
                snprintf(path, sizeof(path), "assets/Character/%s/%s", heroDirs[i], variantFile[v]);
                Wave w = LoadWave(path);
                WaveFormat(&w, 44100, 32, 2); // normalize to 32-bit float stereo
                ui.heroAttackSound[i + 1][v] = LoadSoundFromWave(w);
                UnloadWave(w);
            }
        }
    }

    ui.musicVolume = 0.8f;
    ui.musicEnabled = true;
    ui.draggingVolume = false;

    ui.sfxVolume = 0.8f;
    ui.draggingSFX = false;
    ApplySFXVolume(ui); // push the initial SFX volume onto every loaded sound

    ui.menuScrollY = 350.0f; // position of the first menu item

    // Init embers, scattered across the screen so it isn't empty at start
    for (int i = 0; i < UIState::MAX_EMBERS; i++) {
        ui.embers[i].x     = (float)(rand() % 1920);
        ui.embers[i].y     = (float)(rand() % 1080);
        ui.embers[i].vx    = ((rand() % 100) - 50) * 0.01f;
        ui.embers[i].vy    = -((rand() % 60) + 20) * 0.01f;
        ui.embers[i].alpha = (float)(rand() % 80 + 20) / 100.0f;
        ui.embers[i].size  = (float)(rand() % 4 + 2);
        ui.embers[i].life  = (float)(rand() % 100) / 100.0f;
    }

    // Init char select particles, all off at start
    for (int i = 0; i < UIState::MAX_CHAR_PARTICLES; i++) {
        ui.charParticles[i].life = 0.0f;
        ui.charParticles[i].alpha = 0.0f;
    }

    PlayMusicStream(ui.bgMusic);
    SetMusicVolume(ui.bgMusic, ui.musicVolume);
}

void UnloadGUI(UIState& ui) {
    UnloadTexture(ui.cursorNormal);
    UnloadTexture(ui.cursorClick);
    UnloadTexture(ui.bgMenu);
    UnloadTexture(ui.btnNewGame);
    UnloadTexture(ui.btnLoadGame);
    UnloadTexture(ui.btnSettings);
    UnloadTexture(ui.btnHelp);
    UnloadTexture(ui.btnCredits);
    UnloadTexture(ui.btnExit);
    UnloadTexture(ui.bgSettings);
    UnloadTexture(ui.bgSaveLoad);
    UnloadTexture(ui.bgLoadGame);
    UnloadTexture(ui.bgSelect);
    UnloadTexture(ui.boardFrame);
    UnloadTexture(ui.cell);
    UnloadTexture(ui.playerBadge);
    UnloadTexture(ui.roundBadge);

    UnloadTexture(ui.bgGame);
    UnloadTexture(ui.helpBg);
    UnloadFont(ui.mainFont);

    // Unload all hero animations, icons and effects
    for (int i = 0; i < 6; i++) {
        UnloadCharAnim(ui.heroIdle[i]);
        UnloadCharAnim(ui.heroAttack[i]);
        UnloadCharAnim(ui.heroAttack2[i]);
        UnloadCharAnim(ui.heroAttack3[i]);
        UnloadCharAnim(ui.heroDeath[i]);
        UnloadTexture(ui.heroIcon[i]);
        UnloadTexture(ui.heroEffect[i]);
        if (i != 0) for (int v = 0; v < 3; v++) UnloadSound(ui.heroAttackSound[i][v]); // index 0 is never loaded
    }
    
    UnloadMusicStream(ui.bgMusic);
    CloseAudioDevice();
}

void UpdateGUI(GameState& game, UIState& ui) {
    UpdateMusicStream(ui.bgMusic);
    if (ui.currentScreen == 1) {
        UpdateGUIGame(game, ui);
    } else {
        UpdateMenuScreens(game, ui);
    }

}

void DrawGUI(const GameState& game, const UIState& ui) {
    if (ui.currentScreen == 1) {
        DrawGUIGame(game, ui);
    } else {
        DrawMenuScreens(game, ui);
    }
    Vector2 mouse = GetMousePosition();
    
    Texture2D activeCursor = IsMouseButtonDown(MOUSE_LEFT_BUTTON) ? ui.cursorClick : ui.cursorNormal;
    float cursorScale = 0.2f;
    DrawTextureEx(activeCursor, mouse, 0.0f, cursorScale, WHITE);
}