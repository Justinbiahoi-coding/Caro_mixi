#include "GUI_Game.h"
#include "LogicControl.h"

void UpdateGUIGame(GameState& game, UIState& ui) {

    Vector2 mouse = GetMousePosition();
    float dt = GetFrameTime();

    // Chỉ animate phe đang có lượt, phe kia giữ frame 0
    if (game.matchStatus == 0) {
        CharAnim& active = game.isPlayer1Turn ? ui.charP1 : ui.charP2;
        CharAnim& idle   = game.isPlayer1Turn ? ui.charP2 : ui.charP1;

        active.frameTimer += dt;
        if (active.frameTimer >= active.frameDuration) {
            active.frameTimer = 0.0f;
            active.currentFrame = (active.currentFrame + 1) % active.frameCount;
        }

        // Phe chờ: reset về frame 0
        idle.currentFrame = 0;
        idle.frameTimer   = 0.0f;
    }

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

static void DrawCharAnim(const CharAnim& c,
                         float x, float y, float drawW, float drawH,
                         bool flipH, bool isActive)
{
    Rectangle srcRec = {
        (float)(c.currentFrame * c.frameWidth),
        0,
        flipH ? -(float)c.frameWidth : (float)c.frameWidth,
        (float)c.frameHeight
    };
    Rectangle dstRec = { x, y, drawW, drawH };
    Color tint = isActive ? WHITE : Fade(GRAY, 0.5f);
    DrawTexturePro(c.spriteSheet, srcRec, dstRec, {0,0}, 0.0f, tint);
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
    DrawBadgeText(ui.mainFont, ui.titleBadge, "THONG TIN VAN DAU", 15, 402.3f, 84.7f, 48, WHITE);

    char roundText[30];
    sprintf(roundText, "VONG CHOI %d", game.roundCount);
    DrawBadgeText(ui.mainFont, ui.roundBadge, roundText, 100, 261.1f, 74.6f, 39, WHITE);

    // if (game.matchStatus == 0) {
    //     if (game.isPlayer1Turn) DrawTextCustom(ui.mainFont, TextFormat(">> LUOT CUA: %s (X) <<", game.player1.name), 790, 100, 30, RED);
    //     else DrawTextCustom(ui.mainFont, TextFormat(">> LUOT CUA: %s (O) <<", game.player2.name), 790, 100, 30, BLUE);
    // }

    // if (game.inputType == 0) DrawTextCustom(ui.mainFont, "Dieu khien: Chuot", 880, 140, 20, GRAY);
    // else DrawTextCustom(ui.mainFont, "Dieu khien: WASD + Enter", 850, 140, 20, GRAY);
    
    // DrawTextCustom(ui.mainFont, "Bam [L] de luu game | Nhan [M] de ve Menu", 720, 980, 25, WHITE);
    const char* guide = "Bam [L] de luu game | Nhan [M] de ve Menu";
    float guideSize = 25;
    float guideWidth = MeasureTextEx(ui.mainFont, guide, guideSize, 0).x;
    float guideX = GetScreenWidth() / 2.0f - guideWidth / 2.0f;
    DrawTextCustom(ui.mainFont, guide, guideX, 1000, guideSize, WHITE);

    // Kích thước cố định cho badge tên người chơi
    int badgePWidth = 504;
    int badgePHeight = 109;

    // --- Player 1 (Bên Trái) ---
    float p1X = 31.2; 
    float p1Y = 386.7 ;
    // Vẽ badge dưới tên
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height},
                   {(float)p1X, (float)p1Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    
    // Căn giữa text tên người chơi vào trong badge
    int p1NameWidth = MeasureTextCustomX(ui.mainFont, game.player1.name, 72);
    DrawTextCustom(ui.mainFont, game.player1.name, p1X + (badgePWidth - p1NameWidth) / 2, p1Y + 15, 72, WHITE);


    // DrawTextCustom(ui.mainFont, "Phe: X", p1X, p1Y + 70, 30, DARKGRAY);
    // DrawTextCustom(ui.mainFont, TextFormat("Thang: %d | Thua: %d", game.player1.winCount, game.player1.loseCount), p1X, p1Y + 120, 30, BLACK);
    // DrawTextCustom(ui.mainFont, TextFormat("So buoc van nay: %d", game.player1.stepCount), p1X, p1Y + 170, 25, GRAY);
    
    if (game.gameMode == 1) {
        DrawTextCustom(ui.mainFont, TextFormat("HP: %d / 3", game.player1.hp), p1X, p1Y + 220, 30, MAROON);
        DrawTextCustom(ui.mainFont, TextFormat("Scan: %d", game.player1.scansLeft), p1X, p1Y + 270, 30, ORANGE);
    }

    float p2X = 1388.4; 
    float p2Y = 386.7 ;
    // Vẽ badge dưới tên
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height},
                   {(float)p2X, (float)p2Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    
    // Căn giữa text tên người chơi vào trong badge
    int p2NameWidth = MeasureTextCustomX(ui.mainFont, game.player2.name, 72);
    DrawTextCustom(ui.mainFont, game.player2.name, p2X + (badgePWidth - p2NameWidth) / 2, p2Y + 15, 72, WHITE);
    // DrawTextCustom(ui.mainFont, "Phe: O", p2X, p2Y + 70, 30, DARKGRAY);
    // DrawTextCustom(ui.mainFont, TextFormat("Thang: %d | Thua: %d", game.player2.winCount, game.player2.loseCount), p2X, p2Y + 120, 30, BLACK);
    // DrawTextCustom(ui.mainFont, TextFormat("So buoc van nay: %d", game.player2.stepCount), p2X, p2Y + 170, 25, GRAY);
    
    if (game.gameMode == 1) {
        DrawTextCustom(ui.mainFont, TextFormat("HP: %d / 3", game.player2.hp), p2X, p2Y + 220, 30, MAROON);
        DrawTextCustom(ui.mainFont, TextFormat("Scan: %d", game.player2.scansLeft), p2X, p2Y + 270, 30, ORANGE);
    }
    
    float P1charW = 356.0f, P1charH = 356.0f;
    float P2charW = 356.0f, P2charH = 356.0f;

    // Player 1 — không flip
    bool p1Active = game.isPlayer1Turn && (game.matchStatus == 0);
    DrawCharAnim(ui.charP1,
                135.0f,
                640.0f,
                P1charW, P1charH,
                false, p1Active);

    // Player 2 — flip ngang
    bool p2Active = !game.isPlayer1Turn && (game.matchStatus == 0);
    DrawCharAnim(ui.charP2,
                1467.0f,
                640.0f,
                P2charW, P2charH,
                true, p2Active);

    if (game.matchStatus != 0) {
        DrawRectangle(ui.cellStartX, ui.cellStartY, BOARD_SIZE * ui.cellSize, BOARD_SIZE * ui.cellSize, Fade(WHITE, 0.7f));
        
        if (game.matchStatus == 1) DrawTextCustom(ui.mainFont, TextFormat("%s (X) THANG!", game.player1.name), 800, 500, 50, RED);
        if (game.matchStatus == 2) DrawTextCustom(ui.mainFont, TextFormat("%s (O) THANG!", game.player2.name), 800, 500, 50, BLUE);
        if (game.matchStatus == 3) DrawTextCustom(ui.mainFont, "HOA NHAU!", 850, 500, 50, GRAY);
        Rectangle btnPlayAgain = { 720, 560, 220, 50 };
        Rectangle btnMenu = { 980, 560, 220, 50 };

        Color colorPlayAgain = (ui.endGameSelection == 0) ? YELLOW : LIGHTGRAY;
        Color colorMenu = (ui.endGameSelection == 1) ? YELLOW : LIGHTGRAY;

        DrawRectangleRec(btnPlayAgain, colorPlayAgain);
        DrawRectangleLinesEx(btnPlayAgain, 3.0f, BLACK);
        DrawTextCustom(ui.mainFont, "Choi Van Moi", 745, 572, 25, BLACK);

        DrawRectangleRec(btnMenu, colorMenu);
        DrawRectangleLinesEx(btnMenu, 3.0f, BLACK);
        DrawTextCustom(ui.mainFont, "Ve Menu", 1035, 572, 25, BLACK);

        DrawTextCustom(ui.mainFont, "Dung [A]/[D] hoac Chuot de chon. [ENTER] de xac nhan.", 650, 640, 22, DARKGRAY);
    }
}