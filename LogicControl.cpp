#include "LogicControl.h"

void InitGame(GameState& game, int mode) {
    // Reset the chessboard matrix to 0 (no one has played yet)
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            game.board[i][j].c = 0;
            
            // Set Booming Caro's stats to default.
            game.board[i][j].isMine = false;
            game.board[i][j].isScanned = false;
        }
    }

    // Initialize game state
    game.isPlayer1Turn = true; // X go first
    game.moveCount = 0;
    game.gameMode = mode;
    game.matchStatus = 0; // 0 mean is playing
    
    // Initialize health and scan turns for 2 players (for Booming mode)
    game.player1.hp = 3;
    game.player1.scansLeft = 2;
    game.player1.stepCount = 0;
    
    game.player2.hp = 3;
    game.player2.scansLeft = 2;
    game.player2.stepCount = 0;

    // keyboard
    game.cursorRow = BOARD_SIZE / 2; 
    game.cursorCol = BOARD_SIZE / 2;
    game.inputType = 1;
}

bool MakeMove(GameState& game, int row, int col) {
    // If the game is over, no more fighting is allowed.
    if (game.matchStatus != 0) return false;

    // Check if the coordinates are within the chessboard and if that cell is empty (c == 0).
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
        if (game.board[row][col].c == 0) {

            // Mark 1 (X) for P1, 2 (O) for P2
            game.board[row][col].c = game.isPlayer1Turn ? 1 : 2;
            game.moveCount++;
            if (game.isPlayer1Turn) game.player1.stepCount++;
            else game.player2.stepCount++;
            
            // check
            game.matchStatus = CheckWin(game, row, col);
            
            // Switch turns if the game is still ongoing
            if (game.matchStatus == 0) {
                game.isPlayer1Turn = !game.isPlayer1Turn;
            }
            
            return true; // Returns true to indicate successful entry
        }
    }
    return false; // Invalid coordinates or the cell is already occupied
}

int CheckWin(GameState& game, int lastRow, int lastCol) {
    int player = game.board[lastRow][lastCol].c; 
    if (player == 0) return 0; 
    // Horizontal (0, 1), Vertical (1, 0), main diagonal (1, 1), secondary diagonal (1, -1)
    int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int d = 0; d < 4; d++) {
        int dx = directions[d][0];
        int dy = directions[d][1];
        
        int count = 1;     // Count the number of consecutive troops
        int blocks = 0;    // Count the number of blocked entries

        //(+dx, +dy)
        int r = lastRow + dx;
        int c = lastCol + dy;
        while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && game.board[r][c].c == player) {
            count++;
            r += dx;
            c += dy;
        }
        // Check if blocked (touching the boundary or hitting the opponent's flag).
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || (game.board[r][c].c != 0 && game.board[r][c].c != player)) {
            blocks++;
        }

        //(-dx, -dy)
        r = lastRow - dx;
        c = lastCol - dy;
        while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && game.board[r][c].c == player) {
            count++;
            r -= dx;
            c -= dy;
        }
        //Check if blocked
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || (game.board[r][c].c != 0 && game.board[r][c].c != player)) {
            blocks++;
        }

        //Win if you have 5 pieces and are not blocked at both ends
        if (count >= 5 && blocks < 2) {
            return player; 
        }
    }

    // Check for a Tie (if all 225 squares have been played and no one has won).
    if (game.moveCount == BOARD_SIZE * BOARD_SIZE) {
        return 3;
    }
    return 0;
}
// Thêm 2 hàm này vào cuối file LogicControl.cpp

bool SaveGame(const GameState& game, const char* filename) {
    // Mở file để ghi dưới dạng nhị phân (wb)
    FILE* file = fopen(filename, "wb");
    if (file == NULL) return false; // Không tạo được file
    
    // Ghi toàn bộ khối bộ nhớ của biến game xuống file
    fwrite(&game, sizeof(GameState), 1, file);
    
    fclose(file);
    return true;
}

bool LoadGame(GameState& game, const char* filename) {
    // Mở file để đọc dưới dạng nhị phân (rb)
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return false; // File không tồn tại
    
    // Đọc khối dữ liệu từ file đè thẳng vào biến game
    fread(&game, sizeof(GameState), 1, file);
    
    fclose(file);
    return true;
}