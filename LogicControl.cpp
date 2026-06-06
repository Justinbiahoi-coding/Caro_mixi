#include "LogicControl.h"
#include <string.h> 
#include <time.h>   

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

    // Initialize bot 
    game.botPlayer = 2;
    
    // Initialize health and scan turns for 2 players (for Booming mode)
    game.player1.hp = 3;
    game.player1.scansLeft = 2;
    game.player1.stepCount = 0;
    game.player1.winCount = 0; game.player1.loseCount = 0;
    
    game.player2.hp = 3;
    game.player2.scansLeft = 2;
    game.player2.stepCount = 0;
    game.player2.winCount = 0; game.player2.loseCount = 0;

    game.roundCount = 1;
    // keyboard
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
    
    // switch the play move first 
    game.isPlayer1Turn = !game.isPlayer1Turn; 
    
    game.moveCount = 0;
    game.matchStatus = 0; 
    
    game.player1.hp = 3; game.player1.scansLeft = 2; game.player1.stepCount = 0;
    game.player2.hp = 3; game.player2.scansLeft = 2; game.player2.stepCount = 0;

    game.roundCount++; 
    game.cursorRow = BOARD_SIZE / 2; 
    game.cursorCol = BOARD_SIZE / 2;
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
            if (game.matchStatus == 1) {
                game.player1.winCount++;
                game.player2.loseCount++;
            } else if (game.matchStatus == 2) {
                game.player2.winCount++;
                game.player1.loseCount++;
            }

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

void GetLineStatus(GameState& game, int row, int col, int dx, int dy, int player, int& count, int& blocks)
{
    count = 0; // Không bao gồm bản thân ô hiện tại (ô đang xét)
    blocks = 0;

    // Chiều xuôi (+dx, +dy)
    int r = row + dx;
    int c = col + dy;
    while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && game.board[r][c].c == player)
    {
        count++;
        r += dx;
        c += dy;
    }
    // Kiểm tra xem có bị chặn ở đầu này không
    if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || (game.board[r][c].c != 0 && game.board[r][c].c != player))
    {
        blocks++;
    }

    // Chiều ngược (-dx, -dy)
    r = row - dx;
    c = col - dy;
    while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && game.board[r][c].c == player)
    {
        count++;
        r -= dx;
        c -= dy;
    }
    // Kiểm tra xem có bị chặn ở đầu này không
    if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || (game.board[r][c].c != 0 && game.board[r][c].c != player))
    {
        blocks++;
    }
}

int EvaluatePosition(GameState& game, int row, int col)
{
    // Điểm được tính theo: Score[blocks][count]
    // blocks: 0 (mở 2 đầu), 1 (chặn 1 đầu), 2 (chặn 2 đầu)
    // count: số lượng quân liên tiếp (0 đến 5)
    int AttackScore[3][6] = {
        { 0, 9, 54, 162, 1458, 13112 }, // 0 đầu bị chặn
        { 0, 2, 18, 54,  729,  13112 }, // 1 đầu bị chặn
        { 0, 0, 0,  0,   0,    0     }  // 2 đầu bị chặn (Vô dụng)
    };

    int DefenseScore[3][6] = {
        { 0, 3, 27, 99,  729,  6561 },
        { 0, 1, 9,  33,  364,  6561 },
        { 0, 0, 0,  0,   0,    0    }
    };

    int totalScore = 0;

    int directions[4][2] =
    {
        {0,1},
        {1,0},
        {1,1},
        {1,-1}
    };

    for (int i = 0; i < 4; i++)
    {
        int dx = directions[i][0];
        int dy = directions[i][1];

        // ===== ATTACK =====
        int attackCount = 0;
        int attackBlocks = 0;
        GetLineStatus(game, row, col, dx, dy, 2, attackCount, attackBlocks);
        
        if (attackCount > 5) attackCount = 5; // Giới hạn chống tràn mảng
        totalScore += AttackScore[attackBlocks][attackCount];

        // ===== DEFENSE =====
        int defenseCount = 0;
        int defenseBlocks = 0;
        GetLineStatus(game, row, col, dx, dy, 1, defenseCount, defenseBlocks);
        
        if (defenseCount > 5) defenseCount = 5; // Giới hạn chống tràn mảng
        totalScore += DefenseScore[defenseBlocks][defenseCount];
    }

    return totalScore;
}

void BotMove(GameState& game)
{
    int bestScore = -1;
    
    // Khai báo mảng để lưu các nước đi tốt nhất
    struct Move { int r; int c; };
    Move bestMoves[BOARD_SIZE * BOARD_SIZE];
    int bestMoveCount = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (game.board[i][j].c == 0)
            {
                int score = EvaluatePosition(game, i, j);

                if (score > bestScore)
                {
                    bestScore = score;
                    bestMoves[0] = {i, j};
                    bestMoveCount = 1;
                }
                else if (score == bestScore && score > 0)
                {
                    bestMoves[bestMoveCount] = {i, j};
                    bestMoveCount++;
                }
            }
        }
    }

    if (bestMoveCount > 0)
    {
        int randomIndex = GetRandomValue(0, bestMoveCount - 1);
        MakeMove(game, bestMoves[randomIndex].r, bestMoves[randomIndex].c);
    }
}

// slot save 

bool SaveGameSlot(GameState& game, int slot, const char* customName) {
    char filename[30];
    snprintf(filename, sizeof(filename), "save_%d.bin", slot);
    strncpy(game.saveName, customName, 29);
    game.saveName[29] = '\0';

    // get time
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(game.saveTime, 30, "%H:%M %d/%m/%Y", tm_info); 
    FILE* file = fopen(filename, "wb");
    if (file == NULL) return false;
    fwrite(&game, sizeof(GameState), 1, file);
    fclose(file);
    
    return true;
}

bool LoadGameSlot(GameState& game, int slot) {
    char filename[30];
    snprintf(filename, sizeof(filename), "save_%d.bin", slot);
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return false;
    fread(&game, sizeof(GameState), 1, file);
    fclose(file);
    return true;
}

bool DeleteGameSlot(int slot) {
    char filename[30];
    snprintf(filename, sizeof(filename), "save_%d.bin", slot);
    return remove(filename) == 0;
}

bool PeekGameSlot(int slot, GameState& tempGame) {
    char filename[30];
    snprintf(filename, sizeof(filename), "save_%d.bin", slot);
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return false; 
    fread(&tempGame, sizeof(GameState), 1, file);
    fclose(file);
    return true;
}