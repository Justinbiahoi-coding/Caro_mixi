#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include "raylib.h"

// ============================================================
//  CONSTANTS
// ============================================================
#define BOARD_SIZE      15
#define WIN_CONDITION    5

// ============================================================
//  GAME DATA STRUCTURES
// ============================================================

struct Point {
    int  c;           // 0: Empty | 1: X (P1) | 2: O (P2)
    bool isMine;
    bool isScanned;
};

struct Player {
    char name[36];
    int  winCount;
    int  loseCount;
    int  stepCount;
    int  hp;
    int  scansLeft;
};

struct GameState {
    Point board[BOARD_SIZE][BOARD_SIZE];
    Player player1; // c = 1
    Player player2; // c = 2
    
    bool isPlayer1Turn; // true: P1, false: P2
    int moveCount;      // Total number of moves made on the chessboard
    int roundCount;
    int gameMode;       // 0: Normal Caro, 1: Booming Caro
    int matchStatus;    // 0: In progress, 1: P1 wins, 2: P2 wins, 3: Draw
    int cursorRow;  // Y coordinate of the keyboard cursor
    int cursorCol;  // X coordinate of the keyboard cursor
    int inputType;  //0: Play with Mouse, 1: Play with Keyboard
    Point  board[BOARD_SIZE][BOARD_SIZE];
    Player player1;       // c = 1
    Player player2;       // c = 2

    bool isPlayer1Turn;   // true: P1's turn, false: P2's turn
    int  moveCount;       // Total moves made this game
    int  gameMode;        // 0: Normal Caro | 1: Booming Caro
    int  matchStatus;     // 0: In progress | 1: P1 wins | 2: P2 wins | 3: Draw
    int  cursorRow;       // Keyboard cursor – row
    int  cursorCol;       // Keyboard cursor – col
    int  inputType;       // 0: Mouse | 1: Keyboard (WASD + Enter)

    int winStartRow;
    int winStartCol;
    int winEndRow;
    int winEndCol;
    char saveName[30];
    char saveTime[30];
};

struct GameAssets {
    Texture2D boardFrame;
    Texture2D cell;
    Texture2D pieceX;
    Texture2D pieceO;
};

// ============================================================
//  BOARD LAYOUT
//
//  Workflow:
//    1. Fill in the "--- Config (set by you) ---" fields.
//    2. Call ComputeBoardLayout() once after setting them.
//    3. Use the "--- Computed (read-only) ---" fields everywhere else.
//
//  Relationships (always consistent after Compute):
//
//    gridWidth  = BOARD_SIZE * cellSize
//    gridHeight = BOARD_SIZE * cellSize
//
//    frameRect  = { gridX - framePadding,
//                   gridY - framePadding,
//                   gridWidth  + framePadding * 2,
//                   gridHeight + framePadding * 2 }
// ============================================================

struct BoardLayout {
    // --- Config (set by you) ---
    float cellSize;       // Size of one cell in pixels  (e.g. 50)
    float gridX;          // Top-left X of the grid itself (cells only, no frame)
    float gridY;          // Top-left Y of the grid itself
    float framePadding;   // Extra space between the grid edge and the frame border

    // --- Computed (read-only after ComputeBoardLayout) ---
    float     gridWidth;  // BOARD_SIZE * cellSize
    float     gridHeight; // BOARD_SIZE * cellSize
    Rectangle frameRect;  // Position + size of the background frame texture
};

// Call once after setting the four config fields above.
inline void ComputeBoardLayout(BoardLayout& layout) {
    layout.gridWidth  = BOARD_SIZE * layout.cellSize;
    layout.gridHeight = BOARD_SIZE * layout.cellSize;

    layout.frameRect = {
        layout.gridX      - layout.framePadding,
        layout.gridY      - layout.framePadding,
        layout.gridWidth  + layout.framePadding * 2.0f,
        layout.gridHeight + layout.framePadding * 2.0f
    };
}

#endif // DATA_STRUCT_H