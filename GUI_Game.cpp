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

    // Helper: kích hoạt death cho kẻ thua sau attack_s3
    auto triggerDeath = [&]() {
        // pendingWinStatus: 1 = P1 thắng (P2 thua), 2 = P2 thắng (P1 thua)
        if (ui.pendingWinStatus == 1) {
            // P2 thua → P2 chạy death
            ui.isP2Dying = true;
            ui.heroDeath[p2Asset].currentFrame = 0;
            ui.heroDeath[p2Asset].frameTimer = 0.0f;
        } else if (ui.pendingWinStatus == 2) {
            // P1 thua → P1 chạy death
            ui.isP1Dying = true;
            ui.heroDeath[p1Asset].currentFrame = 0;
            ui.heroDeath[p1Asset].frameTimer = 0.0f;
        } else {
            // Draw — reveal ngay
            game.matchStatus = ui.pendingWinStatus;
            ui.pendingWin = false;
            ui.pendingWinStatus = 0;
        }
    };

    // Update P1 attack
    if (ui.isP1Attacking) {
        CharAnim& atkAnim = getAtk(p1Asset, ui.p1AttackVariant);
        atkAnim.frameTimer += dt;
        if (atkAnim.frameTimer >= atkAnim.frameDuration) {
            atkAnim.frameTimer = 0.0f;
            atkAnim.currentFrame++;
            if (atkAnim.currentFrame >= atkAnim.frameCount) {
                ui.isP1Attacking = false;
                atkAnim.currentFrame = 0;
                if (ui.pendingWin) triggerDeath();
            }
        }
    } else if (ui.isP1Dying) {
        CharAnim& deathAnim = ui.heroDeath[p1Asset];
        deathAnim.frameTimer += dt;
        if (deathAnim.frameTimer >= deathAnim.frameDuration) {
            deathAnim.frameTimer = 0.0f;
            deathAnim.currentFrame++;
            if (deathAnim.currentFrame >= deathAnim.frameCount) {
                // Giữ frame cuối (không reset) — nhân vật nằm yên
                deathAnim.currentFrame = deathAnim.frameCount - 1;
                // Reveal win screen
                game.matchStatus = ui.pendingWinStatus;
                ui.pendingWin = false;
                ui.pendingWinStatus = 0;
                ui.isP1Dying = false;
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

    // Update P2 attack
    if (ui.isP2Attacking) {
        CharAnim& atkAnim = getAtk(p2Asset, ui.p2AttackVariant);
        atkAnim.frameTimer += dt;
        if (atkAnim.frameTimer >= atkAnim.frameDuration) {
            atkAnim.frameTimer = 0.0f;
            atkAnim.currentFrame++;
            if (atkAnim.currentFrame >= atkAnim.frameCount) {
                ui.isP2Attacking = false;
                atkAnim.currentFrame = 0;
                if (ui.pendingWin) triggerDeath();
            }
        }
    } else if (ui.isP2Dying) {
        CharAnim& deathAnim = ui.heroDeath[p2Asset];
        deathAnim.frameTimer += dt;
        if (deathAnim.frameTimer >= deathAnim.frameDuration) {
            deathAnim.frameTimer = 0.0f;
            deathAnim.currentFrame++;
            if (deathAnim.currentFrame >= deathAnim.frameCount) {
                deathAnim.currentFrame = deathAnim.frameCount - 1;
                game.matchStatus = ui.pendingWinStatus;
                ui.pendingWin = false;
                ui.pendingWinStatus = 0;
                ui.isP2Dying = false;
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
        if (game.matchStatus == 0 && !ui.pendingWin && !ui.isP1Dying && !ui.isP2Dying) {
            bool moveMade = false;
            bool wasP1 = game.isPlayer1Turn;
            int moveRow = -1, moveCol = -1;
    
            if (game.isVsBot && !game.isPlayer1Turn) {
                game.botThinkTimer += dt;
                if (game.botThinkTimer >= 1.5f) {
                    game.botThinkTimer = 0.0f;
                    BotMove(game);
                    ui.isP2Attacking = true;
                    ui.p2AttackVariant = 0;
                    getAtk(p2Asset, ui.p2AttackVariant).currentFrame = 0;
                    getAtk(p2Asset, ui.p2AttackVariant).frameTimer = 0.0f;
                    // Spawn cell effect cho ô bot vừa đánh
                    if (ui.cellEffectCount < UIState::MAX_CELL_EFFECTS) {
                        UIState::CellEffect& eff = ui.cellEffects[ui.cellEffectCount++];
                        eff.row = game.lastMoveRow;
                        eff.col = game.lastMoveCol;
                        eff.player = 2;
                        eff.heroAsset = p2Asset;
                        eff.timer = 0.0f;
                        eff.frameDur = 0.07f;
                        eff.currentFrame = 0;
                        eff.done = false;
                    }
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

                // Spawn cell effect tại ô vừa đánh
                if (ui.cellEffectCount < UIState::MAX_CELL_EFFECTS) {
                    int asset = wasP1 ? p1Asset : p2Asset;
                    UIState::CellEffect& eff = ui.cellEffects[ui.cellEffectCount++];
                    eff.row = moveRow;
                    eff.col = moveCol;
                    eff.player = wasP1 ? 1 : 2;
                    eff.heroAsset = asset;
                    eff.timer = 0.0f;
                    eff.frameDur = 0.07f;
                    eff.currentFrame = 0;
                    eff.done = false;
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
            if (ui.endGameSelection == 0) {
                ResetRound(game);
                ui.pendingWin = false; ui.pendingWinStatus = 0;
                ui.isP1Dying = false; ui.isP2Dying = false;
                ui.heroDeath[p1Asset].currentFrame = 0;
                ui.heroDeath[p2Asset].currentFrame = 0;
                ui.cellEffectCount = 0;
            }
            else ui.currentScreen = 0; 
        }
    }

    // Update cell effects — clamp dt để tránh skip toàn bộ animation khi lag
    float effDt = (dt > 0.1f) ? 0.1f : dt;
    for (int k = 0; k < ui.cellEffectCount; k++) {
        UIState::CellEffect& eff = ui.cellEffects[k];
        if (eff.done) continue;
        eff.timer += effDt;
        int totalFrames = ui.heroEffectFrames[eff.heroAsset];
        int frame = (int)(eff.timer / eff.frameDur);
        if (frame >= totalFrames) {
            eff.currentFrame = totalFrames - 1;
            eff.done = true;
        } else {
            eff.currentFrame = frame;
        }
    }

    // Reset cell effects khi bắt đầu ván mới (ResetRound đã gọi ở confirm)
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
    const int p1Asset = HERO_MAP[ui.p1HeroSelection];
    const int p2Asset = HERO_MAP[ui.p2HeroSelection];

    // NỀN & BÀN CỜ
    DrawTexturePro(ui.bgGame, { 0, 0, (float)ui.bgGame.width, (float)ui.bgGame.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

    // === PANEL NỀN 2 BÊN NHÂN VẬT — full chiều cao ===
    float boardL = ui.boardFrameRec.x;
    float boardR = ui.boardFrameRec.x + ui.boardFrameRec.width;

    // Panel trái: full màn hình, gradient tối mép → trong suốt sát board
    DrawRectangleGradientH(0, 0, (int)(boardL + 40), 1080,
        {5, 4, 2, 230}, {5, 4, 2, 0});

    // Panel phải: full màn hình
    DrawRectangleGradientH((int)(boardR - 40), 0, (int)(1920 - boardR + 40), 1080,
        {5, 4, 2, 0}, {5, 4, 2, 230});

    // Panel top: gradient từ top xuống (tối → trong suốt)
    DrawRectangleGradientV(0, 0, 1920, 160,
        {5, 4, 2, 200}, {5, 4, 2, 0});

    // Panel bottom: gradient từ bottom lên
    DrawRectangleGradientV(0, 920, 1920, 160,
        {5, 4, 2, 0}, {5, 4, 2, 200});

    // Vignette: chỉ tối ở mép ngoài màn hình, vùng nhân vật giữ sáng tự nhiên
    // Trái: mép ngoài tối (alpha 100), fade nhanh vào → 0 ở 1/3 panel
    DrawRectangleGradientH(0, 0, (int)(boardL * 0.5f), 1080, {0,0,0,100}, {0,0,0,0});
    // Phải
    DrawRectangleGradientH((int)(boardR + (1920-boardR)*0.5f), 0, (int)((1920-boardR)*0.5f), 1080, {0,0,0,0}, {0,0,0,100});
    // Top: chỉ tối phần trên cùng badge, fade nhanh
    DrawRectangleGradientV(0, 0, 1920, 80, {0,0,0,90}, {0,0,0,0});
    // Bottom
    float boardBot = ui.boardFrameRec.y + ui.boardFrameRec.height;
    DrawRectangleGradientV(0, (int)(boardBot + 40), 1920, 60, {0,0,0,0}, {0,0,0,80});


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

            if (game.board[i][j].c == 1 || game.board[i][j].c == 2) {
                int owner = game.board[i][j].c;
                int asset = (owner == 1) ? p1Asset : p2Asset;

                // Tìm cell effect cho ô này
                const UIState::CellEffect* eff = nullptr;
                for (int k = 0; k < ui.cellEffectCount; k++) {
                    if (ui.cellEffects[k].row == i && ui.cellEffects[k].col == j) {
                        eff = &ui.cellEffects[k];
                        break;
                    }
                }

                float pad = ui.cellSize * 0.10f;
                float drawSize = ui.cellSize - pad * 2.0f;
                float dx = x + pad, dy = y + pad;

                if (eff && !eff->done) {
                    // Vẽ frame effect hiện tại
                    const Texture2D& sheet = ui.heroEffect[eff->heroAsset];
                    int fh = sheet.height;
                    int fw = fh; // frame vuông
                    Rectangle src = { (float)(eff->currentFrame * fw), 0, (float)fw, (float)fh };
                    Rectangle dst = { dx, dy, drawSize, drawSize };
                    DrawTexturePro(sheet, src, dst, {0,0}, 0.0f, WHITE);
                } else {
                    // Effect xong → vẽ icon tĩnh
                    const Texture2D& icon = ui.heroIcon[asset];
                    Rectangle src = { 0, 0, (float)icon.width, (float)icon.height };
                    Rectangle dst = { dx, dy, drawSize, drawSize };
                    DrawTexturePro(icon, src, dst, {0,0}, 0.0f, WHITE);
                }
            }
        }
    }
    
    if (game.inputType == 1) {
        float cx = ui.cellStartX + game.cursorCol * ui.cellSize;
        float cy = ui.cellStartY + game.cursorRow * ui.cellSize;
        DrawRectangleLinesEx({cx, cy, ui.cellSize, ui.cellSize}, 3.0f, DARKGREEN);
    }

    // THÔNG TIN VÁN ĐẤU (BADGES)
    DrawBadgeText(ui.mainFont, ui.titleBadge, "MATCH INFO", 15, 402.3f, 84.7f, 48, WHITE, 0.5f);
    char roundText[30];
    snprintf(roundText, sizeof(roundText), "ROUND %d", game.roundCount);
    DrawBadgeText(ui.mainFont, ui.roundBadge, roundText, 100, 261.1f, 74.6f, 39, WHITE, -5.0f);

    // === THANH NGANG DƯỚI BOARD ===
    {
        float bx = ui.boardFrameRec.x;
        float bw = ui.boardFrameRec.width;
        float by = ui.boardFrameRec.y + ui.boardFrameRec.height + 12.0f;
        float bh = 50.0f;
        float third = bw / 3.0f;

        // Nền
        DrawRectangle((int)bx, (int)by, (int)bw, (int)bh, {20, 14, 4, 220});
        DrawRectangleLinesEx({bx, by, bw, bh}, 1.5f, {160, 120, 40, 200});

        // Đường chia 3 khối
        DrawRectangle((int)(bx + third),     (int)(by + 8), 1, (int)(bh - 16), {160, 120, 40, 150});
        DrawRectangle((int)(bx + third*2),   (int)(by + 8), 1, (int)(bh - 16), {160, 120, 40, 150});

        // Khối trái: [L] Save
        const char* saveStr = "[L]  Save";
        int sw = MeasureTextCustomX(ui.mainFont, saveStr, 26);
        DrawTextCustom(ui.mainFont, saveStr, bx + (third - sw)*0.5f, by + (bh-26)*0.5f, 26, {180, 150, 80, 220});

        // Khối giữa: lượt + số nước — highlight tên người đang đi
        const char* turnName = game.isPlayer1Turn ? game.player1.name : game.player2.name;
        char midText[60];
        snprintf(midText, sizeof(midText), "Turn: %s  |  Move: %d", turnName, game.moveCount);
        int mw = MeasureTextCustomX(ui.mainFont, midText, 26);
        DrawTextCustom(ui.mainFont, midText, bx + third + (third - mw)*0.5f, by + (bh-26)*0.5f, 26, {220, 190, 100, 255});

        // Khối phải: [M] Menu
        const char* menuStr = "[M]  Menu";
        int mnw = MeasureTextCustomX(ui.mainFont, menuStr, 26);
        DrawTextCustom(ui.mainFont, menuStr, bx + third*2 + (third - mnw)*0.5f, by + (bh-26)*0.5f, 26, {180, 150, 80, 220});
    }

    float glowT = (float)GetTime();
    float glowAlpha = 0.55f + 0.45f * sinf(glowT * 4.0f);

    // Màu theme theo hero (asset index 1-5)
    const Color heroThemeColors[6] = {
        {150, 150, 150, 255},  // 0: black_knight (hidden)
        {220,  70,  20, 255},  // 1: fire_knight
        { 40, 170, 110, 255},  // 2: green_archer
        { 60, 110, 220, 255},  // 3: wind_assassin
        {180, 160,  60, 255},  // 4: metal_blade
        { 30, 160, 220, 255},  // 5: water_mage
    };
    Color p1ThemeCol = heroThemeColors[HERO_MAP[ui.p1HeroSelection]];
    Color p2ThemeCol = heroThemeColors[HERO_MAP[ui.p2HeroSelection]];

    // VẼ TÊN PLAYER 1 & 2 — badge sát nhân vật
    int badgePWidth = 460, badgePHeight = 95, nameFontSize = 44;

    float p1X = 230.0f - badgePWidth * 0.5f, p1Y = 374.0f;
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height}, {p1X, p1Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    int p1NameWidth = MeasureTextCustomX(ui.mainFont, game.player1.name, nameFontSize);
    int p1NameHeight = MeasureTextCustomY(ui.mainFont, game.player1.name, nameFontSize);
    DrawTextCustom(ui.mainFont, game.player1.name, p1X + (badgePWidth - p1NameWidth) / 2.0f, p1Y + (badgePHeight - p1NameHeight) / 2.0f - 3.0f, nameFontSize, WHITE);
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
    float p2X = 1690.0f - badgePWidth * 0.5f,   p2Y = 374.0f;
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height}, {p2X, p2Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    int p2NameWidth = MeasureTextCustomX(ui.mainFont, game.player2.name, nameFontSize);
    int p2NameHeight = MeasureTextCustomY(ui.mainFont, game.player2.name, nameFontSize);
    DrawTextCustom(ui.mainFont, game.player2.name, p2X + (badgePWidth - p2NameWidth) / 2.0f, p2Y + (badgePHeight - p2NameHeight) / 2.0f - 3.0f, nameFontSize, WHITE);
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

    // Scale 1.5x. Sprite sheet rất lớn (1200x700) nhưng nhân vật thật chỉ chiếm
    // phần giữa — dùng heroDrawOffset để biết nhân vật thật nằm đâu trong sprite.
    // Công thức: anchor = vị trí muốn nhân vật thật đứng
    //   drawX = anchorX - (spriteW/2 + offsetX)   [offsetX âm = nhân vật lệch phải trong sprite]
    //   drawY = anchorY - spriteH - offsetY        [offsetY âm = nhân vật lệch xuống trong sprite]
    float charScale = 1.5f;

    const CharAnim& p1Anim = ui.isP1Attacking ? getAtkC(p1A, ui.p1AttackVariant)
                           : ui.isP1Dying     ? ui.heroDeath[p1A]
                           :                    ui.heroIdle[p1A];
    bool p1Active = true; // luôn sáng, spotlight phân biệt lượt
    float p1W = ui.heroDrawSize[p1A].x * charScale;
    float p1H = ui.heroDrawSize[p1A].y * charScale;
    float pedestalH = 44.0f; // chiều cao bục — nhân vật đứng trên mặt bục

    float p1AnchorX = 230.0f,  p1AnchorY = 1010.0f - pedestalH;
    float p1DrawX = p1AnchorX - p1W * 0.5f;
    float p1DrawY = p1AnchorY - p1H;

    const CharAnim& p2Anim = ui.isP2Attacking ? getAtkC(p2A, ui.p2AttackVariant)
                           : ui.isP2Dying     ? ui.heroDeath[p2A]
                           :                    ui.heroIdle[p2A];
    bool p2Active = true;
    float p2W = ui.heroDrawSize[p2A].x * charScale;
    float p2H = ui.heroDrawSize[p2A].y * charScale;
    float p2AnchorX = 1690.0f, p2AnchorY = 1010.0f - pedestalH;
    float p2DrawX = p2AnchorX - p2W * 0.5f;
    float p2DrawY = p2AnchorY - p2H;

    // === SPOTLIGHT dưới chân nhân vật — sân khấu style ===
    auto drawSpotlight = [&](float cx, float fy, bool active, Color c) {
        float pulse = 0.75f + 0.25f * sinf(glowT * 3.5f);

        // Bục hình thang +30% kích thước
        float topRx = 124.0f;  // 95 * 1.3
        float topRy = 21.0f;   // 16 * 1.3
        float botRx = 169.0f;  // 130 * 1.3
        float botRy = 29.0f;   // 22 * 1.3
        float pH    = 44.0f;   // cao hơn xíu cho cân đối
        float topY  = fy - pH;
        float pRx = topRx, pRy = topRy;

        // Màu mặt bên bục: đục như đá, tint nhẹ màu theme
        unsigned char sr = (unsigned char)((int)c.r * 30 / 100 + 55);
        unsigned char sg = (unsigned char)((int)c.g * 30 / 100 + 55);
        unsigned char sb = (unsigned char)((int)c.b * 30 / 100 + 55);
        // Màu mặt trên: sáng hơn mặt bên, vẫn đục
        unsigned char tr2 = (unsigned char)((int)c.r * 35 / 100 + 80);
        unsigned char tg2 = (unsigned char)((int)c.g * 35 / 100 + 80);
        unsigned char tb2 = (unsigned char)((int)c.b * 35 / 100 + 80);

        int segs = 80;

        // === MẶT BÊN bục (hình thang) ===
        for (int i = 0; i < segs; i++) {
            float a0 = (float)i       * 2.0f * PI / segs;
            float a1 = (float)(i + 1) * 2.0f * PI / segs;

            // Shade: phía trước (sinf > 0) sáng hơn
            float shade0 = 0.55f + 0.45f * sinf(a0);
            float shade1 = 0.55f + 0.45f * sinf(a1);
            float shadeAvg = (shade0 + shade1) * 0.5f;

            float bx0 = cx + cosf(a0) * botRx, by0 = fy    + sinf(a0) * botRy;
            float bx1 = cx + cosf(a1) * botRx, by1 = fy    + sinf(a1) * botRy;
            float tx0 = cx + cosf(a0) * topRx, ty0 = topY  + sinf(a0) * topRy;
            float tx1 = cx + cosf(a1) * topRx, ty1 = topY  + sinf(a1) * topRy;

            unsigned char cr2 = (unsigned char)(sr * (0.6f + 0.4f * shadeAvg));
            unsigned char cg2 = (unsigned char)(sg * (0.6f + 0.4f * shadeAvg));
            unsigned char cb2 = (unsigned char)(sb * (0.6f + 0.4f * shadeAvg));

            DrawTriangle({bx0,by0},{tx0,ty0},{bx1,by1}, {cr2,cg2,cb2,255});
            DrawTriangle({tx0,ty0},{tx1,ty1},{bx1,by1}, {cr2,cg2,cb2,255});
        }

        // === MẶT TRÊN bục — solid đặc ===
        DrawEllipse((int)cx, (int)topY, (int)topRx, (int)topRy, {tr2,tg2,tb2,255});
        // Highlight tâm — vùng sáng nhẹ, vẫn đục
        DrawEllipse((int)cx, (int)topY, (int)(topRx*0.55f), (int)(topRy*0.55f),
            {(unsigned char)((int)tr2+25<255?(int)tr2+25:255),
             (unsigned char)((int)tg2+25<255?(int)tg2+25:255),
             (unsigned char)((int)tb2+25<255?(int)tb2+25:255), 255});

        // === VIỀN mặt trên — glow màu theme ===
        for (int i = 0; i < segs; i++) {
            float a0 = (float)i       * 2.0f * PI / segs;
            float a1 = (float)(i + 1) * 2.0f * PI / segs;
            float ex0 = cx + cosf(a0) * topRx, ey0 = topY + sinf(a0) * topRy;
            float ex1 = cx + cosf(a1) * topRx, ey1 = topY + sinf(a1) * topRy;
            DrawLineEx({ex0,ey0},{ex1,ey1}, 5.0f, {c.r,c.g,c.b,(unsigned char)(70*pulse)});
            DrawLineEx({ex0,ey0},{ex1,ey1}, 2.0f,
                {(unsigned char)((c.r+255)/2),(unsigned char)((c.g+255)/2),(unsigned char)((c.b+255)/2),
                 (unsigned char)(220*pulse)});
        }

        // === VIỀN đáy bục ===
        for (int i = 0; i < segs; i++) {
            float a0 = (float)i       * 2.0f * PI / segs;
            float a1 = (float)(i + 1) * 2.0f * PI / segs;
            float ex0 = cx + cosf(a0) * botRx, ey0 = fy + sinf(a0) * botRy;
            float ex1 = cx + cosf(a1) * botRx, ey1 = fy + sinf(a1) * botRy;
            DrawLineEx({ex0,ey0},{ex1,ey1}, 2.0f, {sr,sg,sb,200});
        }

        // === RÃNH TRANG TRÍ ngang giữa mặt bên ===
        float midH = 0.45f;
        float rRx  = topRx + (botRx - topRx) * midH;
        float rRy  = topRy + (botRy - topRy) * midH;
        float rY   = topY  + pH * midH;
        for (int i = 0; i < segs; i++) {
            float a0 = (float)i       * 2.0f * PI / segs;
            float a1 = (float)(i + 1) * 2.0f * PI / segs;
            float lx0 = cx + cosf(a0) * rRx, ly0 = rY + sinf(a0) * rRy;
            float lx1 = cx + cosf(a1) * rRx, ly1 = rY + sinf(a1) * rRy;
            DrawLineEx({lx0,ly0},{lx1,ly1}, 3.0f, {15,15,15,200});
            DrawLineEx({lx0,ly0},{lx1,ly1}, 1.0f,
                {(unsigned char)((c.r+180)/2),(unsigned char)((c.g+180)/2),(unsigned char)((c.b+180)/2),
                 (unsigned char)(90*pulse)});
        }

        // === LỬA BAY LÊN — chỉ khi active, mô phỏng particle như menu ===
        if (active) {
            // 40 hạt ảo, mỗi hạt có seed pha riêng
            // Tổng 90 hạt: 40 từ mặt trên, 50 từ viền bên bục
            int pCount = 90;
            float speed = 0.45f;
            for (int k = 0; k < pCount; k++) {
                float phaseOffset = (float)k / pCount;
                float t = fmodf(glowT * speed + phaseOffset, 1.0f);

                float spawnX, spawnY;
                if (k < 40) {
                    // Nhóm 1: spawn trên mặt trên bục (như cũ)
                    float spawnAngle = phaseOffset * 2.0f * PI * 3.7f;
                    float spawnR = topRx * (0.25f + 0.70f * (0.5f + 0.5f * sinf(spawnAngle * 2.1f)));
                    spawnX = cx + cosf(spawnAngle) * spawnR;
                    spawnY = topY + sinf(spawnAngle) * topRy * (spawnR / topRx);
                } else {
                    // Nhóm 2: spawn trên viền bên bục — phân tán quanh botRx/midRx
                    float spawnAngle = phaseOffset * 2.0f * PI * 5.3f;
                    // Chọn ngẫu nhiên-ish giữa mặt bên (giữa top và bot)
                    float sideT = 0.3f + 0.7f * (0.5f + 0.5f * sinf(phaseOffset * 19.1f));
                    float sRx = topRx + (botRx - topRx) * sideT;
                    float sRy = topRy + (botRy - topRy) * sideT;
                    float sY  = topY  + pH * sideT;
                    spawnX = cx + cosf(spawnAngle) * sRx;
                    spawnY = sY + sinf(spawnAngle) * sRy;
                }

                // Bay lên cao bằng tướng
                float riseMax = 420.0f + 80.0f * sinf(phaseOffset * 13.7f);
                float sway = sinf(glowT * 1.8f + phaseOffset * 17.3f) * 10.0f * (1.0f - t * 0.7f);
                float px = spawnX + sway;
                float py = spawnY - t * riseMax;

                // Alpha
                float life = (t < 0.1f) ? (t / 0.1f) : (1.0f - (t - 0.1f) / 0.9f);
                life = life * life;
                unsigned char fa2 = (unsigned char)(life * 210);
                if (fa2 < 5) continue;

                float sz = (1.0f - t * 0.9f) * (4.0f + 2.5f * sinf(phaseOffset * 11.1f));

                // Màu: outer = cam/đỏ mix theme, mid = màu theme, core = trắng sáng
                // outer glow
                DrawCircle((int)px, (int)py, sz * 2.4f,
                    {c.r, (unsigned char)(c.g/3), (unsigned char)(c.b/4), (unsigned char)(fa2 * 0.35f)});
                // mid: màu theme thuần
                DrawCircle((int)px, (int)py, sz * 1.5f,
                    {c.r, c.g, c.b, fa2});
                // core: trắng pha theme
                DrawCircle((int)px, (int)py, sz * 0.7f,
                    {(unsigned char)((c.r + 255) / 2),
                     (unsigned char)((c.g + 255) / 2),
                     (unsigned char)((c.b + 255) / 2),
                     (unsigned char)(fa2 < 255 ? fa2 : 255)});
            }
        }

        // === GLOW dưới bục ===
        DrawEllipse((int)cx,(int)fy,(int)(botRx+55),(int)(botRy+9),{c.r,c.g,c.b,(unsigned char)(15*pulse)});
        DrawEllipse((int)cx,(int)fy,(int)(botRx+22),(int)(botRy+4),{c.r,c.g,c.b,(unsigned char)(38*pulse)});

        if (!active) {
            DrawEllipse((int)cx,(int)topY,(int)topRx,(int)topRy,{0,0,0,130});
        }
    };
    // fy = mặt đất thật (anchor + pedestalH = 1010)
    drawSpotlight(p1AnchorX, p1AnchorY + pedestalH - 5.0f, game.isPlayer1Turn,  p1ThemeCol);
    drawSpotlight(p2AnchorX, p2AnchorY + pedestalH - 5.0f, !game.isPlayer1Turn, p2ThemeCol);

    DrawCharAnim(p1Anim, p1DrawX, p1DrawY, p1W, p1H, false, p1Active);
    DrawCharAnim(p2Anim, p2DrawX, p2DrawY, p2W, p2H, true, p2Active);

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