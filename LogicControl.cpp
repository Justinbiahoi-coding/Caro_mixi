#include "LogicControl.h"
#include <string.h> 

void InitGame(GameState& game, int mode) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            game.board[i][j].c = 0;
            game.board[i][j].isMine = false;
            game.board[i][j].isScanned = false;
        }
    }

    //create mines for Booming Caro mode
    if (mode == 1) {
        srand((unsigned int)time(NULL));
        int minesToPlace = 30; 
        while (minesToPlace > 0) {
            int r = rand() % BOARD_SIZE;
            int c = rand() % BOARD_SIZE;
            if (!game.board[r][c].isMine) {
                game.board[r][c].isMine = true;
                minesToPlace--;
            }
        }
    }

    game.isPlayer1Turn = true; 
    game.moveCount = 0;
    game.gameMode = mode;
    game.matchStatus = 0; 
    
    game.player1.hp = 2; game.player1.scansLeft = 2; game.player1.stepCount = 0;
    game.player1.winCount = 0; game.player1.loseCount = 0;
    
    game.player2.hp = 2; game.player2.scansLeft = 2; game.player2.stepCount = 0;
    game.player2.winCount = 0; game.player2.loseCount = 0;

    game.roundCount = 1;
    game.cursorRow = BOARD_SIZE / 2; 
    game.cursorCol = BOARD_SIZE / 2;
    game.inputType = 1;
}

void ResetRound(GameState& game) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            game.board[i][j].c = 0;
            game.board[i][j].isMine = false;
            game.board[i][j].isScanned = false;
        }
    }
    
    if (game.gameMode == 1) {
        srand((unsigned int)time(NULL));
        int minesToPlace = 30; 
        while (minesToPlace > 0) {
            int r = rand() % BOARD_SIZE;
            int c = rand() % BOARD_SIZE;
            if (!game.board[r][c].isMine) {
                game.board[r][c].isMine = true;
                minesToPlace--;
            }
        }
    }

    game.isPlayer1Turn = !game.isPlayer1Turn; 
    game.moveCount = 0;
    game.matchStatus = 0; 
    
    game.player1.hp = 2; game.player1.scansLeft = 2; game.player1.stepCount = 0;
    game.player2.hp = 2; game.player2.scansLeft = 2; game.player2.stepCount = 0;

    game.roundCount++; 
    game.cursorRow = BOARD_SIZE / 2; 
    game.cursorCol = BOARD_SIZE / 2;
}

bool MakeMove(GameState& game, int row, int col) {
    if (game.matchStatus != 0) return false;

    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
        if (game.board[row][col].c == 0) {

            if (game.gameMode == 1 && game.board[row][col].isMine) {
                game.board[row][col].isScanned = true; 
                
                if (game.isPlayer1Turn) {
                    game.player1.hp--;
                    if (game.player1.hp <= 0) game.matchStatus = 2; 
                } else {
                    game.player2.hp--;
                    if (game.player2.hp <= 0) game.matchStatus = 1; 
                }
            }

            game.board[row][col].c = game.isPlayer1Turn ? 1 : 2;
            game.moveCount++;
            if (game.isPlayer1Turn) game.player1.stepCount++;
            else game.player2.stepCount++;
            
            if (game.matchStatus == 0) {
                game.matchStatus = CheckWin(game, row, col);
                if (game.matchStatus == 1) { game.player1.winCount++; game.player2.loseCount++; } 
                else if (game.matchStatus == 2) { game.player2.winCount++; game.player1.loseCount++; }
            }

            if (game.matchStatus == 0) {
                game.isPlayer1Turn = !game.isPlayer1Turn;
            }
            return true; 
        }
    }
    return false; 
}

bool ScanMine(GameState& game, int centerRow, int centerCol) {
    if (game.matchStatus != 0 || game.gameMode != 1) return false;

    int& scansLeft = game.isPlayer1Turn ? game.player1.scansLeft : game.player2.scansLeft;
    if (scansLeft <= 0) return false;

    for (int r = centerRow - 1; r <= centerRow + 1; r++) {
        for (int c = centerCol - 1; c <= centerCol + 1; c++) {
            if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                game.board[r][c].isScanned = true;
            }
        }
    }

    scansLeft--; 
    return true;
}

int CheckWin(GameState& game, int lastRow, int lastCol) {
    int player = game.board[lastRow][lastCol].c; 
    if (player == 0) return 0; 
    int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int d = 0; d < 4; d++) {
        int dx = directions[d][0];
        int dy = directions[d][1];
        int count = 1;     
        int blocks = 0;    

        int r = lastRow + dx; int c = lastCol + dy;
        while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && game.board[r][c].c == player) {
            count++; r += dx; c += dy;
        }
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || (game.board[r][c].c != 0 && game.board[r][c].c != player)) blocks++;

        r = lastRow - dx; c = lastCol - dy;
        while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && game.board[r][c].c == player) {
            count++; r -= dx; c -= dy;
        }
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || (game.board[r][c].c != 0 && game.board[r][c].c != player)) blocks++;

        if (count >= 5 && blocks < 2) return player; 
    }
    if (game.moveCount == BOARD_SIZE * BOARD_SIZE) return 3;
    return 0;
}

bool SaveGameSlot(GameState& game, int slot, const char* customName) {
    char filename[30]; snprintf(filename, sizeof(filename), "save_%d.bin", slot);
    strncpy(game.saveName, customName, 29); game.saveName[29] = '\0';
    time_t t = time(NULL); struct tm *tm_info = localtime(&t); strftime(game.saveTime, 30, "%H:%M %d/%m/%Y", tm_info); 
    FILE* file = fopen(filename, "wb"); if (file == NULL) return false; fwrite(&game, sizeof(GameState), 1, file); fclose(file); return true;
}
bool LoadGameSlot(GameState& game, int slot) {
    char filename[30]; snprintf(filename, sizeof(filename), "save_%d.bin", slot);
    FILE* file = fopen(filename, "rb"); if (file == NULL) return false; fread(&game, sizeof(GameState), 1, file); fclose(file); return true;
}
bool DeleteGameSlot(int slot) {
    char filename[30]; snprintf(filename, sizeof(filename), "save_%d.bin", slot); return remove(filename) == 0;
}
bool PeekGameSlot(int slot, GameState& tempGame) {
    char filename[30]; snprintf(filename, sizeof(filename), "save_%d.bin", slot);
    FILE* file = fopen(filename, "rb"); if (file == NULL) return false; fread(&tempGame, sizeof(GameState), 1, file); fclose(file); return true;
}