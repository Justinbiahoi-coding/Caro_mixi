#ifndef LOGIC_CONTROL_H
#define LOGIC_CONTROL_H

#include "Library.h"

void InitGame(GameState& game, int mode);
bool MakeMove(GameState& game, int row, int col);
bool ScanMine(GameState& game, int centerRow, int centerCol);
int CheckWin(GameState& game, int lastRow, int lastCol);
void ResetRound(GameState& game);
void GetLineStatus(GameState& game, int row, int col, int dx, int dy, int player, int& count, int& blocks);
int EvaluatePosition(GameState& game, int row, int col, int botPlayer);
void BotMove(GameState& game, int botPlayer);
//save game
bool SaveGameSlot(GameState& game, int slot, const char* customName);

// limited slot 
bool LoadGameSlot(GameState& game, int slot);

// delete slot
bool DeleteGameSlot(int slot);

// read file
bool PeekGameSlot(int slot, GameState& tempGame);

#endif // LOGIC_CONTROL_H