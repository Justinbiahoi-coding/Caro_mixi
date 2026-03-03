#ifndef LOGIC_CONTROL_H
#define LOGIC_CONTROL_H

#include "DataStruct.h"

//Create the defaul data
void InitGame(GameState& game, int mode);

// function CheckBoard
// Returns true if the move is valid, false if the square is already occupied.
bool MakeMove(GameState& game, int row, int col);
// The function checks for win/loss/draw based on the latest move at (row, col) 
// Returns: 0 (No winner yet), 1 (P1 wins), 2 (P2 wins), 3 (Draw)
int CheckWin(GameState& game, int lastRow, int lastCol);

#endif // LOGIC_CONTROL_H