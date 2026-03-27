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
    
    ui.bgMenu      = LoadTexture("assets/menu/background.jpg");
    ui.btnNewGame  = LoadTexture("assets/menu/NewGame.png");
    ui.btnLoadGame = LoadTexture("assets/menu/LoadGame.png");
    ui.btnSettings = LoadTexture("assets/menu/Settings.png");
    ui.btnHelp     = LoadTexture("assets/menu/Help.png");
    ui.btnCredits  = LoadTexture("assets/menu/Credits.png");
    ui.btnExit     = LoadTexture("assets/menu/Exit.png");
    ui.bgSettings  = LoadTexture("assets/menu/bgSettings.png");
    
    ui.bgGame      = LoadTexture("assets/board/bg_game.png"); 
    
    ui.boardFrame = LoadTexture("assets/board/board_frame.png");
    ui.cell       = LoadTexture("assets/board/cell_custom.png");
    ui.pieceX     = LoadTexture("assets/board/piece_x.png");
    ui.pieceO     = LoadTexture("assets/board/piece_o.png");

    ui.mainFont = LoadFontEx("assets/font/Aurusenthial Gothic.ttf", 64, 0, 250);

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

    ui.boardFrameRec.x = (1920.0f - ui.boardFrameRec.width) / 2.0f;
    ui.boardFrameRec.y = (1080.0f - ui.boardFrameRec.height) / 2.0f + 30.0f; 

    ui.cellStartX = ui.boardFrameRec.x + (ui.boardFrameRec.width * margin_Left);
    ui.cellStartY = ui.boardFrameRec.y + (ui.boardFrameRec.height * margin_Top);
}

void UnloadGUI(UIState& ui) {
    UnloadTexture(ui.bgMenu);
    UnloadTexture(ui.btnNewGame);
    UnloadTexture(ui.btnLoadGame);
    UnloadTexture(ui.btnSettings);
    UnloadTexture(ui.btnHelp);
    UnloadTexture(ui.btnCredits);
    UnloadTexture(ui.btnExit);
    UnloadTexture(ui.boardFrame);
    UnloadTexture(ui.cell);
    UnloadTexture(ui.pieceX);
    UnloadTexture(ui.pieceO);
    UnloadTexture(ui.bgGame);
    UnloadFont(ui.mainFont);
}

void UpdateGUI(GameState& game, UIState& ui) {
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
}