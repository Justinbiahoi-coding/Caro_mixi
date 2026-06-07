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
        // Animate hero selection sprites
        ui.charP1.frameTimer += GetFrameTime();
        if (ui.charP1.frameTimer >= ui.charP1.frameDuration) {
            ui.charP1.currentFrame = (ui.charP1.currentFrame + 1) % ui.charP1.frameCount;
            ui.charP1.frameTimer = 0.0f;
        }
        ui.charP2.frameTimer += GetFrameTime();
        if (ui.charP2.frameTimer >= ui.charP2.frameDuration) {
            ui.charP2.currentFrame = (ui.charP2.currentFrame + 1) % ui.charP2.frameCount;
            ui.charP2.frameTimer = 0.0f;
        }

        const int MAX_HEROES = 4; // set number of heroes available

        if (ui.selectionPhase == -1) {
            // Chon che do choi: VS Player hoac VS Bot
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) game.isVsBot = false;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) game.isVsBot = true;
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 0;
        }
        else if (ui.selectionPhase == 0) {
            // p1 pick hero
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) ui.p1HeroSelection = (ui.p1HeroSelection - 1 + MAX_HEROES) % MAX_HEROES;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) ui.p1HeroSelection = (ui.p1HeroSelection + 1) % MAX_HEROES;
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 1; 
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
            if (IsKeyPressed(KEY_ENTER)) {
                if (game.isVsBot) {
                    int savedInput = game.inputType;
                    InitGame(game, 0); 
                    game.inputType = savedInput;
                    
                    if (ui.p1LetterCount > 0) strcpy(game.player1.name, ui.p1NameInput);
                    else strcpy(game.player1.name, "Player 1");
                    
                    strcpy(game.player2.name, "BOT");
                    ui.p2HeroSelection = 3;
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
            if (IsKeyPressed(KEY_ENTER)){
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
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.70f));

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

            // Selected: gradient highlight background
            if (isSelected) {
                DrawRectangleGradientH((int)listX, (int)sy+1, (int)listW, (int)slotH - 1,
                    Fade(gothicGold, 0.13f), Fade(BLACK, 0.0f));
                // Left accent bar
                DrawRectangleGradientV((int)listX, (int)sy+1, 4, (int)slotH-1,
                    Fade(gothicGold, 0.0f), Fade(gothicGold, 0.9f));
                DrawRectangleGradientV((int)listX, (int)(sy + slotH/2), 4, (int)(slotH/2)-1,
                    Fade(gothicGold, 0.9f), Fade(gothicGold, 0.0f));
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

            // Slot number label (top right)
            const char* slotLabel = TextFormat("SLOT %d", i + 1);
            int slW = MeasureTextCustomX(ui.mainFont, slotLabel, 22);
            DrawTextCustom(ui.mainFont, slotLabel, (int)(listX + listW - slW - 12), (int)sy + 10, 22,
                isSelected ? Fade(gothicGold, 0.70f) : Fade(dimColor, 0.50f));

            if (hasData) {
                // ">" + save name — large
                Color nameCol = isSelected ? warmWhite : Fade(silverColor, 0.55f);
                int nameSize  = isSelected ? 50 : 42;
                if (isSelected) {
                    DrawTextCustom(ui.mainFont, ">", (int)listX + 18, (int)sy + 34, 44, Fade(gothicGold, 0.9f));
                    DrawTextCustom(ui.mainFont, tempGame.saveName, (int)listX + 62, (int)sy + 34, nameSize,
                        Fade(BLACK, 0.6f)); // shadow
                }
                DrawTextCustom(ui.mainFont, tempGame.saveName,
                    (int)listX + (isSelected ? 62 : 26), (int)sy + (isSelected ? 32 : 34), nameSize, nameCol);

                // Mode badge — colored
                const char* modeShort = (tempGame.gameMode == 0) ? "Classic" : "Booming";
                Color modeCol = (tempGame.gameMode == 0)
                    ? (isSelected ? Fade(gothicGold, 0.90f) : Fade(gothicGold, 0.45f))
                    : (isSelected ? Fade(redAccent,  0.90f) : Fade(redAccent,  0.50f));
                DrawTextCustom(ui.mainFont, modeShort, (int)listX + 26, (int)(sy + slotH - 50), 30, modeCol);

                // Round (bottom right)
                const char* roundStr = TextFormat("Round %d", tempGame.roundCount);
                int rw = MeasureTextCustomX(ui.mainFont, roundStr, 26);
                DrawTextCustom(ui.mainFont, roundStr, (int)(listX + listW - rw - 14), (int)(sy + slotH - 48), 26,
                    isSelected ? Fade(silverColor, 0.80f) : Fade(dimColor, 0.50f));

                // Save time (inline with slot label)
                DrawTextCustom(ui.mainFont, tempGame.saveTime, (int)listX + 26, (int)sy + 10, 20,
                    Fade(dimColor, isSelected ? 0.75f : 0.40f));
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

        // ── Vertical divider ──
        float divX = listX + listW + 40.0f;
        DrawLineEx({divX,   listY - 6}, {divX,   bottomY + 6}, 2.0f, Fade(gothicGold, 0.50f));
        DrawLineEx({divX+5, listY - 6}, {divX+5, bottomY + 6}, 0.8f, Fade(gothicGold, 0.18f));
        // Diamond mid-point on divider
        float dmY = listY + panelH / 2.0f;
        DD5(divX+2, dmY, 18, 18, gothicGold); DD5(divX+2, dmY, 10, 10, BLACK); DC5(divX+2, dmY, 8, gothicGold);

        // ── Right panel: preview ──
        const float prevX = divX + 28.0f;
        const float prevY = listY;
        const float prevW = 1920.0f - prevX - 60.0f;
        const float prevH = panelH;

        GameState selGame;
        bool selHasData = PeekGameSlot(ui.loadSelection, selGame);

        if (selHasData) {
            // ── Save name — big, gold ──
            float snX = prevX;
            DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX + 3, (int)prevY + 4, 64, Fade(BLACK, 0.85f));
            DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX,     (int)prevY,     64,
                Fade(gothicGold, 0.25f * glowPulse)); // glow layer
            DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX,     (int)prevY,     64, warmWhite);

            // Ornate underline after name
            DrawLineEx({prevX, prevY + 74}, {prevX + prevW * 0.42f, prevY + 74}, 2.0f, Fade(gothicGold, 0.55f));
            DD5(prevX + prevW*0.42f + 12, prevY + 74, 14, 14, Fade(gothicGold, 0.55f));

            // ── Mode badge ──
            float contentY = prevY + 90.0f;
            float infoX    = prevX;
            float infoW    = prevW * 0.38f;

            const char* modeStr = (selGame.gameMode == 0) ? "Classic Mode" : "Booming Mode";
            Color modeColor = (selGame.gameMode == 0) ? gothicGold : redAccent;
            // Pill background
            int modeW = MeasureTextCustomX(ui.mainFont, modeStr, 30);
            DrawRectangleRounded({infoX, contentY, (float)(modeW + 28), 42}, 0.3f, 6,
                Fade(modeColor, 0.15f));
            DrawRectangleRoundedLines({infoX, contentY, (float)(modeW + 28), 42}, 0.3f, 6,
                Fade(modeColor, 0.45f));
            DrawTextCustom(ui.mainFont, modeStr, (int)infoX + 14, (int)contentY + 8, 30, modeColor);

            contentY += 56.0f;

            // ── Stats ──
            auto DrawStat5 = [&](const char* label, const char* value, Color valCol, float y) {
                DrawTextCustom(ui.mainFont, label, (int)infoX, (int)y, 22, Fade(dimColor, 0.85f));
                DrawTextCustom(ui.mainFont, value, (int)infoX + 2, (int)(y + 30), 42, Fade(valCol, 0.15f*glowPulse));
                DrawTextCustom(ui.mainFont, value, (int)infoX,     (int)(y + 30), 42, valCol);
                DrawLineEx({infoX, y + 76}, {infoX + infoW, y + 76}, 1.0f, Fade(dimColor, 0.22f));
            };
            DrawStat5("ROUND",
                TextFormat("%d", selGame.roundCount),
                gothicGold, contentY);
            DrawStat5("SAVED ON",
                selGame.saveTime,
                silverColor, contentY + 90);
            DrawStat5("NEXT TURN",
                selGame.isPlayer1Turn ? selGame.player1.name : selGame.player2.name,
                warmWhite, contentY + 180);

            // ── Players ──
            float pY = contentY + 278.0f;
            DrawLineEx({infoX, pY}, {infoX + infoW, pY}, 1.0f, Fade(gothicGold, 0.30f));
            DrawTextCustom(ui.mainFont, "PLAYERS", (int)infoX, (int)pY + 8, 22, Fade(gothicGold, 0.65f));

            // P1 — ember dot
            DrawCircleV({infoX + 12, pY + 56}, 5.0f, Fade({255,140,30,255}, 0.85f));
            DrawCircleV({infoX + 12, pY + 56}, 2.0f, Fade(WHITE, 0.9f));
            DrawTextCustom(ui.mainFont, selGame.player1.name, (int)infoX + 28, (int)(pY + 38), 38, Fade(warmWhite, 0.90f));

            DrawTextCustom(ui.mainFont, "vs", (int)infoX + 28, (int)(pY + 84), 24, Fade(dimColor, 0.60f));

            // P2 — red dot
            DrawCircleV({infoX + 12, pY + 128}, 5.0f, Fade(redAccent, 0.80f));
            DrawCircleV({infoX + 12, pY + 128}, 2.0f, Fade(WHITE, 0.8f));
            DrawTextCustom(ui.mainFont, selGame.player2.name, (int)infoX + 28, (int)(pY + 110), 38, Fade(silverColor, 0.75f));

            // ── Board preview ──
            float boardX   = prevX + infoW + 50.0f;
            float boardAvW = prevX + prevW - boardX - 10.0f;
            float boardAvH = prevH - 4.0f;
            float boardSize = (boardAvW < boardAvH) ? boardAvW : boardAvH;
            float boardY    = prevY + (prevH - boardSize) * 0.5f;

            // Glow rings
            for (int g = 3; g >= 1; g--) {
                float go = g * 8.0f;
                DrawRectangleLinesEx({boardX-go, boardY-go, boardSize+go*2, boardSize+go*2},
                    1.5f, Fade(gothicGold, 0.05f * glowPulse * (4-g)));
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

        DrawTextCustom(ui.mainFont, "THONG TIN NGUOI CHOI", 750, 300, 40, WHITE);
        
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
        //background 
        DrawTexturePro(ui.bgSelect, { 0, 0, (float)ui.bgSelect.width, (float)ui.bgSelect.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.2f)); 

        Color goldColor = { 253, 249, 0, 255 }; 
        Color silverColor = { 200, 200, 200, 255 };

        const char* title = "Lua chon nhan vat";
        DrawTextCustom(ui.mainFont, title, 1920 / 2 - MeasureTextCustomX(ui.mainFont, title, 50) / 2, 60, 50, goldColor);

        // draw flipped hero preview on sides with lock effect if not selected yet
        auto DrawPreviewHero = [&](int heroID, float x, float y, bool isFlipped, bool isLocked) {
            CharAnim anim;
            if (heroID == 0) anim = ui.heroIdle[0];
            else if (heroID == 1) anim = ui.heroIdle[1];
            else if (heroID == 2) anim = ui.heroIdle[2];
            else if (heroID == 3) anim = ui.heroIdle[3];
            else return; 

            float drawScale = 8.0f;
            Rectangle source = { (float)anim.currentFrame * anim.frameWidth, 0.0f, isFlipped ? -(float)anim.frameWidth : (float)anim.frameWidth, (float)anim.frameHeight }; 
            Rectangle dest = { x - (anim.frameWidth * drawScale) / 2, y - (anim.frameHeight * drawScale), anim.frameWidth * drawScale, anim.frameHeight * drawScale };
            DrawTexturePro(anim.spriteSheet, source, dest, {0,0}, 0.0f, isLocked ? WHITE : Fade(WHITE, 0.4f));
        };

        //p1
        DrawPreviewHero(ui.p1HeroSelection, 400, 800, false, (ui.selectionPhase >= 1));
        //p2
        if (ui.selectionPhase >= 2) {
            DrawPreviewHero(ui.p2HeroSelection, 1520, 800, true, (ui.selectionPhase >= 3));
        }

        // avatar selection area
        int avatarSize = 120;
        int gap = 30;
        int totalWidth = (4 * avatarSize) + (3 * gap);
        int startX = (1920 - totalWidth) / 2;
        int startY = 820;

        for (int i = 0; i < 4; i++) {
            Rectangle avtRec = { (float)startX + i * (avatarSize + gap), (float)startY, (float)avatarSize, (float)avatarSize };
            
            // avatar border
            bool isCurrent = (ui.selectionPhase <= 1 && ui.p1HeroSelection == i) || (ui.selectionPhase >= 2 && ui.p2HeroSelection == i);
            DrawRectangleLinesEx(avtRec, 2, isCurrent ? goldColor : silverColor);
            if (isCurrent) DrawRectangleRec(avtRec, Fade(goldColor, 0.2f));

            // avatar - display all 4 heroes
            if (i == 0) DrawTexturePro(ui.heroIdle[0].spriteSheet, {0, 0, (float)ui.heroIdle[0].frameWidth, (float)ui.heroIdle[0].frameHeight}, avtRec, {0,0}, 0.0f, WHITE);
            else if (i == 1) DrawTexturePro(ui.heroIdle[1].spriteSheet, {0, 0, (float)ui.heroIdle[1].frameWidth, (float)ui.heroIdle[1].frameHeight}, avtRec, {0,0}, 0.0f, WHITE);
            else if (i == 2) DrawTexturePro(ui.heroIdle[2].spriteSheet, {0, 0, (float)ui.heroIdle[2].frameWidth, (float)ui.heroIdle[2].frameHeight}, avtRec, {0,0}, 0.0f, WHITE);
            else if (i == 3) DrawTexturePro(ui.heroIdle[3].spriteSheet, {0, 0, (float)ui.heroIdle[3].frameWidth, (float)ui.heroIdle[3].frameHeight}, avtRec, {0,0}, 0.0f, WHITE);
        }

        //input for name entry during selection phases
        if (ui.selectionPhase == -1) {
            DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.5f)); 
            const char* prompt = "CHON CHE DO CHOI";
            const char* input = game.isVsBot ? "< Choi Voi May >" : "< Choi Voi Nguoi >";
            
            int boxW = 800, boxH = 200;
            Rectangle box = { (float)(1920/2 - boxW/2), (float)(1080/2 - boxH/2), (float)boxW, (float)boxH };
            DrawRectangleRec(box, Fade(BLACK, 0.9f));
            DrawRectangleLinesEx(box, 2, goldColor);

            int promptW = MeasureTextCustomX(ui.mainFont, prompt, 30);
            int inputW = MeasureTextCustomX(ui.mainFont, input, 50);

            DrawTextCustom(ui.mainFont, prompt, box.x + boxW/2 - promptW/2, box.y + 40, 30, silverColor);
            DrawTextCustom(ui.mainFont, input, box.x + boxW/2 - inputW/2, box.y + 110, 50, WHITE);
            DrawTextCustom(ui.mainFont, "[A]/[D] to switch, [ENTER] to confirm", box.x + boxW/2 - 200, box.y + 180, 20, DARKGRAY);
        }
        else if (ui.selectionPhase == 1 || ui.selectionPhase == 3) {
            DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.5f)); 
            const char* prompt = (ui.selectionPhase == 1) ? "PLAYER 1 - NHAP TEN" : "PLAYER 2 - NHAP TEN";
            const char* input = (ui.selectionPhase == 1) ? ui.p1NameInput : ui.p2NameInput;
            
            int boxW = 800, boxH = 200;
            Rectangle box = { (float)(1920/2 - boxW/2), (float)(1080/2 - boxH/2), (float)boxW, (float)boxH };
            DrawRectangleRec(box, Fade(BLACK, 0.9f));
            DrawRectangleLinesEx(box, 2, goldColor);

            DrawTextCustom(ui.mainFont, prompt, box.x + 40, box.y + 40, 30, silverColor);
            DrawTextCustom(ui.mainFont, input, box.x + 40, box.y + 110, 50, WHITE);
            if (((int)(GetTime() * 2) % 2) == 0) DrawTextCustom(ui.mainFont, "_", box.x + 45 + MeasureTextCustomX(ui.mainFont, input, 50), box.y + 110, 50, goldColor);
        }
    }
}