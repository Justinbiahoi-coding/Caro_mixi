#ifndef GUI_H
#define GUI_H
#define _CRT_SECURE_NO_WARNINGS
#include "Library.h"

struct UIState {
    int currentScreen; 
    int menuSelection;
    int settingSelection;
    int loadSelection;
    int saveSelection;
    
    int endGameSelection; 

    char nameInput[30];
    int letterCount;
    bool shouldExit;

    char p1NameInput[16]; 
    char p2NameInput[16];
    int p1LetterCount;
    int p2LetterCount;
    int activeInputField; 
    
    Texture2D boardFrame;
    Texture2D cell;
    Texture2D pieceX;
    Texture2D pieceO;

    float cellSize;       
    float cellStartX;     
    float cellStartY;     
    Rectangle boardFrameRec;

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

void InitGUI(UIState& ui);
void UnloadGUI(UIState& ui);
void UpdateGUI(GameState& game, UIState& ui);
void DrawGUI(const GameState& game, const UIState& ui);

#endif // GUI_H