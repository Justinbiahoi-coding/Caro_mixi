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

    
    ui.hp0     = LoadTexture("assets/board/0.png");
    ui.hp50    = LoadTexture("assets/board/50.png");
    ui.hp100   = LoadTexture("assets/board/100.png");
    ui.hpFrame = LoadTexture("assets/board/Frame.png");
    ui.uiRadar = LoadTexture("assets/board/ui_radar.png");

    ui.mainFont = LoadFontEx("assets/font/Aurusenthial Gothic.ttf", 64, 0, 250);

    ui.charP1 = LoadCharAnim("assets/Character/black_knight/idle.png", 6, 0.12f);
    ui.atkP1 = LoadCharAnim("assets/Character/black_knight/attack.png", 22, 0.03f);
    ui.atkP2 = LoadCharAnim("assets/Character/fire_knight/attack.png", 6, 0.08f);
    ui.charP2 = LoadCharAnim("assets/Character/fire_knight/idle.png", 4, 0.12f);
    ui.isP1Attacking = false;
    ui.isP2Attacking = false;
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

    UnloadTexture(ui.hp0);
    UnloadTexture(ui.hp50);
    UnloadTexture(ui.hp100);
    UnloadTexture(ui.hpFrame);
    UnloadTexture(ui.uiRadar);
    
    UnloadTexture(ui.bgGame);
    UnloadFont(ui.mainFont);
    UnloadCharAnim(ui.charP1);
    UnloadCharAnim(ui.charP2);
    UnloadCharAnim(ui.atkP1);
    UnloadCharAnim(ui.atkP2);
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