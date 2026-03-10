#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

//define the constant
#define BOARD_SIZE 15 // The chessboard size is 15x15
#define WIN_CONDITION 5

// Structure of 1 square on the chessboard
struct Point {
    int c;          // 0: Empty, 1: X (played), 2:  (not played)
    
    //Booming Caro
    bool isMine;    
    bool isScanned; 
};

// Player structure
struct Player {
    char name[36];
    int winCount;
    int loseCount;
    int stepCount;  // Number of steps taken in the current game
    
    //Booming Caro
    int hp;        
    int scansLeft;
};

// Structure for managing the entire game state
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

    char saveName[30];
    char saveTime[30];
};

#endif // DATA_STRUCT_H