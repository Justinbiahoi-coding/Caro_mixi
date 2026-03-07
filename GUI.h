#ifndef GUI_H
#define GUI_H

#include "Library.h"

// ============================================================
//  UIState
//  Chứa toàn bộ trạng thái giao diện: màn hình hiện tại,
//  lựa chọn menu/setting, input tên người chơi, và tất cả
//  texture (menu + bàn cờ).
// ============================================================
struct UIState {
    // --- Điều hướng màn hình ---
    // 0: Menu  | 1: Game   | 2: Settings | 3: Credits
    // 5: Load  | 6: Save   | 7: New Game (nhập tên)
    int currentScreen;
    int menuSelection;
    int settingSelection;
    int loadSelection;
    int saveSelection;

    // --- Input tên lưu file ---
    char nameInput[30];
    int  letterCount;
    bool shouldExit;

    // --- Input tên người chơi (màn hình 7) ---
    char p1NameInput[16];
    char p2NameInput[16];
    int  p1LetterCount;
    int  p2LetterCount;
    int  activeInputField;  // 0: đang nhập P1 | 1: đang nhập P2

    // --- Texture menu ---

    Texture2D bgMenu;
    Texture2D btnNewGame;
    Texture2D btnLoadGame;
    Texture2D btnSettings;
    Texture2D btnHelp;
    Texture2D btnCredits;
    Texture2D btnExit;

    // --- Texture bàn cờ ---
    Texture2D boardFrame;   // Khung nền bao quanh lưới ô
    Texture2D cell;         // Một ô trống
    Texture2D pieceX;       // Quân X
    Texture2D pieceO;       // Quân O

    // --- Texture Settings ---
    Texture2D bgSettings;
    // --- Layout bàn cờ ---
    // Config (set 1 lần trong InitGUI, không đổi khi chạy):
    //   cellSize, gridX, gridY, framePadding
    // Computed (tự tính qua ComputeBoardLayout):
    //   gridWidth, gridHeight, frameRect
    BoardLayout boardLayout;
};

// --- Vòng đời ---
void InitGUI(UIState& ui);      // Load texture + tính layout
void UnloadGUI(UIState& ui);    // Unload tất cả texture

// --- Vòng lặp chính ---
void UpdateGUI(GameState& game, UIState& ui);   // Xử lý input theo màn hình
void DrawGUI(const GameState& game, const UIState& ui); // Vẽ theo màn hình

#endif // GUI_H