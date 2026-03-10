#ifndef GUI_H
#define GUI_H
#define _CRT_SECURE_NO_WARNINGS
#include "Library.h"

// struct for image 
struct UIState {
    int currentScreen; // 0: Menu, 1: Game, 2: Settings, 3: Credits, 5: Load, 6: Save
    int menuSelection;
    int settingSelection;
    int loadSelection;
    int saveSelection;
    
    int endGameSelection; // 0: play again , 1 back to menu

    char nameInput[30];
    int letterCount;
    bool shouldExit;

    char p1NameInput[16]; 
    char p2NameInput[16];
    int p1LetterCount;
    int p2LetterCount;
    int activeInputField; // 0: P1 is entering , 1 P2 is entering 
    //board
    Texture2D boardFrame;
    Texture2D cell;
    Texture2D pieceX;
    Texture2D pieceO;

    //layout
    float cellSize;       
    float cellStartX;     
    float cellStartY;     
    Rectangle boardFrameRec;

    //Textures
    Texture2D bgMenu;
    Texture2D btnNewGame;
    Texture2D btnLoadGame;
    Texture2D bgGame;
    Texture2D btnSettings;
    Texture2D btnHelp;
    Texture2D btnCredits;
    Texture2D btnExit;
    Texture2D bgSettings;
};

// loadpic
void InitGUI(UIState& ui);
void UnloadGUI(UIState& ui);

void UpdateGUI(GameState& game, UIState& ui);

// Draw
void DrawGUI(const GameState& game, const UIState& ui);

#endif // GUI_H