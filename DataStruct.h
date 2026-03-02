#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

//define the constant
#define BOARD_SIZE 15 // Kích thước bàn cờ 15x15 theo ý tưởng của bạn
#define WIN_CONDITION 5 // Cần 5 quân để thắng

// Cấu trúc 1 ô trên bàn cờ
struct Point {
    int c;          // 0: Trống, 1: X đánh, 2: O đánh
    
    // -- Chuẩn bị sẵn cho chế độ Booming Caro --
    bool isMine;    // true: ô này có mìn
    bool isScanned; // true: ô này đã bị scan
};

// Cấu trúc người chơi
struct Player {
    char name[36];
    int winCount;
    int loseCount;
    int stepCount;  // Số bước đã đi trong ván hiện tại
    
    // -- Chuẩn bị sẵn cho chế độ Booming Caro --
    int hp;         // Máu (Mặc định: 3)
    int scansLeft;  // Số lượt scan (Mặc định: 2)
};

// Cấu trúc quản lý trạng thái toàn bộ ván game
struct GameState {
    Point board[BOARD_SIZE][BOARD_SIZE];
    Player player1; // P1 thường đánh X (c = 1)
    Player player2; // P2 thường đánh O (c = 2)
    
    bool isPlayer1Turn; // true: Lượt P1, false: Lượt P2
    int moveCount;      // Tổng số nước đã đi trên bàn cờ
    int gameMode;       // 0: Normal Caro, 1: Booming Caro
    int matchStatus;    // 0: Đang chơi, 1: P1 thắng, 2: P2 thắng, 3: Hòa
};

#endif // DATA_STRUCT_H