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

        Rectangle btnPlayAgain = { 960 - 240, 550, 220, 50 }; 
        Rectangle btnMenu      = { 960 + 20,  550, 220, 50 };
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
   // --- 1. THÔNG TIN VÁN ĐẤU (offsetY = 8.0f và 5.0f để đẩy chữ xuống cho khớp khung) ---
    DrawBadgeText(ui.mainFont, ui.titleBadge, "THONG TIN VAN DAU", 15, 402.3f, 84.7f, 48, WHITE, 0.5f);

    char roundText[30];
    sprintf(roundText, "VONG CHOI %d", game.roundCount);
    DrawBadgeText(ui.mainFont, ui.roundBadge, roundText, 100, 261.1f, 74.6f, 39, WHITE, -5.0f);

    const char* guide = "Bam [L] de luu game | Nhan [M] de ve Menu";
    float guideSize = 25;
    float guideWidth = MeasureTextEx(ui.mainFont, guide, guideSize, 0).x;
    float guideX = 1920.0f / 2.0f - guideWidth / 2.0f; 
    DrawTextCustom(ui.mainFont, guide, guideX, 1000, guideSize, WHITE);

    // --- 2. THÔNG TIN NGƯỜI CHƠI ---
    int badgePWidth = 504;
    int badgePHeight = 109;
    int nameFontSize = 55; // Giảm từ 72 xuống 55 để chữ không bị tràn viền

    // Player 1 (Bên Trái)
    float p1X = 31.2f; 
    float p1Y = 386.7f;
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height},
                   {p1X, p1Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    
    int p1NameWidth = MeasureTextCustomX(ui.mainFont, game.player1.name, nameFontSize);
    int p1NameHeight = MeasureTextCustomY(ui.mainFont, game.player1.name, nameFontSize);
    float p1TextY = p1Y + (badgePHeight - p1NameHeight) / 2.0f - 5.0f; // Kéo chữ lên 5px cho cân
    DrawTextCustom(ui.mainFont, game.player1.name, p1X + (badgePWidth - p1NameWidth) / 2.0f, p1TextY, nameFontSize, WHITE);

    if (game.gameMode == 1) {
        DrawTextCustom(ui.mainFont, TextFormat("HP: %d / 3", game.player1.hp), p1X + 150, p1Y + 120, 30, MAROON);
        DrawTextCustom(ui.mainFont, TextFormat("Scan: %d", game.player1.scansLeft), p1X + 150, p1Y + 170, 30, ORANGE);
    }

    // Player 2 (Bên Phải)
    float p2X = 1388.4f; 
    float p2Y = 386.7f;
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height},
                   {p2X, p2Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    
    int p2NameWidth = MeasureTextCustomX(ui.mainFont, game.player2.name, nameFontSize);
    int p2NameHeight = MeasureTextCustomY(ui.mainFont, game.player2.name, nameFontSize);
    float p2TextY = p2Y + (badgePHeight - p2NameHeight) / 2.0f - 5.0f; // Kéo chữ lên 5px cho cân
    DrawTextCustom(ui.mainFont, game.player2.name, p2X + (badgePWidth - p2NameWidth) / 2.0f, p2TextY, nameFontSize, WHITE);
    
    if (game.gameMode == 1) {
        DrawTextCustom(ui.mainFont, TextFormat("HP: %d / 3", game.player2.hp), p2X + 150, p2Y + 120, 30, MAROON);
        DrawTextCustom(ui.mainFont, TextFormat("Scan: %d", game.player2.scansLeft), p2X + 150, p2Y + 170, 30, ORANGE);
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
        // 1. Làm mờ bàn cờ
        DrawRectangle((int)ui.cellStartX, (int)ui.cellStartY, BOARD_SIZE * (int)ui.cellSize, BOARD_SIZE * (int)ui.cellSize, Fade(WHITE, 0.7f));
        
        // 2. Tự động đo độ dài chữ và Căn giữa thông báo Thắng/Thua
        const char* winText = "";
        Color winColor = WHITE;
        if (game.matchStatus == 1) { winText = TextFormat("%s (X) THANG!", game.player1.name); winColor = RED; }
        else if (game.matchStatus == 2) { winText = TextFormat("%s (O) THANG!", game.player2.name); winColor = BLUE; }
        else if (game.matchStatus == 3) { winText = "HOA NHAU!"; winColor = GRAY; }
        
        int winWidth = MeasureTextCustomX(ui.mainFont, winText, 50);
        DrawTextCustom(ui.mainFont, winText, 960 - winWidth / 2, 470, 50, winColor);

        // 3. Vẽ Khung 2 nút bấm
        Rectangle btnPlayAgain = { 960 - 240, 550, 220, 50 };
        Rectangle btnMenu      = { 960 + 20,  550, 220, 50 };

        Color colorPlayAgain = (ui.endGameSelection == 0) ? YELLOW : LIGHTGRAY;
        Color colorMenu = (ui.endGameSelection == 1) ? YELLOW : LIGHTGRAY;

        DrawRectangleRec(btnPlayAgain, colorPlayAgain);
        DrawRectangleLinesEx(btnPlayAgain, 3.0f, BLACK);

        DrawRectangleRec(btnMenu, colorMenu);
        DrawRectangleLinesEx(btnMenu, 3.0f, BLACK);

        // 4. Tự động căn giữa chữ bên TRONG từng nút bấm
        const char* txtPlay = "Choi Van Moi";
        const char* txtMenu = "Ve Menu";
        int wPlay = MeasureTextCustomX(ui.mainFont, txtPlay, 25);
        int wMenu = MeasureTextCustomX(ui.mainFont, txtMenu, 25);

        DrawTextCustom(ui.mainFont, txtPlay, btnPlayAgain.x + (btnPlayAgain.width - wPlay) / 2, btnPlayAgain.y + 12, 25, BLACK);
        DrawTextCustom(ui.mainFont, txtMenu, btnMenu.x + (btnMenu.width - wMenu) / 2, btnMenu.y + 12, 25, BLACK);

        // 5. Căn giữa dòng Hướng dẫn thao tác
        const char* guideEnd = "Dung [A]/[D] hoac Chuot de chon. [ENTER] de xac nhan.";
        int guideEndW = MeasureTextCustomX(ui.mainFont, guideEnd, 22);
        DrawTextCustom(ui.mainFont, guideEnd, 960 - guideEndW / 2, 620, 22, DARKGRAY);
    }
}
