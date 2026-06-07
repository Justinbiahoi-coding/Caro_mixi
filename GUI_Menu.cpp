#include "GUI_Menu.h"
#include "LogicControl.h"

const int TOTAL_MENU_ITEMS = 6;
const int TOTAL_SETTING_ITEMS = 5;

void UpdateMenuScreens(GameState& game, UIState& ui) {
    Vector2 mouse = GetMousePosition();

    // Update embers + smooth scroll mọi lúc đang ở menu
    if (ui.currentScreen == 0) {
        float dt = GetFrameTime();
        float targetY = 350.0f + ui.menuSelection * (80.0f + 35.0f);
        ui.menuScrollY += (targetY - ui.menuScrollY) * 18.0f * dt;
        for (int i = 0; i < UIState::MAX_EMBERS; i++) {
            UIState::Ember& e = ui.embers[i];
            e.x    += e.vx * 60.0f * dt;
            e.y    += e.vy * 60.0f * dt;
            e.life += dt * 0.18f;
            e.alpha = (1.0f - e.life) * 0.85f;
            // Spawn lại khi hết tuổi thọ
            if (e.life >= 1.0f) {
                e.x    = (float)(rand() % 1920);
                e.y    = 1080.0f + (float)(rand() % 60);
                e.vx   = ((rand() % 100) - 50) * 0.006f;
                e.vy   = -((rand() % 40) + 25) * 0.014f;
                e.size = (float)(rand() % 3 + 2);
                e.life = 0.0f;
            }
        }
    }

    if (ui.currentScreen == 0) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            ui.menuSelection--;
            if (ui.menuSelection < 0) ui.menuSelection = TOTAL_MENU_ITEMS - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            ui.menuSelection++;
            if (ui.menuSelection >= TOTAL_MENU_ITEMS) ui.menuSelection = 0;
        }

        float btnW = 800.0f; // Mở rộng vùng chọn siêu to khổng lồ
        float btnH = 80.0f;
        float centerX = 1500.0f; 
        float startY = 350.0f;
        float gap = btnH + 35.0f;

        Rectangle rects[TOTAL_MENU_ITEMS];
        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            rects[i] = { centerX - btnW/2.0f, startY + i*gap, btnW, btnH };
        }

        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            if (CheckCollisionPointRec(mouse, rects[i])) ui.menuSelection = i; 
        }

        bool confirmSelection = false;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, rects[ui.menuSelection])) confirmSelection = true;
        if (IsKeyPressed(KEY_ENTER)) confirmSelection = true;

        if (confirmSelection) {
            const char* helpText[] = {
                "CARO BATTLE - LUAT CHOI",
                "- Hai ben luan phien danh X va O.",
                "- Ben nao co 5 quan lien tiep se thang.",
                "- Trong Booming Caro, moi ben co HP va Ky Nang.",
                "- Chuc ban choi vui ve!"
            };        
            switch (ui.menuSelection) {
                case 0: { 
                    ui.currentScreen = 8; 
                    ui.p1HeroSelection = 0;
                    ui.p2HeroSelection = 0; 
                    ui.selectionPhase = -1; 
                    
                    ui.p1NameInput[0] = '\0'; ui.p1LetterCount = 0;
                    ui.p2NameInput[0] = '\0'; ui.p2LetterCount = 0;
                } break;
                case 1: ui.currentScreen = 5; ui.loadSelection = 0; break;
                case 2: ui.currentScreen = 2; ui.settingSelection = game.inputType; ui.draggingVolume = false; break;
                case 3: break;
                case 4: ui.currentScreen = 3; break;
                case 5: ui.shouldExit = true; break;
            }
        }
    }
   
    else if (ui.currentScreen == 2) {
        Vector2 mouse = GetMousePosition();

        const int barWidth = 400;
        const int barHeight = 24;
        const int barX = (1920 - barWidth) / 2;
        const int barY = 350 + 2 * 80 + 60;

        // Tăng vùng nhận diện click cho dễ nắm kéo
        Rectangle volumeHitbox = { (float)barX - 20, (float)barY - 20, (float)barWidth + 40, (float)barHeight + 40 };

        if (CheckCollisionPointRec(mouse, volumeHitbox)) {
            ui.settingSelection = -1; // Ẩn highlight khi rê chuột lên thanh volume
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, volumeHitbox)) {
                ui.draggingVolume = true;
            }
        }

        if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            ui.draggingVolume = false;
        }

        if (ui.draggingVolume) {
            float t = (mouse.x - barX) / (float)barWidth;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            ui.musicVolume = t;
            SetMusicVolume(ui.bgMusic, ui.musicVolume);
            
            ui.settingSelection = -1; // Đảm bảo ẩn highlight khi đang kéo volume
        }

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            if (ui.settingSelection == -1) ui.settingSelection = 3; // Nếu đang trỏ volume, nhảy về nút dưới
            else {
                ui.settingSelection = (ui.settingSelection - 1 + TOTAL_SETTING_ITEMS) % TOTAL_SETTING_ITEMS;
                if (ui.settingSelection == 2) ui.settingSelection = 1;
            }
        }

        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            if (ui.settingSelection == -1) ui.settingSelection = 3;
            else {
                ui.settingSelection = (ui.settingSelection + 1) % TOTAL_SETTING_ITEMS;
                if (ui.settingSelection == 2) ui.settingSelection = 3;
            }
        }

    const char* menuOptions[TOTAL_MENU_ITEMS] = {
        "NEW GAME",
        "LOAD GAME",
        "SETTINGS",
        "HELP",
        "CREDITS",
        "EXIT"
    };
        const char* setOptions[TOTAL_SETTING_ITEMS] = {
            "Mouse Control (Recommended)",
            "Keyboard Control (WASD + Enter)",
            "Music Volume",
            "Toggle Music",
            "Back to Menu"
        };

        Rectangle setRects[TOTAL_SETTING_ITEMS];
        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            if (i == 2) continue; // Không cho phép chọn "Am Luong Nhac"
            int yPos = 350 + i * 80;
            if (i >= 3) yPos += 40;
            int textWidth = MeasureTextCustomX(ui.mainFont, setOptions[i], 40);
            int xPos = (1920 - textWidth) / 2;
            setRects[i] = { (float)(xPos - 40), (float)(yPos - 10), (float)(textWidth + 80), 60 };

            if (!ui.draggingVolume && CheckCollisionPointRec(mouse, setRects[i]))
                ui.settingSelection = i;
        }

        if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, setRects[ui.settingSelection])
            && !ui.draggingVolume) || IsKeyPressed(KEY_ENTER)) {
            if (ui.settingSelection == 0) game.inputType = 0;
            else if (ui.settingSelection == 1) game.inputType = 1;
            else if (ui.settingSelection == 3) {
                ui.musicEnabled = !ui.musicEnabled;
                if (ui.musicEnabled) PlayMusicStream(ui.bgMusic);
                else PauseMusicStream(ui.bgMusic);
            }
            else if (ui.settingSelection == 4) {
                ui.currentScreen = 0;
            }
        }

        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE))
            ui.currentScreen = 0;
    }
    else if (ui.currentScreen == 3) {
        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0;
    }
    else if (ui.currentScreen == 5) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            ui.loadSelection--;
            if (ui.loadSelection < 0) ui.loadSelection = 3;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            ui.loadSelection++;
            if (ui.loadSelection > 3) ui.loadSelection = 0;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (LoadGameSlot(game, ui.loadSelection)) ui.currentScreen = 1; 
        }
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
            DeleteGameSlot(ui.loadSelection);
        }
        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0; 
    }
    else if (ui.currentScreen == 6) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (ui.letterCount < 29)) {
                ui.nameInput[ui.letterCount] = (char)key;
                ui.nameInput[ui.letterCount+1] = '\0';
                ui.letterCount++;
            }
            key = GetCharPressed(); 
        }

        if (IsKeyPressed(KEY_BACKSPACE) && ui.letterCount > 0) {
            ui.letterCount--;
            ui.nameInput[ui.letterCount] = '\0';
        }
        if (IsKeyPressed(KEY_UP)) {
            ui.saveSelection--;
            if (ui.saveSelection < 0) ui.saveSelection = 3;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            ui.saveSelection++;
            if (ui.saveSelection > 3) ui.saveSelection = 0;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (ui.letterCount == 0) SaveGameSlot(game, ui.saveSelection, "Chua Dat Ten");
            else SaveGameSlot(game, ui.saveSelection, ui.nameInput);
            ui.currentScreen = 1; 
        }
        if (IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 1; 
    }
    else if (ui.currentScreen == 7) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125)) {
                if (ui.activeInputField == 0 && ui.p1LetterCount < 10) {
                    ui.p1NameInput[ui.p1LetterCount] = (char)key;
                    ui.p1NameInput[ui.p1LetterCount+1] = '\0';
                    ui.p1LetterCount++;
                } else if (ui.activeInputField == 1 && ui.p2LetterCount < 10) {
                    ui.p2NameInput[ui.p2LetterCount] = (char)key;
                    ui.p2NameInput[ui.p2LetterCount+1] = '\0';
                    ui.p2LetterCount++;
                }
            }
            key = GetCharPressed(); 
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (ui.activeInputField == 0 && ui.p1LetterCount > 0) {
                ui.p1LetterCount--;
                ui.p1NameInput[ui.p1LetterCount] = '\0';
            } else if (ui.activeInputField == 1 && ui.p2LetterCount > 0) {
                ui.p2LetterCount--;
                ui.p2NameInput[ui.p2LetterCount] = '\0';
            }
        }

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_TAB)) {
            ui.activeInputField = 1 - ui.activeInputField; 
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (ui.activeInputField == 0) {
                ui.activeInputField = 1;
            } else {
                int savedInput = game.inputType;
                InitGame(game, 0); 
                game.inputType = savedInput;
                
                if (ui.p1LetterCount > 0) strcpy(game.player1.name, ui.p1NameInput);
                else strcpy(game.player1.name, "Player 1");
                
                if (ui.p2LetterCount > 0) strcpy(game.player2.name, ui.p2NameInput);
                else strcpy(game.player2.name, "Player 2");

                ui.currentScreen = 1; 
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0; 
    }
    else if (ui.currentScreen == 8) {
        // Animate all hero idle sprites (indices 0-5)
        float dt8 = GetFrameTime();
        for (int i = 0; i < 6; i++) {
            CharAnim& anim = ui.heroIdle[i];
            if (anim.frameCount <= 0) continue;
            anim.frameTimer += dt8;
            if (anim.frameTimer >= anim.frameDuration) {
                anim.currentFrame = (anim.currentFrame + 1) % anim.frameCount;
                anim.frameTimer = 0.0f;
            }
        }

        const int MAX_HEROES = 5; // black_knight hidden (index 0 reserved)
        // Maps selection index (0,1,2,3,4) → asset index (1,2,3,4,5) skipping black_knight
        const int heroMap[5] = {1, 2, 3, 4, 5};

        if (ui.selectionPhase == -1) {
            // Chon che do choi: VS Player hoac VS Bot
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) game.isVsBot = false;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) game.isVsBot = true;
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 0;
            if (IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0;
        }
        else if (ui.selectionPhase == 0) {
            // p1 pick hero
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) ui.p1HeroSelection = (ui.p1HeroSelection - 1 + MAX_HEROES) % MAX_HEROES;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) ui.p1HeroSelection = (ui.p1HeroSelection + 1) % MAX_HEROES;
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 1;
            if (IsKeyPressed(KEY_ESCAPE)) ui.selectionPhase = -1;
        }
        else if (ui.selectionPhase == 1) {
            // p1 set name
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && ui.p1LetterCount < 10) {
                    ui.p1NameInput[ui.p1LetterCount] = (char)key;
                    ui.p1NameInput[ui.p1LetterCount+1] = '\0';
                    ui.p1LetterCount++;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && ui.p1LetterCount > 0) {
                ui.p1LetterCount--;
                ui.p1NameInput[ui.p1LetterCount] = '\0';
            }
            if (IsKeyPressed(KEY_ESCAPE)) ui.selectionPhase = 0;
            else if (IsKeyPressed(KEY_ENTER)) {
                // Hero names (same order as heroMap: asset 1-5)
                const char* heroNamesUpd[5] = {"Fire Knight", "Green Archer", "Earth Assassin", "Metal Blade", "Water Mage"};
                if (game.isVsBot) {
                    // Random bot hero: pick from 0-4 excluding p1HeroSelection
                    int botIdx = rand() % 4; // 4 remaining choices
                    if (botIdx >= ui.p1HeroSelection) botIdx++; // skip p1's slot
                    ui.p2HeroSelection = botIdx;

                    int savedInput = game.inputType;
                    InitGame(game, 0);
                    game.inputType = savedInput;

                    // P1 name: typed or "P1 - HeroName"
                    if (ui.p1LetterCount > 0)
                        strcpy(game.player1.name, ui.p1NameInput);
                    else {
                        snprintf(game.player1.name, sizeof(game.player1.name),
                            "P1 - %s", heroNamesUpd[ui.p1HeroSelection]);
                    }
                    // BOT name: "BOT - HeroName"
                    snprintf(game.player2.name, sizeof(game.player2.name),
                        "BOT - %s", heroNamesUpd[ui.p2HeroSelection]);

                    ui.currentScreen = 1;
                } else {
                    ui.selectionPhase = 2;
                }
            }
        }
        else if (ui.selectionPhase == 2) {
            // p2 pick hero
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) ui.p2HeroSelection = (ui.p2HeroSelection - 1 + MAX_HEROES) % MAX_HEROES;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) ui.p2HeroSelection = (ui.p2HeroSelection + 1) % MAX_HEROES;
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 3;
            if (IsKeyPressed(KEY_ESCAPE)) ui.selectionPhase = 1;
        }
        else if (ui.selectionPhase == 3) {
            // p2 set name
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && ui.p2LetterCount < 10) {
                    ui.p2NameInput[ui.p2LetterCount] = (char)key;
                    ui.p2NameInput[ui.p2LetterCount+1] = '\0';
                    ui.p2LetterCount++;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && ui.p2LetterCount > 0) {
                ui.p2LetterCount--;
                ui.p2NameInput[ui.p2LetterCount] = '\0';
            }
            if (IsKeyPressed(KEY_ESCAPE)) ui.selectionPhase = 2;
            else if (IsKeyPressed(KEY_ENTER)) {
                const char* heroNamesUpd[5] = {"Fire Knight", "Green Archer", "Earth Assassin", "Metal Blade", "Water Mage"};
                int savedInput = game.inputType;
                InitGame(game, 0);
                game.inputType = savedInput;
                // P1 name: typed or "P1 - HeroName"
                if (ui.p1LetterCount > 0)
                    strcpy(game.player1.name, ui.p1NameInput);
                else
                    snprintf(game.player1.name, sizeof(game.player1.name),
                        "P1 - %s", heroNamesUpd[ui.p1HeroSelection]);
                // P2 name: typed or "P2 - HeroName"
                if (ui.p2LetterCount > 0)
                    strcpy(game.player2.name, ui.p2NameInput);
                else
                    snprintf(game.player2.name, sizeof(game.player2.name),
                        "P2 - %s", heroNamesUpd[ui.p2HeroSelection]);
                ui.currentScreen = 1;
            }
        }

        // ── Update char select particles ──
        {
            const float cardW   = 320.0f;
            const float cardH   = 510.0f;
            const float cardGap = 24.0f;
            const float totalCW = 5*cardW + 4*cardGap;
            const float cardStartX = (1920.0f - totalCW) / 2.0f;
            const float cardY      = 310.0f;
            int curSel = (ui.selectionPhase <= 1) ? ui.p1HeroSelection : ui.p2HeroSelection;
            float dt = GetFrameTime();

            // Spawn new particles for selected card
            int spawnSlot = -1;
            for (int k = 0; k < UIState::MAX_CHAR_PARTICLES; k++) {
                if (ui.charParticles[k].life <= 0.0f) { spawnSlot = k; break; }
            }
            // Spawn ~2 per frame for selected card
            for (int spawn = 0; spawn < 2 && spawnSlot >= 0; spawn++) {
                float cx = cardStartX + curSel * (cardW + cardGap);
                float footY = cardY + cardH * 0.88f - (curSel == (int)curSel ? 16.0f : 0.0f);
                auto& p = ui.charParticles[spawnSlot];
                p.heroID = curSel;
                p.life   = 1.0f;
                p.type   = (rand() % 5 == 0) ? 1 : 0; // 20% rune, 80% ember
                if (p.type == 0) {
                    // Rising ember from foot area
                    p.x     = cx + cardW * 0.2f + (rand() % (int)(cardW * 0.6f));
                    p.y     = footY - (rand() % 30);
                    p.vx    = ((rand() % 60) - 30) * 0.01f;
                    p.vy    = -((rand() % 50) + 30) * 0.012f;
                    p.size  = (float)(rand() % 4 + 2);
                    p.alpha = 0.7f + (rand() % 30) / 100.0f;
                    p.angle = 0.0f; p.angleV = 0.0f;
                } else {
                    // Rune diamond floating around card border
                    int side = rand() % 4;
                    if (side == 0) { p.x = cx + rand() % (int)cardW; p.y = cardY; }
                    else if (side == 1) { p.x = cx + cardW; p.y = cardY + rand() % (int)cardH; }
                    else if (side == 2) { p.x = cx + rand() % (int)cardW; p.y = cardY + cardH; }
                    else { p.x = cx; p.y = cardY + rand() % (int)cardH; }
                    p.vx    = ((rand() % 40) - 20) * 0.008f;
                    p.vy    = -((rand() % 30) + 10) * 0.010f;
                    p.size  = (float)(rand() % 5 + 4);
                    p.alpha = 0.5f + (rand() % 40) / 100.0f;
                    p.angle = (float)(rand() % 360);
                    p.angleV = ((rand() % 60) - 30) * 0.05f;
                }
                // find next free slot
                spawnSlot = -1;
                for (int k = spawnSlot + 1; k < UIState::MAX_CHAR_PARTICLES; k++) {
                    if (ui.charParticles[k].life <= 0.0f) { spawnSlot = k; break; }
                }
            }

            // Update all particles
            for (int k = 0; k < UIState::MAX_CHAR_PARTICLES; k++) {
                auto& p = ui.charParticles[k];
                if (p.life <= 0.0f) continue;
                p.life  -= dt * 0.55f;
                p.x     += p.vx;
                p.y     += p.vy;
                p.vx    *= 0.995f;
                p.angle += p.angleV;
                p.alpha  = p.life * 0.9f;
                if (p.life < 0.0f) p.life = 0.0f;
            }
        }
    }
}

void DrawMenuScreens(const GameState& game, const UIState& ui) {
    if (ui.currentScreen == 0) {
        DrawTexturePro(ui.bgMenu, { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

        // Lớp gradient ma mị đổ từ phải qua
        DrawRectangleGradientH(950, 0, 970, 1080, Fade(BLACK, 0.0f), Fade(BLACK, 0.85f));

        // Vẽ Tên Game phía trên Menu
        const char* titleText = "CARO BATTLE";
        int titleSize = 120;
        int titleW = MeasureTextCustomX(ui.mainFont, titleText, titleSize);
        int titleY = 130;

        float t = (float)GetTime();
        float pulse = 0.5f + 0.5f * sinf(t * 2.0f); // dao động 0.0 → 1.0
        Color goldColor2 = { 255, 200, 50, 255 };
        float tx0 = 1500.0f - titleW / 2.0f;

        // Bóng đen
        DrawTextCustom(ui.mainFont, titleText, tx0 + 6, titleY + 6, titleSize, Fade(BLACK, 0.9f));
        // Glow đỏ loang rộng (pulse mạnh)
        for (int d = 1; d <= 6; d++)
            DrawTextCustom(ui.mainFont, titleText, tx0, titleY, titleSize, Fade(RED, pulse * 0.13f));
        // Glow vàng loang (pulse nhẹ hơn, luôn có)
        for (int d = 2; d <= 5; d++) {
            DrawTextCustom(ui.mainFont, titleText, tx0 - d, titleY,     titleSize, Fade(goldColor2, 0.18f + pulse * 0.12f));
            DrawTextCustom(ui.mainFont, titleText, tx0 + d, titleY,     titleSize, Fade(goldColor2, 0.18f + pulse * 0.12f));
            DrawTextCustom(ui.mainFont, titleText, tx0,     titleY - d, titleSize, Fade(goldColor2, 0.18f + pulse * 0.12f));
            DrawTextCustom(ui.mainFont, titleText, tx0,     titleY + d, titleSize, Fade(goldColor2, 0.18f + pulse * 0.12f));
        }
        // Chữ chính vàng sáng
        DrawTextCustom(ui.mainFont, titleText, tx0, titleY, titleSize, goldColor2);

        // --- PARTICLES: tàn lửa bay lên ---
        for (int i = 0; i < UIState::MAX_EMBERS; i++) {
            const UIState::Ember& e = ui.embers[i];
            if (e.alpha <= 0.0f) continue;
            Color emberColor = Fade((Color){255, 160, 40, 255}, e.alpha);
            DrawCircleV({e.x, e.y}, e.size, emberColor);
            DrawCircleV({e.x, e.y}, e.size * 0.45f, Fade(WHITE, e.alpha * 0.7f));
        }

        // --- SMOOTH HIGHLIGHT BAR ---
        const float menuBlockX = 1500.0f;
        const float btnWidth   = 800.0f;
        const float btnHeight  = 80.0f;
        const float startY2    = 350.0f;
        const float gap2       = btnHeight + 35.0f;
        float targetY = startY2 + ui.menuSelection * gap2;

        // Highlight bar — nhiều lớp mỏng chồng nhau tạo cảm giác blur/glow
        float hy = ui.menuScrollY + btnHeight / 2.0f; // tâm dọc của item
        float hw = btnWidth;
        Color gc = {255, 200, 50, 255};
        // Lớp ngoài cùng — rất rộng, rất mờ
        DrawRectangleGradientH((int)(menuBlockX - hw/2), (int)(hy - 28), (int)(hw/2), 56,
            Fade(gc, 0.0f), Fade(gc, 0.06f));
        DrawRectangleGradientH((int)(menuBlockX),        (int)(hy - 28), (int)(hw/2), 56,
            Fade(gc, 0.06f), Fade(gc, 0.0f));
        // Lớp giữa — vừa
        DrawRectangleGradientH((int)(menuBlockX - hw/2 + 60), (int)(hy - 16), (int)(hw/2 - 60), 32,
            Fade(gc, 0.0f), Fade(gc, 0.12f));
        DrawRectangleGradientH((int)(menuBlockX),               (int)(hy - 16), (int)(hw/2 - 60), 32,
            Fade(gc, 0.12f), Fade(gc, 0.0f));
        // Lớp trong — mỏng, sáng nhất ở tâm
        DrawRectangleGradientH((int)(menuBlockX - hw/2 + 120), (int)(hy - 6), (int)(hw/2 - 120), 12,
            Fade(gc, 0.0f), Fade(gc, 0.22f));
        DrawRectangleGradientH((int)(menuBlockX),                (int)(hy - 6), (int)(hw/2 - 120), 12,
            Fade(gc, 0.22f), Fade(gc, 0.0f));

        const char* menuOptions[TOTAL_MENU_ITEMS] = {
            "NEW GAME",
            "LOAD GAME",
            "SETTINGS",
            "HELP",
            "CREDITS",
            "EXIT"
        };
        
        float centerX = 1500.0f;
        float startY = 350.0f;
        float gap = btnHeight + 35.0f;

        Color goldColor = { 255, 200, 50, 255 };

        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            Rectangle rect = { centerX - btnWidth/2.0f, startY + gap*i, btnWidth, btnHeight };
            bool isSelected = (ui.menuSelection == i);

            int fontSize     = isSelected ? 52 : 42;
            int textW = MeasureTextCustomX(ui.mainFont, menuOptions[i], fontSize);
            int textH = MeasureTextCustomY(ui.mainFont, menuOptions[i], fontSize);
            float tx = rect.x + (btnWidth - textW) / 2.0f;
            float ty = rect.y + (btnHeight - textH) / 2.0f;

            if (isSelected) {
                // glow vàng loang mạnh ra 4 phía
                for (int d = 1; d <= 5; d++) {
                    float a = 0.35f - d * 0.06f;
                    DrawTextCustom(ui.mainFont, menuOptions[i], tx - d, ty,     fontSize, Fade(goldColor, a));
                    DrawTextCustom(ui.mainFont, menuOptions[i], tx + d, ty,     fontSize, Fade(goldColor, a));
                    DrawTextCustom(ui.mainFont, menuOptions[i], tx,     ty - d, fontSize, Fade(goldColor, a));
                    DrawTextCustom(ui.mainFont, menuOptions[i], tx,     ty + d, fontSize, Fade(goldColor, a));
                }
                // mũi tên 2 bên
                DrawTextCustom(ui.mainFont, ">", tx - 45, ty, fontSize, goldColor);
                DrawTextCustom(ui.mainFont, "<", tx + textW + 15, ty, fontSize, goldColor);
                // chữ chính vàng sáng
                DrawTextCustom(ui.mainFont, menuOptions[i], tx, ty, fontSize, goldColor);
            } else {
                DrawTextCustom(ui.mainFont, menuOptions[i], tx, ty, fontSize, Fade(LIGHTGRAY, 0.55f));
            }
        }
    } 
    else if (ui.currentScreen == 2) {
        DrawTexturePro(ui.bgSettings, { 0, 0, (float)ui.bgSettings.width, (float)ui.bgSettings.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

        // Nền tối toàn màn hình thay vì khung chữ nhật thô
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.75f));

        Color selectedColor = { 255, 180, 0, 255 }; // Gothic Gold

        // Hai thanh ngang trang trí (Top & Bottom)
        auto DrawOrnateLine = [](float x, float y, float width, Color c) {
            // Main horizontal lines (Double line effect)
            DrawLineEx({x + 40, y}, {x + width - 40, y}, 4.0f, c);
            DrawLineEx({x + 40, y - 6}, {x + width - 40, y - 6}, 1.0f, Fade(c, 0.6f));
            DrawLineEx({x + 40, y + 6}, {x + width - 40, y + 6}, 1.0f, Fade(c, 0.6f));
            
            auto DrawDiamond = [](float cx, float cy, float w, float h, Color color) {
                Vector2 top = {cx, cy - h/2};
                Vector2 left = {cx - w/2, cy};
                Vector2 right = {cx + w/2, cy};
                Vector2 bottom = {cx, cy + h/2};
                DrawTriangle(left, bottom, right, color);
                DrawTriangle(left, right, top, color);
            };

            auto DrawCross = [](float cx, float cy, float size, Color color) {
                DrawRectangle(cx - size/6, cy - size/2, size/3, size, color);
                DrawRectangle(cx - size/2, cy - size/6, size, size/3, color);
            };

            // Center Medallion
            float cx = x + width / 2;
            DrawDiamond(cx, y, 40, 40, c);
            DrawDiamond(cx, y, 30, 30, BLACK);
            DrawDiamond(cx, y, 16, 16, c);
            DrawCross(cx, y, 12, BLACK); // Inner cross

            // Repeated tiny diamonds along the line
            int segments = 12;
            for(int i = 1; i < segments; i++) {
                float px = x + 40 + i * ((width - 80) / segments);
                if (abs(px - cx) > 50) { // Don't draw over center medallion
                    DrawDiamond(px, y, 10, 10, c);
                    DrawCircle(px, y, 2.0f, BLACK); // Small hole
                }
            }

            // Elaborate End Caps (Left)
            DrawDiamond(x + 20, y, 30, 30, c);
            DrawDiamond(x + 20, y, 16, 16, BLACK);
            DrawCross(x + 20, y, 10, c);
            DrawDiamond(x, y, 14, 14, c);
            
            // Elaborate End Caps (Right)
            DrawDiamond(x + width - 20, y, 30, 30, c);
            DrawDiamond(x + width - 20, y, 16, 16, BLACK);
            DrawCross(x + width - 20, y, 10, c);
            DrawDiamond(x + width, y, 14, 14, c);
        };

        DrawOrnateLine(1920/2 - 600, 100, 1200, selectedColor);
        DrawOrnateLine(1920/2 - 600, 850, 1200, selectedColor);

        // Tiêu đề SETTINGS ma mị
        const char* titleText = "SETTINGS";
        int titleSize = 100;
        int titleW = MeasureTextCustomX(ui.mainFont, titleText, titleSize);
        int titleY = 150;
        
        DrawTextCustom(ui.mainFont, titleText, (1920 - titleW)/2 + 6, titleY + 6, titleSize, Fade(BLACK, 0.8f));
        DrawTextCustom(ui.mainFont, titleText, (1920 - titleW)/2, titleY, titleSize, Fade(RED, 0.6f));
        DrawTextCustom(ui.mainFont, titleText, (1920 - titleW)/2, titleY, titleSize, (Color){ 255, 200, 50, 255 });

        const char* setOptions[TOTAL_SETTING_ITEMS] = {
          "Mouse Control (Recommended)",
          "Keyboard Control (WASD + Enter)",
          "Music Volume",
          "Toggle Music",
          "Back to Menu"
        };

        Vector2 mouse = GetMousePosition();

        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            int yPos = 350 + i * 80;
            if (i >= 3) yPos += 40; // Khoảng cách cho thanh âm lượng
            
            Color textColor = LIGHTGRAY;
            if (i == ui.settingSelection) textColor = selectedColor;

            // Highlight the currently active input mode
            if (i == 0 && game.inputType == 0) textColor = WHITE;
            if (i == 1 && game.inputType == 1) textColor = WHITE;
            if (i == ui.settingSelection) textColor = selectedColor;

            char displayText[256];
            if (i == 3) { 
                const char* status = ui.musicEnabled ? " [ON]" : " [OFF]";
                snprintf(displayText, sizeof(displayText), "%s%s", setOptions[i], status);
            } else {
                snprintf(displayText, sizeof(displayText), "%s", setOptions[i]);
            }
            if (i == 0 && game.inputType == 0) snprintf(displayText, sizeof(displayText), "%s [ACTIVE]", setOptions[i]);
            if (i == 1 && game.inputType == 1) snprintf(displayText, sizeof(displayText), "%s [ACTIVE]", setOptions[i]);

            if (i == ui.settingSelection) {
                // Thêm ngoặc nhọn nếu đang được chọn
                char selectedText[256];
                snprintf(selectedText, sizeof(selectedText), "> %s <", displayText);
                int w = MeasureTextCustomX(ui.mainFont, selectedText, 40);
                DrawTextCustom(ui.mainFont, selectedText, (1920 - w)/2, yPos, 40, textColor);
            } else {
                int w = MeasureTextCustomX(ui.mainFont, displayText, 40);
                DrawTextCustom(ui.mainFont, displayText, (1920 - w)/2, yPos, 40, textColor);
            }
        }

        const int barWidth = 400;
        const int barHeight = 24;
        const int barX = (1920 - barWidth) / 2;
        const int barY = 350 + 2 * 80 + 60;
        
        Rectangle volumeBar = { (float)barX, (float)barY, (float)barWidth, (float)barHeight };
        DrawRectangleRec(volumeBar, DARKGRAY);
        DrawRectangle(barX, barY, (int)(barWidth * ui.musicVolume), barHeight, MAROON);
        
        Rectangle knob = {
            barX + ui.musicVolume * barWidth - 8.0f,
            (float)barY - 12.0f,
            16.0f,
            48.0f
        };
        DrawRectangleRec(knob, selectedColor);

        DrawTextCustom(ui.mainFont, TextFormat("%i%%", (int)(ui.musicVolume * 100)), barX + barWidth + 20, barY - 5, 30, WHITE);
    }
   else if (ui.currentScreen == 5) {
        DrawTexturePro(ui.bgLoadGame, { 0, 0, (float)ui.bgLoadGame.width, (float)ui.bgLoadGame.height },
            { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.52f));
        // Vignette — tối 4 cạnh, sáng giữa
        DrawRectangleGradientH(0, 0, 500, 1080, Fade(BLACK, 0.55f), Fade(BLACK, 0.0f));
        DrawRectangleGradientH(1420, 0, 500, 1080, Fade(BLACK, 0.0f), Fade(BLACK, 0.55f));
        DrawRectangleGradientV(0, 0, 1920, 160, Fade(BLACK, 0.50f), Fade(BLACK, 0.0f));
        DrawRectangleGradientV(0, 880, 1920, 200, Fade(BLACK, 0.0f), Fade(BLACK, 0.70f));

        float t = (float)GetTime();
        float glowPulse = 0.5f + 0.5f * sinf(t * 2.0f);

        Color gothicGold  = { 255, 180,   0, 255 };
        Color accentDim   = { 160, 110,   0, 255 };
        Color redAccent   = { 200,  60,  30, 255 };
        Color silverColor = { 220, 210, 195, 255 };
        Color dimColor    = { 150, 130, 100, 255 };
        Color warmWhite   = { 255, 245, 220, 255 };

        // ── DrawOrnateLine ──
        auto DD5 = [](float cx, float cy, float w, float h, Color col) {
            DrawTriangle({cx-w/2,cy},{cx,cy+h/2},{cx+w/2,cy},col);
            DrawTriangle({cx-w/2,cy},{cx+w/2,cy},{cx,cy-h/2},col);
        };
        auto DC5 = [](float cx, float cy, float sz, Color col) {
            DrawRectangle((int)(cx-sz/6),(int)(cy-sz/2),(int)(sz/3),(int)sz,col);
            DrawRectangle((int)(cx-sz/2),(int)(cy-sz/6),(int)sz,(int)(sz/3),col);
        };
        auto DrawOrnateLine5 = [&](float x, float y, float width, Color c) {
            DrawLineEx({x+40,y},{x+width-40,y},4.0f,c);
            DrawLineEx({x+40,y-6},{x+width-40,y-6},1.0f,Fade(c,0.6f));
            DrawLineEx({x+40,y+6},{x+width-40,y+6},1.0f,Fade(c,0.6f));
            float cx=x+width/2;
            DD5(cx,y,40,40,c); DD5(cx,y,30,30,BLACK); DD5(cx,y,16,16,c); DC5(cx,y,12,BLACK);
            for(int s=1;s<12;s++){float px=x+40+s*((width-80)/12);if(fabsf(px-cx)>50){DD5(px,y,10,10,c);DrawCircle((int)px,(int)y,2.0f,BLACK);}}
            DD5(x+20,y,30,30,c);DD5(x+20,y,16,16,BLACK);DC5(x+20,y,10,c);DD5(x,y,14,14,c);
            DD5(x+width-20,y,30,30,c);DD5(x+width-20,y,16,16,BLACK);DC5(x+width-20,y,10,c);DD5(x+width,y,14,14,c);
        };

        // ── Top ornament line ──
        DrawOrnateLine5(80, 92, 1760, gothicGold);

        // ── Title ──
        const char* title = "LOAD GAME";
        int titleW = MeasureTextCustomX(ui.mainFont, title, 80);
        DrawTextCustom(ui.mainFont, title, 1920/2 - titleW/2 + 3, 108, 80, Fade(BLACK, 0.9f));
        DrawTextCustom(ui.mainFont, title, 1920/2 - titleW/2,     104, 80, gothicGold);

        // ── Layout constants ──
        const float listX  = 80.0f;
        const float listY  = 210.0f;
        const float listW  = 620.0f;
        const float slotH  = 170.0f;
        const float slotGap = 10.0f;
        float panelH = 4 * slotH + 3 * slotGap;

        // ── Left panel: slot list ──
        for (int i = 0; i < 4; i++) {
            float sy = listY + i * (slotH + slotGap);
            bool isSelected = (i == ui.loadSelection);
            GameState tempGame;
            bool hasData = PeekGameSlot(i, tempGame);

            // Selected: glow background + border
            if (isSelected) {
                // Nền gradient ấm
                DrawRectangleGradientH((int)listX, (int)sy+1, (int)listW, (int)slotH-1,
                    Fade(gothicGold, 0.18f), Fade(BLACK, 0.0f));
                // Border vàng quanh slot selected
                DrawRectangleLinesEx({listX, sy, listW, slotH}, 2.0f, Fade(gothicGold, 0.70f));
                // Left accent bar sáng
                DrawRectangle((int)listX, (int)sy+1, 5, (int)slotH-1, Fade(gothicGold, 0.95f));
                // Outer glow pulse
                DrawRectangleLinesEx({listX-3, sy-3, listW+6, slotH+6}, 1.5f,
                    Fade(gothicGold, 0.25f * glowPulse));
            }

            // Top separator
            Color lineCol = isSelected ? Fade(gothicGold, 0.85f) : Fade(dimColor, 0.35f);
            DrawLineEx({listX, sy}, {listX + listW, sy}, isSelected ? 2.5f : 1.0f, lineCol);

            // Diamond bullet
            {
                float dcx = listX + 10, dcy = sy, dw = isSelected ? 16.0f : 10.0f, dh = isSelected ? 16.0f : 10.0f;
                Color dc = isSelected ? gothicGold : Fade(dimColor, 0.5f);
                DrawTriangle({dcx-dw/2,dcy},{dcx,dcy+dh/2},{dcx+dw/2,dcy},dc);
                DrawTriangle({dcx-dw/2,dcy},{dcx+dw/2,dcy},{dcx,dcy-dh/2},dc);
            }

            // Slot number label (top right) — vàng rõ hơn
            const char* slotLabel = TextFormat("SLOT %d", i + 1);
            int slW = MeasureTextCustomX(ui.mainFont, slotLabel, 24);
            DrawTextCustom(ui.mainFont, slotLabel, (int)(listX + listW - slW - 12), (int)sy + 8, 24,
                isSelected ? gothicGold : Fade(gothicGold, 0.55f));

            if (hasData) {
                // ">" + save name — large
                Color nameCol = isSelected ? warmWhite : Fade(warmWhite, 0.75f);
                int nameSize  = isSelected ? 50 : 44;
                if (isSelected) {
                    DrawTextCustom(ui.mainFont, ">", (int)listX + 18, (int)sy + 34, 44, Fade(gothicGold, 0.9f));
                    DrawTextCustom(ui.mainFont, tempGame.saveName, (int)listX + 62, (int)sy + 34, nameSize,
                        Fade(BLACK, 0.6f));
                }
                DrawTextCustom(ui.mainFont, tempGame.saveName,
                    (int)listX + (isSelected ? 62 : 26), (int)sy + (isSelected ? 32 : 34), nameSize, nameCol);

                // Mode badge — colored, đậm hơn khi không selected
                const char* modeShort = (tempGame.gameMode == 0) ? "Classic" : "Booming";
                Color modeCol = (tempGame.gameMode == 0)
                    ? (isSelected ? gothicGold : Fade(gothicGold, 0.70f))
                    : (isSelected ? redAccent  : Fade(redAccent,  0.75f));
                DrawTextCustom(ui.mainFont, modeShort, (int)listX + 26, (int)(sy + slotH - 50), 30, modeCol);

                // Round (bottom right) — silverColor đậm hơn
                const char* roundStr = TextFormat("Round %d", tempGame.roundCount);
                int rw = MeasureTextCustomX(ui.mainFont, roundStr, 26);
                DrawTextCustom(ui.mainFont, roundStr, (int)(listX + listW - rw - 14), (int)(sy + slotH - 48), 26,
                    isSelected ? silverColor : Fade(silverColor, 0.70f));

                // Save time — đậm hơn, màu riêng
                DrawTextCustom(ui.mainFont, tempGame.saveTime, (int)listX + 26, (int)sy + 8, 22,
                    isSelected ? Fade(gothicGold, 0.80f) : Fade({200, 180, 130, 255}, 0.75f));
            } else {
                // Empty slot
                const char* emStr = "— Empty —";
                int emW = MeasureTextCustomX(ui.mainFont, emStr, 34);
                DrawTextCustom(ui.mainFont, emStr,
                    (int)(listX + listW/2 - emW/2), (int)(sy + slotH/2 - 20), 34,
                    Fade(dimColor, isSelected ? 0.55f : 0.25f));
            }
        }

        // Bottom separator
        float bottomY = listY + panelH;
        DrawLineEx({listX, bottomY}, {listX + listW, bottomY}, 1.5f, Fade(dimColor, 0.4f));
        { // small diamond at bottom left
            float dcx = listX + 10, dcy = bottomY;
            DrawTriangle({dcx-8,dcy},{dcx,dcy+8},{dcx+8,dcy},Fade(dimColor,0.4f));
            DrawTriangle({dcx-8,dcy},{dcx+8,dcy},{dcx,dcy-8},Fade(dimColor,0.4f));
        }

        // ── Vertical divider — rõ hơn ──
        float divX = listX + listW + 40.0f;
        DrawLineEx({divX,   listY - 6}, {divX,   bottomY + 6}, 2.5f, Fade(gothicGold, 0.80f));
        DrawLineEx({divX+6, listY - 6}, {divX+6, bottomY + 6}, 1.0f, Fade(gothicGold, 0.25f));
        // Diamond mid-point sáng
        float dmY = listY + panelH / 2.0f;
        DD5(divX+3, dmY, 24, 24, gothicGold); DD5(divX+3, dmY, 14, 14, BLACK); DC5(divX+3, dmY, 10, gothicGold);

        // ── Right panel: preview ──
        const float prevX = divX + 28.0f;
        const float prevY = listY;
        const float prevW = 1920.0f - prevX - 60.0f;
        const float prevH = panelH;

        GameState selGame;
        bool selHasData = PeekGameSlot(ui.loadSelection, selGame);

        if (selHasData) {
            // ── Save name — to nhất, gothicGold, glow pulse ──
            float snX = prevX;
            int snSize = 72;
            // Outer glow
            for (int g = 3; g >= 1; g--)
                DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX+g, (int)prevY+g, snSize,
                    Fade(gothicGold, 0.08f * glowPulse * (4-g)));
            DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX+3, (int)prevY+4, snSize, Fade(BLACK,0.90f));
            DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX,   (int)prevY,   snSize, gothicGold);

            // Ornate underline
            DrawLineEx({prevX, prevY+82}, {prevX + prevW*0.44f, prevY+82}, 2.0f, Fade(gothicGold, 0.70f));
            DD5(prevX + prevW*0.44f + 12, prevY+82, 16, 16, gothicGold);

            // ── Mode badge — rõ hơn ──
            float contentY = prevY + 100.0f;
            float infoX    = prevX;
            float infoW    = prevW * 0.40f;

            const char* modeStr = (selGame.gameMode == 0) ? "Classic Mode" : "Booming Mode";
            Color modeColor = (selGame.gameMode == 0) ? gothicGold : redAccent;
            int modeW = MeasureTextCustomX(ui.mainFont, modeStr, 32);
            DrawRectangleRounded({infoX, contentY, (float)(modeW+30), 46}, 0.3f, 6, Fade(modeColor, 0.20f));
            DrawRectangleRoundedLines({infoX, contentY, (float)(modeW+30), 46}, 0.3f, 6, Fade(modeColor, 0.75f));
            DrawTextCustom(ui.mainFont, modeStr, (int)infoX+15, (int)contentY+9, 32, modeColor);

            contentY += 62.0f;

            // ── Stats — label sáng hơn, value to hơn ──
            Color labelColor = {190, 170, 120, 255};
            auto DrawStat5 = [&](const char* label, const char* value, Color valCol, float y) {
                DrawTextCustom(ui.mainFont, label, (int)infoX, (int)y, 22, labelColor);
                // Value glow
                DrawTextCustom(ui.mainFont, value, (int)infoX+2, (int)(y+28), 44, Fade(valCol, 0.20f*glowPulse));
                DrawTextCustom(ui.mainFont, value, (int)infoX,   (int)(y+28), 44, valCol);
                DrawLineEx({infoX, y+76}, {infoX+infoW, y+76}, 1.0f, Fade(gothicGold, 0.20f));
            };
            DrawStat5("ROUND",     TextFormat("%d", selGame.roundCount), gothicGold,   contentY);
            DrawStat5("SAVED ON",  selGame.saveTime,                     silverColor,  contentY + 92);
            DrawStat5("NEXT TURN",
                selGame.isPlayer1Turn ? selGame.player1.name : selGame.player2.name,
                warmWhite, contentY + 184);

            // ── Players ──
            float pY = contentY + 284.0f;
            DrawLineEx({infoX, pY}, {infoX+infoW, pY}, 1.5f, Fade(gothicGold, 0.45f));
            DrawTextCustom(ui.mainFont, "PLAYERS", (int)infoX, (int)pY+8, 22, Fade(gothicGold, 0.85f));

            // P1 — ember dot + tên sáng
            DrawCircleV({infoX+12, pY+58}, 6.0f, {255,140,30,255});
            DrawCircleV({infoX+12, pY+58}, 2.5f, WHITE);
            DrawTextCustom(ui.mainFont, selGame.player1.name, (int)infoX+30, (int)(pY+40), 40, warmWhite);

            DrawTextCustom(ui.mainFont, "vs", (int)infoX+30, (int)(pY+88), 24, Fade(dimColor, 0.70f));

            // P2 — red dot + tên silver
            DrawCircleV({infoX+12, pY+132}, 6.0f, redAccent);
            DrawCircleV({infoX+12, pY+132}, 2.5f, WHITE);
            DrawTextCustom(ui.mainFont, selGame.player2.name, (int)infoX+30, (int)(pY+112), 40, silverColor);

            // ── Board preview ──
            float boardX   = prevX + infoW + 50.0f;
            float boardAvW = prevX + prevW - boardX - 10.0f;
            float boardAvH = prevH - 4.0f;
            float boardSize = (boardAvW < boardAvH) ? boardAvW : boardAvH;
            float boardY    = prevY + (prevH - boardSize) * 0.5f;

            // Glow rings — sáng hơn
            for (int g = 4; g >= 1; g--) {
                float go = g * 9.0f;
                DrawRectangleLinesEx({boardX-go, boardY-go, boardSize+go*2, boardSize+go*2},
                    2.0f, Fade(gothicGold, 0.10f * glowPulse * (5-g)));
            }
            DrawTexturePro(ui.boardFrame,
                {0,0,(float)ui.boardFrame.width,(float)ui.boardFrame.height},
                {boardX, boardY, boardSize, boardSize}, {0,0}, 0.0f, WHITE);

            float ix2 = boardX + boardSize*0.073f;
            float iy2 = boardY + boardSize*0.13f;
            float iw2 = boardSize*(1.0f-0.073f-0.070f);
            float ih2 = boardSize*(1.0f-0.13f-0.050f);
            float cw2 = iw2/BOARD_SIZE, ch2 = ih2/BOARD_SIZE;

            for (int r = 0; r < BOARD_SIZE; r++) {
                for (int c = 0; c < BOARD_SIZE; c++) {
                    Rectangle dest = {ix2+c*cw2, iy2+r*ch2, cw2, ch2};
                    DrawTexturePro(ui.cell,   {0,0,(float)ui.cell.width,  (float)ui.cell.height},   dest,{0,0},0.0f,WHITE);
                    if (selGame.board[r][c].c == 1)
                        DrawTexturePro(ui.pieceX,{0,0,(float)ui.pieceX.width,(float)ui.pieceX.height},dest,{0,0},0.0f,WHITE);
                    if (selGame.board[r][c].c == 2)
                        DrawTexturePro(ui.pieceO,{0,0,(float)ui.pieceO.width,(float)ui.pieceO.height},dest,{0,0},0.0f,WHITE);
                }
            }

        } else {
            // Empty slot placeholder
            float cx2 = prevX + prevW / 2.0f;
            float cy2 = prevY + prevH / 2.0f;
            const char* emMsg = "EMPTY SLOT";
            int emMsgW = MeasureTextCustomX(ui.mainFont, emMsg, 52);
            DrawTextCustom(ui.mainFont, emMsg, (int)(cx2-emMsgW/2), (int)(cy2-30), 52, Fade(dimColor, 0.30f));
            const char* hint = "No save data found";
            int hintW = MeasureTextCustomX(ui.mainFont, hint, 28);
            DrawTextCustom(ui.mainFont, hint, (int)(cx2-hintW/2), (int)(cy2+36), 28, Fade(dimColor, 0.20f));
        }

        // ── Bottom guide bar ──
        DrawRectangleGradientV(0, 1012, 1920, 68, Fade(BLACK, 0.0f), Fade(BLACK, 0.96f));
        DrawLineEx({80, 1024}, {1840, 1024}, 1.0f, Fade(accentDim, 0.40f));
        const char* guide = "[W] / [S]  navigate      [ENTER]  load      [DEL]  delete      [M]  back";
        int gW2 = MeasureTextCustomX(ui.mainFont, guide, 26);
        DrawTextCustom(ui.mainFont, guide, 1920/2 - gW2/2, 1040, 26, Fade(silverColor, 0.60f));
    }
    else if (ui.currentScreen == 6) {
        // draw background 
        DrawTexturePro(ui.bgSaveLoad, { 0, 0, (float)ui.bgSaveLoad.width, (float)ui.bgSaveLoad.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.5f));

        // set color palette 
        Color valhallaTeal = { 40, 200, 200, 255 };  // Cyan
        Color textColor = { 230, 230, 230, 255 };    // light gray
        Color mutedText = { 150, 150, 150, 255 };    // muted gray

        // left panel with title and separator
        DrawTextCustom(ui.mainFont, "SAVE GAME", 250, 120, 60, WHITE);
        DrawLineEx({250, 190}, {1670, 190}, 2.0f, Fade(WHITE, 0.2f)); 

        // cell for name input 
        DrawTextCustom(ui.mainFont, "Enter save name:", 250, 230, 30, valhallaTeal);
        Rectangle inputBox = { 550, 220, 1120, 50 };
        DrawLineEx({inputBox.x, inputBox.y + inputBox.height}, {inputBox.x + inputBox.width, inputBox.y + inputBox.height}, 1.0f, Fade(WHITE, 0.3f));
        DrawTextCustom(ui.mainFont, ui.nameInput, inputBox.x + 10, inputBox.y + 10, 35, WHITE);
        
        if (ui.letterCount < 29 && ((int)(GetTime() * 2) % 2) == 0) {
            int textW = MeasureTextCustomX(ui.mainFont, ui.nameInput, 35);
            DrawTextCustom(ui.mainFont, "_", inputBox.x + 10 + textW, inputBox.y + 10, 35, valhallaTeal);
        }

        // slot list
        int startX = 250;
        int startY = 320;
        int slotWidth = 1420;
        int slotHeight = 100;
        int gap = 15;

        for (int i = 0; i < 4; i++) {
            int currentY = startY + i * (slotHeight + gap);
            Rectangle slotRec = { (float)startX, (float)currentY, (float)slotWidth, (float)slotHeight };
            GameState tempGame;
            bool hasData = PeekGameSlot(i, tempGame);

            DrawLineEx({(float)startX, (float)currentY}, {(float)(startX + slotWidth), (float)currentY}, 1.0f, Fade(WHITE, 0.15f));

            if (i == ui.saveSelection) {
                DrawRectangleRec(slotRec, Fade(WHITE, 0.08f)); 
                DrawRectangle(startX, currentY, 5, slotHeight, valhallaTeal); 
            }

            // Text
            if (hasData) {
                Color nameColor = (i == ui.saveSelection) ? WHITE : textColor;
                DrawTextCustom(ui.mainFont, tempGame.saveName, startX + 40, currentY + 15, 40, nameColor);

                const char* modeText = (tempGame.gameMode == 0) ? "Co Dien" : "Booming";
                const char* detailText = TextFormat("Che Do: %s   |   Vong: %d", modeText, tempGame.roundCount);
                DrawTextCustom(ui.mainFont, detailText, startX + 40, currentY + 65, 25, mutedText);

                //time
                int dateW = MeasureTextCustomX(ui.mainFont, tempGame.saveTime, 25);
                DrawTextCustom(ui.mainFont, tempGame.saveTime, startX + slotWidth - dateW - 20, currentY + 35, 25, (i == ui.saveSelection) ? valhallaTeal : mutedText);
            } else {
                Color emptyColor = (i == ui.saveSelection) ? valhallaTeal : Fade(mutedText, 0.5f);
                DrawTextCustom(ui.mainFont, "Slot Trong", startX + 40, currentY + 30, 40, emptyColor);
            }
        }
        
        DrawLineEx({(float)startX, (float)(startY + 4 * (slotHeight + gap))}, {(float)(startX + slotWidth), (float)(startY + 4 * (slotHeight + gap))}, 1.0f, Fade(WHITE, 0.15f));

        //guide 
        const char* footerBtn = "[ENTER] to Save      [ESC] to Cancel";
        int footerW = MeasureTextCustomX(ui.mainFont, footerBtn, 25);
        DrawTextCustom(ui.mainFont, footerBtn, 1920 / 2 - footerW / 2, 950, 25, mutedText);
    }
    else if (ui.currentScreen == 7) {
        DrawTexturePro(ui.bgMenu, { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.85f)); 

        DrawTextCustom(ui.mainFont, "PLAYER INFO", 750, 300, 40, WHITE);
        
        //Player 1
        DrawTextCustom(ui.mainFont, "Ten Player 1 (X) - Toi da 10 ky tu:", 750, 400, 25, LIGHTGRAY);
        Color p1BoxColor = (ui.activeInputField == 0) ? RAYWHITE : DARKGRAY;
        DrawRectangle(750, 440, 400, 50, p1BoxColor);
        DrawTextCustom(ui.mainFont, ui.p1NameInput, 760, 455, 25, BLACK);
        if (ui.activeInputField == 0 && ((int)(GetTime() * 2) % 2) == 0) {
            DrawTextCustom(ui.mainFont, "_", 760 + MeasureTextCustomX(ui.mainFont, ui.p1NameInput, 25), 455, 25, BLACK);
        }

        //Player 2
        DrawTextCustom(ui.mainFont, "Ten Player 2 (O) - Toi da 10 ky tu:", 750, 530, 25, LIGHTGRAY);
        Color p2BoxColor = (ui.activeInputField == 1) ? RAYWHITE : DARKGRAY;
        DrawRectangle(750, 570, 400, 50, p2BoxColor);
        DrawTextCustom(ui.mainFont, ui.p2NameInput, 760, 585, 25, BLACK);
        if (ui.activeInputField == 1 && ((int)(GetTime() * 2) % 2) == 0) {
            DrawTextCustom(ui.mainFont, "_", 760 + MeasureTextCustomX(ui.mainFont, ui.p2NameInput, 25), 585, 25, BLACK);
        }

        DrawTextCustom(ui.mainFont, "Use [Up]/[Down] or [Tab] to switch input fields.", 650, 700, 25, GRAY);
        DrawTextCustom(ui.mainFont, "Press [ENTER] to start the battle!", 750, 750, 25, YELLOW);
        DrawTextCustom(ui.mainFont, "Press [ESC] to go back.", 850, 800, 20, DARKGRAY);
    }
    else if (ui.currentScreen == 8) {
        // Background
        DrawTexturePro(ui.bgSelect, {0,0,(float)ui.bgSelect.width,(float)ui.bgSelect.height},
            {0,0,1920.0f,1080.0f}, {0,0}, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.42f));
        // Vignette
        DrawRectangleGradientH(0, 0, 300, 1080, Fade(BLACK, 0.60f), Fade(BLACK, 0.0f));
        DrawRectangleGradientH(1620, 0, 300, 1080, Fade(BLACK, 0.0f), Fade(BLACK, 0.60f));
        DrawRectangleGradientV(0, 0, 1920, 130, Fade(BLACK, 0.55f), Fade(BLACK, 0.0f));
        DrawRectangleGradientV(0, 920, 1920, 160, Fade(BLACK, 0.0f), Fade(BLACK, 0.85f));

        float t8 = (float)GetTime();
        float gp8 = 0.5f + 0.5f * sinf(t8 * 2.2f);
        float gp8b = 0.5f + 0.5f * sinf(t8 * 1.3f + 1.0f);

        Color gothicGold  = {255, 180,   0, 255};
        Color accentDim   = {160, 110,   0, 255};
        Color redAccent   = {200,  60,  30, 255};
        Color silverColor = {220, 210, 195, 255};
        Color dimColor    = {150, 130, 100, 255};
        Color warmWhite   = {255, 245, 220, 255};
        Color p1Color     = {255, 160,  40, 255};
        Color p2Color     = {100, 160, 255, 255};

        // ── Hero names (3 active heroes, black_knight hidden) ──
        const char* heroNames[5] = {"Fire Knight", "Green Archer", "Earth Assassin", "Metal Blade", "Water Mage"};
        // Maps selection index (0,1,2,3,4) → asset index (1,2,3,4,5)
        const int heroMap[5] = {1, 2, 3, 4, 5};

        // ── Ornate helpers ──
        auto DD8 = [](float cx, float cy, float w, float h, Color col) {
            DrawTriangle({cx-w/2,cy},{cx,cy+h/2},{cx+w/2,cy},col);
            DrawTriangle({cx-w/2,cy},{cx+w/2,cy},{cx,cy-h/2},col);
        };
        auto DC8 = [](float cx, float cy, float sz, Color col) {
            DrawRectangle((int)(cx-sz/6),(int)(cy-sz/2),(int)(sz/3),(int)sz,col);
            DrawRectangle((int)(cx-sz/2),(int)(cy-sz/6),(int)sz,(int)(sz/3),col);
        };
        auto DrawOrnateLine8 = [&](float x, float y, float width, Color c) {
            DrawLineEx({x+40,y},{x+width-40,y},4.0f,c);
            DrawLineEx({x+40,y-5},{x+width-40,y-5},1.0f,Fade(c,0.5f));
            DrawLineEx({x+40,y+5},{x+width-40,y+5},1.0f,Fade(c,0.5f));
            float cx=x+width/2;
            DD8(cx,y,40,40,c); DD8(cx,y,28,28,BLACK); DD8(cx,y,16,16,c); DC8(cx,y,12,BLACK);
            for(int s=1;s<12;s++){float px=x+40+s*((width-80)/12);if(fabsf(px-cx)>50){DD8(px,y,10,10,c);DrawCircle((int)px,(int)y,2,BLACK);}}
            DD8(x+20,y,28,28,c);DD8(x+20,y,16,16,BLACK);DC8(x+20,y,10,c);DD8(x,y,14,14,c);
            DD8(x+width-20,y,28,28,c);DD8(x+width-20,y,16,16,BLACK);DC8(x+width-20,y,10,c);DD8(x+width,y,14,14,c);
        };

        // ── Top ornament line ──
        DrawOrnateLine8(80, 88, 1760, gothicGold);

        // ── Title ──
        const char* title8 = "CHARACTER SELECT";
        int titW8 = MeasureTextCustomX(ui.mainFont, title8, 68);
        DrawTextCustom(ui.mainFont, title8, 1920/2-titW8/2+3, 106, 68, Fade(BLACK,0.9f));
        DrawTextCustom(ui.mainFont, title8, 1920/2-titW8/2,   102, 68, gothicGold);

        // ── 5 Hero card slots — chia đều ngang ──
        const float cardW   = 320.0f;
        const float cardH   = 510.0f;
        const float cardGap = 24.0f;
        const float totalCW = 5*cardW + 4*cardGap;
        const float cardStartX = (1920.0f - totalCW) / 2.0f;
        const float cardY   = 310.0f;

        // Xác định card nào đang được hover (dựa vào selectionPhase)
        int curSel = (ui.selectionPhase <= 1) ? ui.p1HeroSelection : ui.p2HeroSelection;
        bool isP1turn = (ui.selectionPhase == 0 || ui.selectionPhase == 1);

        // Per-hero color theme (index 0=fire, 1=archer, 2=assassin, 3=metal_blade, 4=water_mage)
        Color heroTheme[5] = {
            {220,  70,  20, 255},  // fire_knight   — red-orange
            { 40, 170, 110, 255},  // wind_archer   — teal-green
            { 60, 110, 220, 255},  // wind_assassin — royal blue
            {180, 160,  60, 255},  // metal_blade   — gold/steel
            { 30, 160, 220, 255},  // water_mage    — sky blue
        };

        // Hero bottom-pixel anchors — all 5 heroes use 126px
        struct HeroBot { float bottomPx; } bots[5] = {
            {126.0f}, {126.0f}, {126.0f}, {126.0f}, {126.0f},
        };

        for (int i = 0; i < 5; i++) {
            int assetID = heroMap[i]; // translate to asset index
            float cx = cardStartX + i*(cardW+cardGap);
            bool isSelected = (curSel == i);
            bool p1owns = (ui.selectionPhase >= 1 && ui.p1HeroSelection == i);
            bool p2owns = (ui.selectionPhase >= 3 && ui.p2HeroSelection == i);

            // Selected card lifts up
            float cardOffY = isSelected ? -16.0f : 0.0f;
            float cy = cardY + cardOffY;

            Color hTheme = heroTheme[i];

            // ── Card background: per-hero gradient ──
            DrawRectangleRec({cx, cy, cardW, cardH}, Fade(BLACK, 0.80f));
            // Hero color seeps in from top
            DrawRectangleGradientV((int)cx, (int)cy, (int)cardW, (int)(cardH*0.55f),
                Fade(hTheme, isSelected ? 0.32f : 0.14f), Fade(hTheme, 0.0f));
            // Dark vignette at bottom
            DrawRectangleGradientV((int)cx, (int)(cy+cardH*0.45f), (int)cardW, (int)(cardH*0.55f),
                Fade(BLACK, 0.0f), Fade(BLACK, 0.82f));

            // Ambient glow at center-bottom when selected
            float footY = cy + cardH * 0.88f;
            if (isSelected) {
                for (int g = 5; g >= 1; g--) {
                    float gr = g * 28.0f;
                    float ga = 0.10f * gp8 * (6-g) / 5.0f;
                    DrawCircle((int)(cx+cardW/2), (int)footY, gr, Fade(hTheme, ga));
                }
            } else {
                // subtle ambient even when unselected
                DrawCircle((int)(cx+cardW/2), (int)footY, 40.0f, Fade(hTheme, 0.06f));
            }

            // Selected: player color top overlay + outer glow pulse
            if (isSelected) {
                Color selCol = isP1turn ? p1Color : p2Color;
                DrawRectangleGradientV((int)cx, (int)cy, (int)cardW, (int)(cardH*0.3f),
                    Fade(selCol, 0.12f), Fade(selCol, 0.0f));
                // Shimmer sweep
                float shimX = cx + fmodf(t8 * 160.0f, cardW + 80.0f) - 40.0f;
                DrawRectangleGradientH((int)shimX, (int)cy, 60, (int)cardH,
                    Fade(WHITE, 0.0f), Fade(WHITE, 0.07f * gp8b));
                DrawRectangleGradientH((int)(shimX+60), (int)cy, 60, (int)cardH,
                    Fade(WHITE, 0.07f * gp8b), Fade(WHITE, 0.0f));
                // Outer glow rings
                for (int g = 4; g >= 1; g--) {
                    float go = g * 5.0f;
                    DrawRectangleLinesEx({cx-go, cy-go, cardW+go*2, cardH+go*2},
                        1.5f, Fade(selCol, 0.16f * gp8 * (5-g) / 4.0f));
                }
            }

            // ── Ornate diamond corners ──
            float crnSz = 10.0f;
            Color crnCol = isSelected ? Fade(hTheme, 0.90f) : Fade(hTheme, 0.45f);
            // top-left
            DD8(cx,       cy,       crnSz, crnSz, crnCol);
            DD8(cx+crnSz, cy,       crnSz, crnSz, crnCol);
            DD8(cx,       cy+crnSz, crnSz, crnSz, crnCol);
            // top-right
            DD8(cx+cardW,       cy,       crnSz, crnSz, crnCol);
            DD8(cx+cardW-crnSz, cy,       crnSz, crnSz, crnCol);
            DD8(cx+cardW,       cy+crnSz, crnSz, crnSz, crnCol);
            // bottom-left
            DD8(cx,       cy+cardH,       crnSz, crnSz, crnCol);
            DD8(cx+crnSz, cy+cardH,       crnSz, crnSz, crnCol);
            DD8(cx,       cy+cardH-crnSz, crnSz, crnSz, crnCol);
            // bottom-right
            DD8(cx+cardW,       cy+cardH,       crnSz, crnSz, crnCol);
            DD8(cx+cardW-crnSz, cy+cardH,       crnSz, crnSz, crnCol);
            DD8(cx+cardW,       cy+cardH-crnSz, crnSz, crnSz, crnCol);

            // ── Hero sprite ──
            const CharAnim& anim = ui.heroIdle[assetID];
            float botPx   = bots[i].bottomPx;
            float heroScaleMult = 1.3f; // all 3 remaining heroes use same scale
            float scale8  = (cardW * 3.0f) / (float)anim.frameWidth * heroScaleMult;
            float heroW   = anim.frameWidth  * scale8;
            float heroH   = anim.frameHeight * scale8;
            float footOff = botPx * scale8;
            float hy      = footY - footOff;
            float hx      = cx + cardW/2 - heroW/2;

            Rectangle src8 = {(float)anim.currentFrame*anim.frameWidth, 0,
                (float)anim.frameWidth, (float)anim.frameHeight};
            Rectangle dst8 = {hx, hy, heroW, heroH};

            // Dim non-selected heroes
            Color heroTint = isSelected ? WHITE : Fade(WHITE, 0.50f);
            DrawTexturePro(anim.spriteSheet, src8, dst8, {0,0}, 0.0f, heroTint);

            // P1/P2 lock badge
            if (p1owns) {
                DrawRectangleRounded({cx+8, cy+8, 56, 28}, 0.4f, 4, Fade(p1Color, 0.85f));
                DrawTextCustom(ui.mainFont, "P1", (int)cx+18, (int)cy+12, 22, BLACK);
            }
            if (p2owns) {
                DrawRectangleRounded({cx+8, cy+(p1owns?42:8), 56, 28}, 0.4f, 4, Fade(p2Color, 0.85f));
                DrawTextCustom(ui.mainFont, "P2", (int)cx+18, (int)cy+(p1owns?46:12), 22, BLACK);
            }

            // ── Card border ──
            Color borderCol;
            float borderW8;
            if (isSelected) {
                borderCol = isP1turn ? p1Color : p2Color;
                borderW8  = 3.0f;
            } else if (p1owns || p2owns) {
                borderCol = p1owns ? Fade(p1Color,0.60f) : Fade(p2Color,0.60f);
                borderW8  = 2.0f;
            } else {
                borderCol = Fade(hTheme, 0.40f);
                borderW8  = 1.5f;
            }
            DrawRectangleLinesEx({cx, cy, cardW, cardH}, borderW8, borderCol);
            // Inner thin accent line
            DrawRectangleLinesEx({cx+4, cy+4, cardW-8, cardH-8}, 0.8f, Fade(hTheme, isSelected ? 0.35f : 0.15f));

            // ── Hero name below card ──
            const char* hname = heroNames[i];
            int hnW = MeasureTextCustomX(ui.mainFont, hname, 30);
            float nameY = cy + cardH + 10.0f;
            DrawTextCustom(ui.mainFont, hname, (int)(cx+cardW/2-hnW/2)+2, (int)nameY+2, 30, Fade(BLACK,0.9f));
            Color nameCol = isSelected ? (isP1turn ? p1Color : p2Color) : Fade(hTheme, 0.75f);
            DrawTextCustom(ui.mainFont, hname, (int)(cx+cardW/2-hnW/2), (int)nameY, 30, nameCol);

            // Ornate line + diamond under name when selected
            if (isSelected) {
                float lx = cx+cardW/2-60;
                DrawLineEx({lx, nameY+36}, {lx+120, nameY+36}, 1.5f, Fade(nameCol, 0.6f));
                DD8(lx+60, nameY+36, 10, 10, nameCol);
            }
        }

        // ── Draw char select particles ──
        {
            Color heroThemeParts[5] = {
                {255, 120,  30, 255},  // fire_knight   — orange-red
                { 60, 220, 130, 255},  // wind_archer   — bright teal
                { 80, 150, 255, 255},  // wind_assassin — sky blue
                {220, 200,  60, 255},  // metal_blade   — gold shimmer
                { 40, 200, 255, 255},  // water_mage    — aqua
            };
            for (int k = 0; k < UIState::MAX_CHAR_PARTICLES; k++) {
                const UIState::CharParticle& p = ui.charParticles[k];
                if (p.life <= 0.0f || p.alpha <= 0.0f) continue;
                Color col = Fade(heroThemeParts[p.heroID], p.alpha);
                if (p.type == 0) {
                    // Rising ember — small glowing dot
                    DrawCircle((int)p.x, (int)p.y, p.size, col);
                    // Glow halo
                    DrawCircle((int)p.x, (int)p.y, p.size * 2.2f, Fade(col, 0.25f));
                } else {
                    // Rune diamond — rotated diamond shape
                    float s = p.size;
                    // Draw diamond using 2 triangles (no rotation, just diamond shape)
                    DrawTriangle(
                        {p.x,     p.y - s},
                        {p.x + s, p.y    },
                        {p.x,     p.y + s},
                        col);
                    DrawTriangle(
                        {p.x,     p.y - s},
                        {p.x,     p.y + s},
                        {p.x - s, p.y    },
                        col);
                    // Inner cross
                    DrawLineEx({p.x - s*0.5f, p.y}, {p.x + s*0.5f, p.y}, 1.0f, Fade(WHITE, p.alpha * 0.6f));
                    DrawLineEx({p.x, p.y - s*0.5f}, {p.x, p.y + s*0.5f}, 1.0f, Fade(WHITE, p.alpha * 0.6f));
                }
            }
        }

        // ── Turn indicator bar ──
        if (ui.selectionPhase == 0 || ui.selectionPhase == 2) {
            Color turnCol = (ui.selectionPhase == 0) ? p1Color : p2Color;
            const char* pLabel = (ui.selectionPhase == 0) ? "PLAYER 1" : "PLAYER 2";
            const char* turnTxt = "  —  Choose your champion";
            int plW  = MeasureTextCustomX(ui.mainFont, pLabel, 38);
            int ttW  = MeasureTextCustomX(ui.mainFont, turnTxt, 32);
            int totW = plW + ttW;
            int barY = 880;
            DrawTextCustom(ui.mainFont, pLabel,   1920/2-totW/2,      barY, 38, turnCol);
            DrawTextCustom(ui.mainFont, turnTxt,  1920/2-totW/2+plW,  barY+4, 32, Fade(warmWhite,0.65f));
            // Pulse underline
            DrawLineEx({(float)(1920/2-totW/2), (float)(barY+44)},
                       {(float)(1920/2+totW/2), (float)(barY+44)},
                       1.5f, Fade(turnCol, 0.45f*gp8));
        }

        // ── P1/P2 chosen display — bottom corners ──
        if (ui.selectionPhase >= 1) {
            // P1 bottom left
            const char* p1n = game.player1.name[0] ? game.player1.name : "PLAYER 1";
            DrawCircleV({90, 960}, 7.0f, p1Color);
            DrawCircleV({90, 960}, 3.0f, WHITE);
            DrawTextCustom(ui.mainFont, p1n, 108, 942, 32, Fade(warmWhite, 0.90f));
            const char* p1hero = heroNames[ui.p1HeroSelection];
            DrawTextCustom(ui.mainFont, p1hero, 108, 980, 22, Fade(p1Color, 0.80f));
        }
        if (ui.selectionPhase >= 3) {
            // P2 bottom right
            const char* p2n = game.player2.name[0] ? game.player2.name : "PLAYER 2";
            const char* p2hero = heroNames[ui.p2HeroSelection];
            int p2nW = MeasureTextCustomX(ui.mainFont, p2n, 32);
            int p2hW = MeasureTextCustomX(ui.mainFont, p2hero, 22);
            DrawCircleV({1830, 960}, 7.0f, p2Color);
            DrawCircleV({1830, 960}, 3.0f, WHITE);
            DrawTextCustom(ui.mainFont, p2n,    (int)(1830-p2nW-18), 942, 32, Fade(warmWhite,0.90f));
            DrawTextCustom(ui.mainFont, p2hero, (int)(1830-p2hW-18), 980, 22, Fade(p2Color,0.80f));
        }

        // ── Bottom ornate line ──
        DrawOrnateLine8(80, 1000, 1760, Fade(gothicGold, 0.60f));

        // ── Guide hint ──
        const char* guide8 = "[A] / [D]  navigate      [ENTER]  select      [M]  back";
        int gW8 = MeasureTextCustomX(ui.mainFont, guide8, 24);
        DrawTextCustom(ui.mainFont, guide8, 1920/2-gW8/2, 1038, 24, Fade(silverColor, 0.55f));

        // ── Overlay: Game mode selection ──
        if (ui.selectionPhase == -1) {
            DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.65f));

            const char* modeTxt = game.isVsBot ? "VS  BOT" : "VS  PLAYER";
            Color modeCol = game.isVsBot ? redAccent : p2Color;

            int boxW=920, boxH=270;
            Rectangle box={(float)(1920/2-boxW/2),(float)(1080/2-boxH/2),(float)boxW,(float)boxH};
            DrawRectangleRec(box, Fade(BLACK, 0.90f));
            DrawRectangleLinesEx(box, 2.5f, Fade(gothicGold, 0.85f));
            DrawRectangleLinesEx({box.x-5,box.y-5,box.width+10,box.height+10}, 1.0f,
                Fade(gothicGold, 0.25f*gp8));

            const char* label8 = "SELECT GAME MODE";
            int labW = MeasureTextCustomX(ui.mainFont, label8, 28);
            DrawTextCustom(ui.mainFont, label8, (int)(box.x+boxW/2-labW/2), (int)box.y+28, 28,
                Fade(dimColor, 0.90f));
            DrawLineEx({box.x+40, box.y+68}, {box.x+boxW-40, box.y+68}, 1.5f, Fade(gothicGold,0.40f));

            const char* arrow = "<  ";
            const char* arrowR = "  >";
            int modeW2 = MeasureTextCustomX(ui.mainFont, modeTxt, 58);
            int arrowLW = MeasureTextCustomX(ui.mainFont, arrow, 58);
            int arrowRW = MeasureTextCustomX(ui.mainFont, arrowR, 58);
            int totalMW = arrowLW + modeW2 + arrowRW;
            int modeX = (int)(box.x+boxW/2-totalMW/2);
            int modeY = (int)(box.y+102);
            DrawTextCustom(ui.mainFont, arrow,   modeX,               modeY, 58, Fade(gothicGold, 0.70f));
            DrawTextCustom(ui.mainFont, modeTxt, modeX+arrowLW,       modeY, 58, modeCol);
            DrawTextCustom(ui.mainFont, arrowR,  modeX+arrowLW+modeW2,modeY, 58, Fade(gothicGold, 0.70f));

            const char* hint8 = "[A] / [D]  switch      [ENTER]  confirm";
            int hintW8 = MeasureTextCustomX(ui.mainFont, hint8, 24);
            DrawTextCustom(ui.mainFont, hint8, (int)(box.x+boxW/2-hintW8/2), (int)box.y+204, 24,
                Fade(silverColor, 0.55f));
        }
        // ── Overlay: Name input ──
        else if (ui.selectionPhase == 1 || ui.selectionPhase == 3) {
            DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.65f));

            bool isP1 = (ui.selectionPhase == 1);
            Color pCol = isP1 ? p1Color : p2Color;
            int heroID = isP1 ? ui.p1HeroSelection : ui.p2HeroSelection;
            Color hTheme = heroTheme[heroID];

            const char* prompt8 = isP1 ? "PLAYER 1  —  Enter your name" : "PLAYER 2  —  Enter your name";
            const char* input8  = isP1 ? ui.p1NameInput : ui.p2NameInput;

            int boxW=900, boxH=260;
            Rectangle box={(float)(1920/2-boxW/2),(float)(1080/2-boxH/2),(float)boxW,(float)boxH};
            DrawRectangleRec(box, Fade(BLACK, 0.90f));

            // Viền ngoài màu tướng
            DrawRectangleLinesEx(box, 2.5f, Fade(hTheme, 0.90f));
            // Viền glow mờ bên ngoài, nhấp nháy
            DrawRectangleLinesEx({box.x-5,box.y-5,box.width+10,box.height+10}, 1.0f,
                Fade(hTheme, 0.30f * gp8));

            // Accent bar top màu tướng
            DrawRectangleGradientH((int)box.x, (int)box.y, (int)boxW, 5,
                Fade(hTheme, 0.0f), Fade(hTheme, 0.95f));

            int prmW = MeasureTextCustomX(ui.mainFont, prompt8, 30);
            DrawTextCustom(ui.mainFont, prompt8, (int)(box.x+boxW/2-prmW/2), (int)box.y+26, 30, pCol);
            DrawLineEx({box.x+40, box.y+70}, {box.x+boxW-40, box.y+70}, 1.0f, Fade(hTheme, 0.40f));

            DrawTextCustom(ui.mainFont, input8, (int)box.x+52, (int)box.y+104, 54, warmWhite);
            if (((int)(GetTime()*2)%2)==0) {
                int curX = (int)box.x+52+MeasureTextCustomX(ui.mainFont, input8, 54);
                DrawTextCustom(ui.mainFont, "|", curX, (int)box.y+104, 54, Fade(hTheme, 0.95f));
            }

            // Selected hero reminder màu tướng
            const char* selHeroName = heroNames[heroID];
            int shrW = MeasureTextCustomX(ui.mainFont, selHeroName, 22);
            DrawTextCustom(ui.mainFont, selHeroName, (int)(box.x+boxW/2-shrW/2), (int)box.y+176, 22,
                Fade(hTheme, 0.75f));

            const char* hint8b = "[ENTER] confirm    [ESC] back";
            int hw = MeasureTextCustomX(ui.mainFont, hint8b, 22);
            DrawTextCustom(ui.mainFont, hint8b, (int)(box.x+boxW/2-hw/2), (int)box.y+210, 22,
                Fade(silverColor, 0.50f));
        }
    }
}