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
    ui.boardFrame = LoadTexture("assets/board/board_frame.png");
    ui.cell       = LoadTexture("assets/board/cell_custom.png");
    ui.pieceX     = LoadTexture("assets/board/piece_x.png");
    ui.pieceO     = LoadTexture("assets/board/piece_o.png");
    ui.playerBadge = LoadTexture("assets/board/playername_badge.png");
    ui.roundBadge = LoadTexture("assets/board/round_badge.png");
    ui.titleBadge = LoadTexture("assets/board/title_badge.png");

    
    ui.uiBase  = LoadTexture("assets/board/ui_base.png");
    ui.uiHp    = LoadTexture("assets/board/ui_hp.png");
    ui.uiRadar = LoadTexture("assets/board/ui_radar.png");

    ui.mainFont = LoadFontEx("assets/font/Aurusenthial Gothic.ttf", 64, 0, 250);
// 0 co gi --------- 
    // config: frame_count, frame_duration
    // Load hero animations for all 4 heroes
    // Hero 0: black_knight (hidden, kept for future use)
    ui.heroIdle[0]    = LoadCharAnim("assets/Character/black_knight/idle.png", 8, 0.2f);
    ui.heroAttack[0]  = LoadCharAnim("assets/Character/black_knight/attack_s1.png", 8, 0.12f);
    ui.heroAttack2[0] = LoadCharAnim("assets/Character/black_knight/attack_s2.png", 8, 0.12f);
    ui.heroAttack3[0] = LoadCharAnim("assets/Character/black_knight/attack_s2.png", 8, 0.12f); // no s3
    ui.heroDrawSize[0]   = { 900.0f, 900.0f };
    ui.heroDrawOffset[0] = { -300.0f, -250.0f };

    // Hero 1: fire_knight
    ui.heroIdle[1]    = LoadCharAnim("assets/Character/fire_knight/idle.png", 8, 0.1f);
    ui.heroAttack[1]  = LoadCharAnim("assets/Character/fire_knight/attack_s1.png", 11, 0.08f);
    ui.heroAttack2[1] = LoadCharAnim("assets/Character/fire_knight/attack_s2.png",  9, 0.08f);
    ui.heroAttack3[1] = LoadCharAnim("assets/Character/fire_knight/attack_s3.png", 18, 0.07f);
    ui.heroDrawSize[1]   = { 1200.0f, 600.0f };
    ui.heroDrawOffset[1] = { -400.0f, -250.0f };

    // Hero 2: green_archer
    ui.heroIdle[2]    = LoadCharAnim("assets/Character/green_archer/idle.png", 12, 0.08f);
    ui.heroAttack[2]  = LoadCharAnim("assets/Character/green_archer/attack_s1.png", 15, 0.07f);
    ui.heroAttack2[2] = LoadCharAnim("assets/Character/green_archer/attack_s2.png", 12, 0.07f);
    ui.heroAttack3[2] = LoadCharAnim("assets/Character/green_archer/attack_s3.png", 17, 0.06f);
    ui.heroDrawSize[2]   = { 1200.0f, 700.0f };
    ui.heroDrawOffset[2] = { -400.0f, -350.0f };

    // Hero 3: wind_assassin
    ui.heroIdle[3]    = LoadCharAnim("assets/Character/wind_assassin/idle.png", 8, 0.12f);
    ui.heroAttack[3]  = LoadCharAnim("assets/Character/wind_assassin/attack_s1.png",  8, 0.10f);
    ui.heroAttack2[3] = LoadCharAnim("assets/Character/wind_assassin/attack_s2.png", 15, 0.08f);
    ui.heroAttack3[3] = LoadCharAnim("assets/Character/wind_assassin/attack_s3.png", 30, 0.06f);
    ui.heroDrawSize[3]   = { 1200.0f, 700.0f };
    ui.heroDrawOffset[3] = { -400.0f, -350.0f };

    // Hero 4: metal_blade
    ui.heroIdle[4] = LoadCharAnim("assets/Character/metal_blade/idle.png", 8, 0.12f); //animation-time
    ui.heroAttack[4] = LoadCharAnim("assets/Character/metal_blade/attack_s1.png", 6, 0.07f); //animation-time
    ui.heroAttack2[4] = LoadCharAnim("assets/Character/metal_blade/attack_s2.png", 8, 0.12f); //animation-time
    ui.heroAttack3[4] = LoadCharAnim("assets/Character/metal_blade/attack_s3.png", 11, 0.12f); //animation-time
    ui.heroDrawSize[4] = { 1200.0f, 700.0f }; //size-char
    ui.heroDrawOffset[4] = { -400.0f, -350.0f };

    // Hero 5: water_mage
    ui.heroIdle[5] = LoadCharAnim("assets/Character/water_mage/idle.png", 8, 0.14f); //animation-time
    ui.heroAttack[5] = LoadCharAnim("assets/Character/water_mage/attack_s1.png", 7, 0.1f); //animation-time
    ui.heroAttack2[5] = LoadCharAnim("assets/Character/water_mage/attack_s2.png", 21, 0.08f); //animation-time
    ui.heroAttack3[5] = LoadCharAnim("assets/Character/water_mage/attack_s3.png", 32, 0.06f); //animation-time
    ui.heroDrawSize[5] = { 1200.0f, 700.0f }; //size-char
    ui.heroDrawOffset[5] = { -400.0f, -350.0f };
    
    ui.isP1Attacking = false;
    ui.isP2Attacking = false;
    ui.p1AttackVariant = 0;
    ui.p2AttackVariant = 0;
    ui.pendingWin = false;
    ui.pendingWinStatus = 0;
    ui.cellSize = 50.0f; 
    ui.cellStartX = (1920.0f - BOARD_SIZE * ui.cellSize) / 2.0f; 
    ui.cellStartY = (1080.0f - BOARD_SIZE * ui.cellSize) / 2.0f; 

    ui.cellSize = 44.0f; 
    float gridWidth = BOARD_SIZE * 1.0275 * ui.cellSize;  
    float gridHeight = BOARD_SIZE * 1.0275 * ui.cellSize; 
    
    float margin_Left   = 0.073f;  
    float margin_Right  = 0.07f;  
    float margin_Top    = 0.13f;  
    float margin_Bottom = 0.050f; 

    ui.boardFrameRec.width = gridWidth / (1.0f - margin_Left - margin_Right);
    ui.boardFrameRec.height = gridHeight / (1.0f - margin_Top - margin_Bottom);

    ui.boardFrameRec.x = 570.7f;
    ui.boardFrameRec.y = 180.9f;    

    ui.cellStartX = ui.boardFrameRec.x + (ui.boardFrameRec.width * margin_Left);
    ui.cellStartY = ui.boardFrameRec.y + (ui.boardFrameRec.height * margin_Top);

    InitAudioDevice();

    ui.bgMusic = LoadMusicStream("assets/music/bgm.ogg");

    ui.musicVolume = 0.8f;
    ui.musicEnabled = true;
    ui.draggingVolume = false;

    ui.menuScrollY = 350.0f; // vị trí item đầu tiên

    // Init embers — rải ngẫu nhiên toàn màn hình để tránh trống lúc đầu
    for (int i = 0; i < UIState::MAX_EMBERS; i++) {
        ui.embers[i].x     = (float)(rand() % 1920);
        ui.embers[i].y     = (float)(rand() % 1080);
        ui.embers[i].vx    = ((rand() % 100) - 50) * 0.01f;
        ui.embers[i].vy    = -((rand() % 60) + 20) * 0.01f;
        ui.embers[i].alpha = (float)(rand() % 80 + 20) / 100.0f;
        ui.embers[i].size  = (float)(rand() % 4 + 2);
        ui.embers[i].life  = (float)(rand() % 100) / 100.0f;
    }

    // Init char select particles — bắt đầu tắt hết
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
    UnloadTexture(ui.bgSaveLoad);
    UnloadTexture(ui.bgLoadGame);
    UnloadTexture(ui.boardFrame);
    UnloadTexture(ui.cell);
    UnloadTexture(ui.pieceX);
    UnloadTexture(ui.pieceO);
    UnloadTexture(ui.playerBadge);
    UnloadTexture(ui.roundBadge);
    UnloadTexture(ui.titleBadge);

    UnloadTexture(ui.uiBase);
    UnloadTexture(ui.uiHp);
    UnloadTexture(ui.uiRadar);
    
    UnloadTexture(ui.bgGame);
    UnloadFont(ui.mainFont);
    
    // Unload all hero animations
    for (int i = 0; i < 6; i++) {
        UnloadCharAnim(ui.heroIdle[i]);
        UnloadCharAnim(ui.heroAttack[i]);
        UnloadCharAnim(ui.heroAttack2[i]);
        UnloadCharAnim(ui.heroAttack3[i]);
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