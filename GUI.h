#ifndef GUI_H
#define GUI_H

#include "Library.h"

// struct for image 
struct UIState {
    int currentScreen; // 0: Menu, 1: Game, 2: Settings, 3: Credits, 5: Load, 6: Save
    int menuSelection;
    int settingSelection;
    int loadSelection;
    int saveSelection;
    
    char nameInput[30];
    int letterCount;
    bool shouldExit;

    char p1NameInput[16]; 
    char p2NameInput[16];
    int p1LetterCount;
    int p2LetterCount;
    int activeInputField; // 0: P1 is entering , 1 P2 is entering 
    //Textures
    Texture2D bgMenu;
    Texture2D btnNewGame;
    Texture2D btnLoadGame;
    Texture2D btnSettings;
    Texture2D btnHelp;
    Texture2D btnCredits;
    Texture2D btnExit;
};

// loadpic
void InitGUI(UIState& ui);
void UnloadGUI(UIState& ui);

void UpdateGUI(GameState& game, UIState& ui);

// Draw
void DrawGUI(const GameState& game, const UIState& ui);

#endif // GUI_H