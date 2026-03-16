#include "GUI_Game.h"
#include "LogicControl.h"

void UpdateGUIGame(GameState& game, UIState& ui) {
    Vector2 mouse = GetMousePosition();

    if (game.matchStatus == 0) { 
        if (game.inputType == 0) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                float gridWidth = BOARD_SIZE * ui.cellSize;
                float gridHeight = BOARD_SIZE * ui.cellSize;

                if (mouse.x >= ui.cellStartX && mouse.x < ui.cellStartX + gridWidth &&
                    mouse.y >= ui.cellStartY && mouse.y < ui.cellStartY + gridHeight) {
                    
                    int col = (mouse.x - ui.cellStartX) / ui.cellSize;
                    int row = (mouse.y - ui.cellStartY) / ui.cellSize;
                    MakeMove(game, row, col);
                }
            }
        } else if (game.inputType == 1) {
            if (IsKeyPressed(KEY_W) && game.cursorRow > 0) game.cursorRow--;
            if (IsKeyPressed(KEY_S) && game.cursorRow < BOARD_SIZE - 1) game.cursorRow++;
            if (IsKeyPressed(KEY_A) && game.cursorCol > 0) game.cursorCol--;
            if (IsKeyPressed(KEY_D) && game.cursorCol < BOARD_SIZE - 1) game.cursorCol++;
            if (IsKeyPressed(KEY_ENTER)) MakeMove(game, game.cursorRow, game.cursorCol);
        }
    } 
    else { 
        Vector2 mouse = GetMousePosition();
        
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) ui.endGameSelection = 0;
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) ui.endGameSelection = 1;

        Rectangle btnPlayAgain = { 720, 560, 220, 50 };
        Rectangle btnMenu = { 980, 560, 220, 50 };

        if (CheckCollisionPointRec(mouse, btnPlayAgain)) ui.endGameSelection = 0;
        if (CheckCollisionPointRec(mouse, btnMenu)) ui.endGameSelection = 1;

        bool confirm = false;
        if (IsKeyPressed(KEY_ENTER)) confirm = true;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && 
           (CheckCollisionPointRec(mouse, btnPlayAgain) || CheckCollisionPointRec(mouse, btnMenu))) {
            confirm = true;
        }

        if (confirm) {
            if (ui.endGameSelection == 0) {
                ResetRound(game); 
            } else {
                ui.currentScreen = 0; 
            }
        }
    }

    if (IsKeyPressed(KEY_M)) ui.currentScreen = 0; 
    if (IsKeyPressed(KEY_L)) {
        ui.currentScreen = 6; 
        ui.nameInput[0] = '\0'; 
        ui.letterCount = 0;
        ui.saveSelection = 0;
    }
}

void DrawGUIGame(const GameState& game, const UIState& ui) {
    DrawTexturePro(ui.bgGame, { 0, 0, (float)ui.bgGame.width, (float)ui.bgGame.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
    Rectangle frameSrc = { 0, 0, (float)ui.boardFrame.width, (float)ui.boardFrame.height };
    DrawTexturePro(ui.boardFrame, frameSrc, ui.boardFrameRec, {0, 0}, 0.0f, WHITE);

    Vector2 mousePos = GetMousePosition();
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            float x = ui.cellStartX + j * ui.cellSize;
            float y = ui.cellStartY + i * ui.cellSize;
            Rectangle cellRect = { x, y, ui.cellSize, ui.cellSize };
            Color cellTint = WHITE; 

            if (game.inputType == 0 && CheckCollisionPointRec(mousePos, cellRect) && game.board[i][j].c == 0) {
                cellTint = LIGHTGRAY; 
            } else if (game.inputType == 1 && i == game.cursorRow && j == game.cursorCol) {
                cellTint = LIGHTGRAY; 
            }

            Rectangle sourceCell = { 0, 0, (float)ui.cell.width, (float)ui.cell.height };
            DrawTexturePro(ui.cell, sourceCell, cellRect, {0, 0}, 0.0f, cellTint);

            if (game.board[i][j].c == 1) {
                Rectangle sourceX = { 0, 0, (float)ui.pieceX.width, (float)ui.pieceX.height };
                DrawTexturePro(ui.pieceX, sourceX, cellRect, {0, 0}, 0.0f, WHITE);
            } 
            else if (game.board[i][j].c == 2) {
                Rectangle sourceO = { 0, 0, (float)ui.pieceO.width, (float)ui.pieceO.height };
                DrawTexturePro(ui.pieceO, sourceO, cellRect, {0, 0}, 0.0f, WHITE);
            }
        }
    }
    
    if (game.inputType == 1) {
        float cx = ui.cellStartX + game.cursorCol * ui.cellSize;
        float cy = ui.cellStartY + game.cursorRow * ui.cellSize;
        DrawRectangleLinesEx({cx, cy, ui.cellSize, ui.cellSize}, 3.0f, DARKGREEN);
    }
    DrawText("THONG TIN VAN DAU", 820, 35, 30, BLACK);
    DrawText(TextFormat("--- VONG CHOI %d ---", game.roundCount), 840, 70, 25, DARKBLUE);
    if (game.matchStatus == 0) {
        if (game.isPlayer1Turn) DrawText(TextFormat(">> LUOT CUA: %s (X) <<", game.player1.name), 790, 100, 30, RED);
        else DrawText(TextFormat(">> LUOT CUA: %s (O) <<", game.player2.name), 790, 100, 30, BLUE);
    }

    if (game.inputType == 0) DrawText("Dieu khien: Chuot", 880, 140, 20, GRAY);
    else DrawText("Dieu khien: WASD + Enter", 850, 140, 20, GRAY);
    
    DrawText("Bam [L] de luu game | Nhan [M] de ve Menu", 720, 980, 25, DARKGRAY);

    int p1X = 150; 
    int p1Y = 350;
    DrawText(game.player1.name, p1X, p1Y, 50, RED);
    DrawText("Phe: X", p1X, p1Y + 70, 30, DARKGRAY);
    DrawText(TextFormat("Thang: %d | Thua: %d", game.player1.winCount, game.player1.loseCount), p1X, p1Y + 120, 30, BLACK);
    DrawText(TextFormat("So buoc van nay: %d", game.player1.stepCount), p1X, p1Y + 170, 25, GRAY);
    
    if (game.gameMode == 1) {
        DrawText(TextFormat("HP: %d / 3", game.player1.hp), p1X, p1Y + 220, 30, MAROON);
        DrawText(TextFormat("Scan: %d", game.player1.scansLeft), p1X, p1Y + 270, 30, ORANGE);
    }

    int p2X = 1450; 
    int p2Y = 350;
    DrawText(game.player2.name, p2X, p2Y, 50, BLUE);
    DrawText("Phe: O", p2X, p2Y + 70, 30, DARKGRAY);
    DrawText(TextFormat("Thang: %d | Thua: %d", game.player2.winCount, game.player2.loseCount), p2X, p2Y + 120, 30, BLACK);
    DrawText(TextFormat("So buoc van nay: %d", game.player2.stepCount), p2X, p2Y + 170, 25, GRAY);
    
    if (game.gameMode == 1) {
        DrawText(TextFormat("HP: %d / 3", game.player2.hp), p2X, p2Y + 220, 30, MAROON);
        DrawText(TextFormat("Scan: %d", game.player2.scansLeft), p2X, p2Y + 270, 30, ORANGE);
    }
    
    if (game.matchStatus != 0) {
        DrawRectangle(ui.cellStartX, ui.cellStartY, BOARD_SIZE * ui.cellSize, BOARD_SIZE * ui.cellSize, Fade(WHITE, 0.7f));
        
        if (game.matchStatus == 1) DrawText(TextFormat("%s (X) THANG!", game.player1.name), 800, 500, 50, RED);
        if (game.matchStatus == 2) DrawText(TextFormat("%s (O) THANG!", game.player2.name), 800, 500, 50, BLUE);
        if (game.matchStatus == 3) DrawText("HOA NHAU!", 850, 500, 50, GRAY);
        Rectangle btnPlayAgain = { 720, 560, 220, 50 };
        Rectangle btnMenu = { 980, 560, 220, 50 };

        Color colorPlayAgain = (ui.endGameSelection == 0) ? YELLOW : LIGHTGRAY;
        Color colorMenu = (ui.endGameSelection == 1) ? YELLOW : LIGHTGRAY;

        DrawRectangleRec(btnPlayAgain, colorPlayAgain);
        DrawRectangleLinesEx(btnPlayAgain, 3.0f, BLACK);
        DrawText("Choi Van Moi", 745, 572, 25, BLACK);

        DrawRectangleRec(btnMenu, colorMenu);
        DrawRectangleLinesEx(btnMenu, 3.0f, BLACK);
        DrawText("Ve Menu", 1035, 572, 25, BLACK);

        DrawText("Dung [A]/[D] hoac Chuot de chon. [ENTER] de xac nhan.", 650, 640, 22, DARKGRAY);
    }
}