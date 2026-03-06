#include "GraphicView.h"
#include "LogicControl.h"

const int CELL_SIZE = 60;
const int OFFSET_X = 500; 
const int OFFSET_Y = 150; 
const int FRAME_PADDING = 15; // Độ dày của viền khung bàn cờ

void DrawBoard(const GameState& game, const GameAssets& assets, const BoardLayout& layout) {
    // --- 1. VẼ KHUNG BÀN CỜ ---
    Rectangle frameSrc = { 0, 0, (float)assets.boardFrame.width, (float)assets.boardFrame.height };
    Vector2 origin = { 0, 0 };
    
    // Vẽ khung dựa trên thông số layout.frame đã được thiết lập từ trước
    DrawTexturePro(assets.boardFrame, frameSrc, layout.frame, origin, 0.0f, WHITE);


    // --- 2. VẼ LƯỚI Ô CỜ VÀ X/O ---
    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // Tính toán vị trí từng ô dựa trên cellStartX, cellStartY và cellSize
            float x = layout.cellStartX + j * layout.cellSize;
            float y = layout.cellStartY + i * layout.cellSize;
            
            Rectangle cellRect = { x, y, layout.cellSize, layout.cellSize };
            Color cellTint = WHITE; 

            // Xử lý Hover
            if (game.inputType == 0 && CheckCollisionPointRec(mousePos, cellRect) && game.board[i][j].c == 0) {
                cellTint = LIGHTGRAY; 
            } else if (game.inputType == 1 && i == game.cursorRow && j == game.cursorCol) {
                cellTint = LIGHTGRAY; 
            }

            // Vẽ Ô cờ
            Rectangle sourceCell = { 0, 0, (float)assets.cell.width, (float)assets.cell.height };
            DrawTexturePro(assets.cell, sourceCell, cellRect, origin, 0.0f, cellTint);

            // Vẽ X / O
            if (game.board[i][j].c == 1) {
                Rectangle sourceX = { 0, 0, (float)assets.pieceX.width, (float)assets.pieceX.height };
                DrawTexturePro(assets.pieceX, sourceX, cellRect, origin, 0.0f, WHITE);
            } 
            else if (game.board[i][j].c == 2) {
                Rectangle sourceO = { 0, 0, (float)assets.pieceO.width, (float)assets.pieceO.height };
                DrawTexturePro(assets.pieceO, sourceO, cellRect, origin, 0.0f, WHITE);
            }
        }
    }

    // Vẽ con trỏ bàn phím (Nếu có)
    if (game.inputType == 1) {
        int cursorX = OFFSET_X + game.cursorCol * CELL_SIZE;
        int cursorY = OFFSET_Y + game.cursorRow * CELL_SIZE;
        DrawRectangleLines(cursorX, cursorY, CELL_SIZE, CELL_SIZE, DARKGREEN);
    }
}

void HandleInput(GameState& game, const BoardLayout& layout) {    // Chế độ 0: Dùng Chuột
    if (game.inputType == 0) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();

            // Kích thước toàn bộ khu vực lưới cờ
            float gridWidth = BOARD_SIZE * layout.cellSize;
            float gridHeight = BOARD_SIZE * layout.cellSize;

            // Kiểm tra xem chuột có click trúng khu vực lưới ô cờ không
            if (mouseX >= layout.cellStartX && mouseX < layout.cellStartX + gridWidth &&
                mouseY >= layout.cellStartY && mouseY < layout.cellStartY + gridHeight) {
                
                int col = (mouseX - layout.cellStartX) / layout.cellSize;
                int row = (mouseY - layout.cellStartY) / layout.cellSize;

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