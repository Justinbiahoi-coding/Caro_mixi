#ifndef GRAPHIC_VIEW_H
#define GRAPHIC_VIEW_H

#include "raylib.h"
#include "DataStruct.h"

// Draw the chessboard grid and the X and O pieces.
void DrawBoard(const GameState& game);

// Recognize mouse clicks and call the MakeMove function.
void HandleInput(GameState& game);

#endif // GRAPHIC_VIEW_H