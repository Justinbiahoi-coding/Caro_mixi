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
            
            // goi hahm ktra
            // game.matchStatus = CheckWin(game, row, col);
            
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
    // Tạm thời trả về 0 để game tiếp tục chạy
    return 0;
}