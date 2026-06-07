#include "GUI_Game.h"
#include "Library.h"

// Kiểm tra xem nước vừa đánh tại (row,col) có chặn đối thủ (oppPlayer) không.
// Trả về true nếu:
//   - Chặn 1 đầu của 3 quân liên tiếp đối thủ (đầu kia đã bị chặn bởi biên/quân ta)
//   - Chặn cả 2 đầu của 4 quân liên tiếp đối thủ (biên tính là chặn)
static bool CheckBlockVariant(GameState& game, int row, int col, int oppPlayer) {
    const int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};
    for (int d = 0; d < 4; d++) {
        int dx = dirs[d][0], dy = dirs[d][1];
        int count = 0, blocks = 0;
        GetLineStatus(game, row, col, dx, dy, oppPlayer, count, blocks);
        // count = số quân đối thủ liên tiếp (không tính ô vừa đánh)
        // blocks = số đầu bị chặn (biên + quân không phải oppPlayer)
        // Ô vừa đánh của ta nằm ở một trong hai đầu → nó đóng góp 1 block
        // GetLineStatus tính block ở đầu trống → ta đặt vào đó thì đầu đó thành bị chặn
        // Nhưng GetLineStatus không biết ô (row,col) vừa được đặt, nên cần xét thêm:
        // Thực ra sau MakeMove, board[row][col] đã là quân ta, GetLineStatus sẽ thấy
        // quân ta chặn một phía và biên/quân khác chặn phía kia.
        // Điều kiện: 3 quân liên tiếp + 1 đầu bị chặn (blocks >= 1, count == 3)
        //            4 quân liên tiếp + 2 đầu đều bị chặn (blocks == 2, count == 4)
        if (count == 3) return true;  // chặn 1 đầu 3 quân, không quan tâm đầu kia
        if (count == 4 && blocks == 2) return true;  // chặn 2 đầu 4 quân (biên tính là chặn)
    }
    return false;
}

void UpdateGUIGame(GameState& game, UIState& ui) {
    float dt = GetFrameTime();
    Vector2 mouse = GetMousePosition();

    int p1Asset = HERO_MAP[ui.p1HeroSelection];
    int p2Asset = HERO_MAP[ui.p2HeroSelection];

    // Helper: get active attack anim by variant
    auto getAtk = [&](int asset, int variant) -> CharAnim& {
        if (variant == 1) return ui.heroAttack2[asset];
        if (variant == 2) return ui.heroAttack3[asset];
        return ui.heroAttack[asset];
    };

    if (ui.isP1Attacking) {
        CharAnim& atkAnim = getAtk(p1Asset, ui.p1AttackVariant);
        atkAnim.frameTimer += dt;
        if (atkAnim.frameTimer >= atkAnim.frameDuration) {
            atkAnim.frameTimer = 0.0f;
            atkAnim.currentFrame++;
            if (atkAnim.currentFrame >= atkAnim.frameCount) {
                ui.isP1Attacking = false;
                atkAnim.currentFrame = 0;
                // Nếu đây là attack_s3 (chiến thắng), giờ mới reveal win screen
                if (ui.pendingWin) {
                    game.matchStatus = ui.pendingWinStatus;
                    ui.pendingWin = false;
                    ui.pendingWinStatus = 0;
                }
            }
        }
    } else {
        CharAnim& idleAnim = ui.heroIdle[p1Asset];
        idleAnim.frameTimer += dt;
        if (idleAnim.frameTimer >= idleAnim.frameDuration) {
            idleAnim.frameTimer = 0.0f;
            idleAnim.currentFrame = (idleAnim.currentFrame + 1) % idleAnim.frameCount;
        }
    }

    if (ui.isP2Attacking) {
        CharAnim& atkAnim = getAtk(p2Asset, ui.p2AttackVariant);
        atkAnim.frameTimer += dt;
        if (atkAnim.frameTimer >= atkAnim.frameDuration) {
            atkAnim.frameTimer = 0.0f;
            atkAnim.currentFrame++;
            if (atkAnim.currentFrame >= atkAnim.frameCount) {
                ui.isP2Attacking = false;
                atkAnim.currentFrame = 0;
                if (ui.pendingWin) {
                    game.matchStatus = ui.pendingWinStatus;
                    ui.pendingWin = false;
                    ui.pendingWinStatus = 0;
                }
            }
        }
    } else {
        CharAnim& idleAnim = ui.heroIdle[p2Asset];
        idleAnim.frameTimer += dt;
        if (idleAnim.frameTimer >= idleAnim.frameDuration) {
            idleAnim.frameTimer = 0.0f;
            idleAnim.currentFrame = (idleAnim.currentFrame + 1) % idleAnim.frameCount;
        }
    }

    // --- 2. XỬ LÝ LOGIC ĐÁNH CỜ ---
        if (game.matchStatus == 0 && !ui.pendingWin) {
            bool moveMade = false;
            bool wasP1 = game.isPlayer1Turn;
            int moveRow = -1, moveCol = -1;
    
            if (game.isVsBot && !game.isPlayer1Turn) {
                game.botThinkTimer += dt;
                if (game.botThinkTimer >= 1.5f) {
                    game.botThinkTimer = 0.0f;
                    BotMove(game);
                    // Bot không có row/col riêng → dùng attack_s1 mặc định cho bot
                    // (bot move phức tạp hơn, không xác định được ô vừa đánh dễ dàng)
                    ui.isP2Attacking = true;
                    ui.p2AttackVariant = 0;
                    getAtk(p2Asset, ui.p2AttackVariant).currentFrame = 0;
                    getAtk(p2Asset, ui.p2AttackVariant).frameTimer = 0.0f;
                    // Kiểm tra win sau BotMove
                    if (game.matchStatus != 0) {
                        ui.pendingWinStatus = game.matchStatus;
                        ui.pendingWin = true;
                        game.matchStatus = 0;
                        ui.p2AttackVariant = 2;
                        getAtk(p2Asset, 2).currentFrame = 0;
                        getAtk(p2Asset, 2).frameTimer = 0.0f;
                    }
                }
            } else {
                if (game.inputType == 0) { // CHẾ ĐỘ CHUỘT
                float gridWidth = BOARD_SIZE * ui.cellSize;
                float gridHeight = BOARD_SIZE * ui.cellSize;
    
                if (mouse.x >= ui.cellStartX && mouse.x < ui.cellStartX + gridWidth &&
                    mouse.y >= ui.cellStartY && mouse.y < ui.cellStartY + gridHeight) {
                    
                    int col = (mouse.x - ui.cellStartX) / ui.cellSize;
                    int row = (mouse.y - ui.cellStartY) / ui.cellSize;
    
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        moveMade = MakeMove(game, row, col);
                        if (moveMade) { moveRow = row; moveCol = col; }
                    } 
                    else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                        // Click Phải -> Gọi Logic Quét Mìn (Không vẽ)
                        ScanMine(game, row, col); 
                    }
                }
            } 
            else if (game.inputType == 1) { // CHẾ ĐỘ BÀN PHÍM
                if (IsKeyPressed(KEY_W) && game.cursorRow > 0) game.cursorRow--;
                if (IsKeyPressed(KEY_S) && game.cursorRow < BOARD_SIZE - 1) game.cursorRow++;
                if (IsKeyPressed(KEY_A) && game.cursorCol > 0) game.cursorCol--;
                if (IsKeyPressed(KEY_D) && game.cursorCol < BOARD_SIZE - 1) game.cursorCol++;
                
                if (IsKeyPressed(KEY_ENTER)) {
                    moveMade = MakeMove(game, game.cursorRow, game.cursorCol);
                    if (moveMade) { moveRow = game.cursorRow; moveCol = game.cursorCol; }
                }
                if (IsKeyPressed(KEY_SPACE)) {
                    // Phím Space -> Gọi Logic Quét Mìn (Không vẽ)
                    ScanMine(game, game.cursorRow, game.cursorCol); 
                }
            }
            } // Ket thuc khoi else (khong phai luot bot)

            // --- 3. KÍCH HOẠT ANIMATION CHÉM NẾU ĐÁNH THÀNH CÔNG ---
            if (moveMade) {
                int lastRow = -1, lastCol = -1;
                // Tìm ô vừa đánh (ô có giá trị của wasP1 ? 1 : 2 vừa đặt)
                // Thực ra ta đã biết row/col từ input — lưu lại trước MakeMove
                // → Xem biến moveRow/moveCol được set bên dưới
                int variant = 0; // mặc định attack_s1
                int myPlayer = wasP1 ? 1 : 2;
                int oppPlayer = wasP1 ? 2 : 1;

                // Kiểm tra win: matchStatus != 0 nghĩa là nước này thắng
                if (game.matchStatus != 0) {
                    variant = 2; // attack_s3
                    ui.pendingWinStatus = game.matchStatus;
                    ui.pendingWin = true;
                    game.matchStatus = 0; // giữ lại, reveal sau animation
                } else {
                    // Kiểm tra xem có chặn đối thủ không (dùng row/col đã đánh)
                    if (CheckBlockVariant(game, moveRow, moveCol, oppPlayer)) {
                        variant = 1; // attack_s2
                    }
                }

                if (wasP1) {
                    ui.isP1Attacking = true;
                    ui.p1AttackVariant = variant;
                    getAtk(p1Asset, variant).currentFrame = 0;
                    getAtk(p1Asset, variant).frameTimer = 0.0f;
                } else {
                    ui.isP2Attacking = true;
                    ui.p2AttackVariant = variant;
                    getAtk(p2Asset, variant).currentFrame = 0;
                    getAtk(p2Asset, variant).frameTimer = 0.0f;
                }
            }
        }
    else { // TRẠNG THÁI END GAME (Thắng/Thua/Hòa)
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
            if (ui.endGameSelection == 0) { ResetRound(game); ui.pendingWin = false; ui.pendingWinStatus = 0; }
            else ui.currentScreen = 0; 
        }
    }

    // PHÍM TẮT
    if (IsKeyPressed(KEY_M)) ui.currentScreen = 0; 
    if (IsKeyPressed(KEY_L)) {
        ui.currentScreen = 6; 
        ui.nameInput[0] = '\0'; 
        ui.letterCount = 0;
        ui.saveSelection = 0;
    }
}

static void DrawCharAnim(const CharAnim& c, float x, float y, float drawW, float drawH, bool flipH, bool isActive) {
    Rectangle srcRec = { (float)(c.currentFrame * c.frameWidth), 0, flipH ? -(float)c.frameWidth : (float)c.frameWidth, (float)c.frameHeight };
    Rectangle dstRec = { x, y, drawW, drawH };
    Color tint = isActive ? WHITE : Fade(GRAY, 0.5f);
    DrawTexturePro(c.spriteSheet, srcRec, dstRec, {0,0}, 0.0f, tint);
}

void DrawGUIGame(const GameState& game, const UIState& ui) {
    // NỀN & BÀN CỜ
    DrawTexturePro(ui.bgGame, { 0, 0, (float)ui.bgGame.width, (float)ui.bgGame.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
    Rectangle frameSrc = { 0, 0, (float)ui.boardFrame.width, (float)ui.boardFrame.height };
    DrawTexturePro(ui.boardFrame, frameSrc, ui.boardFrameRec, {0, 0}, 0.0f, WHITE);

    Vector2 mousePos = GetMousePosition();
    
    // VẼ CÁC Ô CỜ VÀ QUÂN CỜ
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
                DrawTexturePro(ui.pieceX, { 0, 0, (float)ui.pieceX.width, (float)ui.pieceX.height }, cellRect, {0, 0}, 0.0f, WHITE);
            } 
            else if (game.board[i][j].c == 2) {
                DrawTexturePro(ui.pieceO, { 0, 0, (float)ui.pieceO.width, (float)ui.pieceO.height }, cellRect, {0, 0}, 0.0f, WHITE);
            }
        }
    }
    
    if (game.inputType == 1) {
        float cx = ui.cellStartX + game.cursorCol * ui.cellSize;
        float cy = ui.cellStartY + game.cursorRow * ui.cellSize;
        DrawRectangleLinesEx({cx, cy, ui.cellSize, ui.cellSize}, 3.0f, DARKGREEN);
    }

    // THÔNG TIN VÁN ĐẤU (BADGES)
    DrawBadgeText(ui.mainFont, ui.titleBadge, "THONG TIN VAN DAU", 15, 402.3f, 84.7f, 48, WHITE, 0.5f);
    char roundText[30];
    snprintf(roundText, sizeof(roundText), "VONG CHOI %d", game.roundCount);
    DrawBadgeText(ui.mainFont, ui.roundBadge, roundText, 100, 261.1f, 74.6f, 39, WHITE, -5.0f);

    const char* guide = "Press [L] to save | Press [M] for Menu";
    float guideSize = 25;
    float guideWidth = MeasureTextEx(ui.mainFont, guide, guideSize, 0).x;
    DrawTextCustom(ui.mainFont, guide, 1920.0f / 2.0f - guideWidth / 2.0f, 1000, guideSize, WHITE);

    // VẼ TÊN PLAYER 1 & 2
    int badgePWidth = 504, badgePHeight = 109, nameFontSize = 55; 
    
    float p1X = 31.2f, p1Y = 386.7f;
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height}, {p1X, p1Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    int p1NameWidth = MeasureTextCustomX(ui.mainFont, game.player1.name, nameFontSize);
    int p1NameHeight = MeasureTextCustomY(ui.mainFont, game.player1.name, nameFontSize);
    DrawTextCustom(ui.mainFont, game.player1.name, p1X + (badgePWidth - p1NameWidth) / 2.0f, p1Y + (badgePHeight - p1NameHeight) / 2.0f - 5.0f, nameFontSize, WHITE);
   // --- VẼ UI MÁU VÀ RADAR CHO PLAYER 1 ---
   if (game.gameMode == 1) {
        float uiScale = 0.35f; // Chỉnh độ to nhỏ của toàn bộ UI ở đây
        float uiX_P1 = p1X - 10.0f; 
        float uiY_P1 = p1Y + 110.0f;

        // 1. Vẽ Khung sắt Base
        DrawTextureEx(ui.uiBase, { uiX_P1, uiY_P1 }, 0.0f, uiScale, WHITE);

        // ==========================================
        // 🛠️ TỌA ĐỘ VÀ KÍCH THƯỚC ĐÃ ĐƯỢC ÉP TỶ LỆ CHUẨN
        // Dựa vào ảnh gốc: Khe nhét bắt đầu ở vị trí ~37.5% chiều ngang
        // ==========================================
        // Tính toán khe nhét MÁU
        float hp_X = uiX_P1 + (ui.uiBase.width * 0.375f * uiScale); // Lùi sang phải 37.5%
        float hp_Y = uiY_P1 + (ui.uiBase.height * 0.18f * uiScale);  // Lùi xuống 18%
        float hp_W = ui.uiBase.width * 0.585f * uiScale;            // Độ rộng khe máu
        float hp_H = ui.uiBase.height * 0.23f * uiScale;             // Độ cao khe máu

        // Tính toán khe nhét RADAR
        float radar_X = uiX_P1 + (ui.uiBase.width * 0.375f * uiScale);
        float radar_Y = uiY_P1 + (ui.uiBase.height * 0.58f * uiScale); // Lùi xuống sâu hơn (58%)
        float radar_W = ui.uiBase.width * 0.38f * uiScale;            // Độ rộng khe radar (ngắn hơn)
        float radar_H = ui.uiBase.height * 0.22f * uiScale;
        // ==========================================

        // 2. Vẽ Máu P1
        float hpPercentP1 = (float)game.player1.hp / 2.0f;
        if (hpPercentP1 > 0) {
            Rectangle hpSrc = { 0, 0, ui.uiHp.width * hpPercentP1, (float)ui.uiHp.height };
            // Tự động kéo dãn chiều width/height cho khớp với khe hp_W và hp_H
            Rectangle hpDest = { hp_X, hp_Y, hp_W * hpPercentP1, hp_H };
            DrawTexturePro(ui.uiHp, hpSrc, hpDest, {0, 0}, 0.0f, WHITE);
        }

        // 3. Vẽ Radar P1
        float scanPercentP1 = (float)game.player1.scansLeft / 2.0f;
        if (scanPercentP1 > 0) {
            Rectangle scanSrc = { 0, 0, ui.uiRadar.width * scanPercentP1, (float)ui.uiRadar.height };
            Rectangle scanDest = { radar_X, radar_Y, radar_W * scanPercentP1, radar_H };
            DrawTexturePro(ui.uiRadar, scanSrc, scanDest, {0, 0}, 0.0f, WHITE);
        }
    }
    float p2X = 1388.4f, p2Y = 386.7f;
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height}, {p2X, p2Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    int p2NameWidth = MeasureTextCustomX(ui.mainFont, game.player2.name, nameFontSize);
    int p2NameHeight = MeasureTextCustomY(ui.mainFont, game.player2.name, nameFontSize);
    DrawTextCustom(ui.mainFont, game.player2.name, p2X + (badgePWidth - p2NameWidth) / 2.0f, p2Y + (badgePHeight - p2NameHeight) / 2.0f - 5.0f, nameFontSize, WHITE);
    // --- VẼ UI MÁU VÀ RADAR CHO PLAYER 2 ---
    if (game.gameMode == 1) {
        float uiScale = 0.35f; 
        float uiX_P2 = p2X - 10.0f; 
        float uiY_P2 = p2Y + 110.0f;

        DrawTextureEx(ui.uiBase, { uiX_P2, uiY_P2 }, 0.0f, uiScale, WHITE);

        // Dùng chung tỷ lệ như Player 1
        float hp_X = uiX_P2 + (ui.uiBase.width * 0.375f * uiScale);
        float hp_Y = uiY_P2 + (ui.uiBase.height * 0.18f * uiScale);
        float hp_W = ui.uiBase.width * 0.585f * uiScale;
        float hp_H = ui.uiBase.height * 0.23f * uiScale;

        float radar_X = uiX_P2 + (ui.uiBase.width * 0.375f * uiScale);
        float radar_Y = uiY_P2 + (ui.uiBase.height * 0.58f * uiScale);
        float radar_W = ui.uiBase.width * 0.38f * uiScale;
        float radar_H = ui.uiBase.height * 0.22f * uiScale;

        // Vẽ Máu P2
        float hpPercentP2 = (float)game.player2.hp / 2.0f;
        if (hpPercentP2 > 0) {
            Rectangle hpSrc = { 0, 0, ui.uiHp.width * hpPercentP2, (float)ui.uiHp.height };
            Rectangle hpDest = { hp_X, hp_Y, hp_W * hpPercentP2, hp_H };
            DrawTexturePro(ui.uiHp, hpSrc, hpDest, {0, 0}, 0.0f, WHITE);
        }

        // Vẽ Radar P2
        float scanPercentP2 = (float)game.player2.scansLeft / 2.0f;
        if (scanPercentP2 > 0) {
            Rectangle scanSrc = { 0, 0, ui.uiRadar.width * scanPercentP2, (float)ui.uiRadar.height };
            Rectangle scanDest = { radar_X, radar_Y, radar_W * scanPercentP2, radar_H };
            DrawTexturePro(ui.uiRadar, scanSrc, scanDest, {0, 0}, 0.0f, WHITE);
        }
    }
    // --- VẼ NHÂN VẬT ---
    int p1A = HERO_MAP[ui.p1HeroSelection];
    int p2A = HERO_MAP[ui.p2HeroSelection];

    auto getAtkC = [&](int asset, int variant) -> const CharAnim& {
        if (variant == 1) return ui.heroAttack2[asset];
        if (variant == 2) return ui.heroAttack3[asset];
        return ui.heroAttack[asset];
    };

    const CharAnim& p1Anim = ui.isP1Attacking ? getAtkC(p1A, ui.p1AttackVariant) : ui.heroIdle[p1A];
    bool p1Active = (game.isPlayer1Turn && game.matchStatus == 0) || ui.isP1Attacking;
    Vector2 p1Size = ui.heroDrawSize[p1A];
    Vector2 p1Offset = ui.heroDrawOffset[p1A];
    DrawCharAnim(p1Anim, 135.0f + p1Offset.x, 640.0f + p1Offset.y, p1Size.x, p1Size.y, false, p1Active);

    const CharAnim& p2Anim = ui.isP2Attacking ? getAtkC(p2A, ui.p2AttackVariant) : ui.heroIdle[p2A];
    bool p2Active = (!game.isPlayer1Turn && game.matchStatus == 0) || ui.isP2Attacking;
    Vector2 p2Size = ui.heroDrawSize[p2A];
    Vector2 p2Offset = ui.heroDrawOffset[p2A];
    DrawCharAnim(p2Anim, 1467.0f + p2Offset.x, 640.0f + p2Offset.y, p2Size.x, p2Size.y, true, p2Active);

    // VẼ MÀN HÌNH END GAME
    if (game.matchStatus != 0) {
        DrawRectangle((int)ui.cellStartX, (int)ui.cellStartY, BOARD_SIZE * (int)ui.cellSize, BOARD_SIZE * (int)ui.cellSize, Fade(WHITE, 0.7f));
        
        const char* winText = "";
        Color winColor = WHITE;
        if (game.matchStatus == 1) { winText = TextFormat("%s (X) WINS!", game.player1.name); winColor = RED; }
        else if (game.matchStatus == 2) { winText = TextFormat("%s (O) WINS!", game.player2.name); winColor = BLUE; }
        else if (game.matchStatus == 3) { winText = "DRAW!"; winColor = GRAY; }
        
        int winWidth = MeasureTextCustomX(ui.mainFont, winText, 50);
        DrawTextCustom(ui.mainFont, winText, 960 - winWidth / 2, 470, 50, winColor);

        Rectangle btnPlayAgain = { 960 - 240, 550, 220, 50 };
        Rectangle btnMenu      = { 960 + 20,  550, 220, 50 };
        Color colorPlayAgain = (ui.endGameSelection == 0) ? YELLOW : LIGHTGRAY;
        Color colorMenu = (ui.endGameSelection == 1) ? YELLOW : LIGHTGRAY;

        DrawRectangleRec(btnPlayAgain, colorPlayAgain); DrawRectangleLinesEx(btnPlayAgain, 3.0f, BLACK);
        DrawRectangleRec(btnMenu, colorMenu); DrawRectangleLinesEx(btnMenu, 3.0f, BLACK);

        int wPlay = MeasureTextCustomX(ui.mainFont, "Play Again", 25);
        int wMenu = MeasureTextCustomX(ui.mainFont, "Main Menu", 25);
        DrawTextCustom(ui.mainFont, "Play Again", btnPlayAgain.x + (btnPlayAgain.width - wPlay) / 2, btnPlayAgain.y + 12, 25, BLACK);
        DrawTextCustom(ui.mainFont, "Main Menu", btnMenu.x + (btnMenu.width - wMenu) / 2, btnMenu.y + 12, 25, BLACK);

        const char* guideEnd = "Use [A]/[D] or Mouse to select. [ENTER] to confirm.";
        int guideEndW = MeasureTextCustomX(ui.mainFont, guideEnd, 22);
        DrawTextCustom(ui.mainFont, guideEnd, 960 - guideEndW / 2, 620, 22, DARKGRAY);
    }
}