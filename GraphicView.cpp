#include "GraphicView.h"
#include "LogicControl.h"

const int CELL_SIZE = 30;
const int OFFSET_X = 50; 
const int OFFSET_Y = 50; 

void DrawBoard(const GameState& game) {
    //Draw the chessboard grid.
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int x = OFFSET_X + j * CELL_SIZE;
            int y = OFFSET_Y + i * CELL_SIZE;

            // Draw a border around the square.
            DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
            if (game.board[i][j].c == 1) {
                // Draw an X using two diagonal lines.
                DrawLine(x + 5, y + 5, x + CELL_SIZE - 5, y + CELL_SIZE - 5, RED);
                DrawLine(x + CELL_SIZE - 5, y + 5, x + 5, y + CELL_SIZE - 5, RED);
            } 
            else if (game.board[i][j].c == 2) {
                // Draw the O 
                DrawCircleLines(x + CELL_SIZE / 2, y + CELL_SIZE / 2, CELL_SIZE / 2 - 4, BLUE);
            }
        }
    }
    if (game.inputType == 1) {
        int cursorX = OFFSET_X + game.cursorCol * CELL_SIZE;
        int cursorY = OFFSET_Y + game.cursorRow * CELL_SIZE;
        DrawRectangleLines(cursorX, cursorY, CELL_SIZE, CELL_SIZE, DARKGREEN);
        DrawRectangle(cursorX + 1, cursorY + 1, CELL_SIZE - 2, CELL_SIZE - 2, Fade(GREEN, 0.3f));
    }
}

void HandleInput(GameState& game) {
    // Chế độ 0: Dùng Chuột
    if (game.inputType == 0) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();

            if (mouseX >= OFFSET_X && mouseX < OFFSET_X + BOARD_SIZE * CELL_SIZE &&
                mouseY >= OFFSET_Y && mouseY < OFFSET_Y + BOARD_SIZE * CELL_SIZE) {
                
                int col = (mouseX - OFFSET_X) / CELL_SIZE;
                int row = (mouseY - OFFSET_Y) / CELL_SIZE;

                MakeMove(game, row, col);
            }
        }
    }
    // Chế độ 1: Dùng Bàn phím (W, A, S, D, Enter)
    else if (game.inputType == 1) {
        // Cập nhật tọa độ con trỏ (có kiểm tra để không vượt ra ngoài bàn cờ)
        if (IsKeyPressed(KEY_W) && game.cursorRow > 0) game.cursorRow--;
        if (IsKeyPressed(KEY_S) && game.cursorRow < BOARD_SIZE - 1) game.cursorRow++;
        if (IsKeyPressed(KEY_A) && game.cursorCol > 0) game.cursorCol--;
        if (IsKeyPressed(KEY_D) && game.cursorCol < BOARD_SIZE - 1) game.cursorCol++;

        // Nhấn Enter để đánh cờ tại vị trí con trỏ hiện tại
        if (IsKeyPressed(KEY_ENTER)) {
            MakeMove(game, game.cursorRow, game.cursorCol);
        }
    }
}