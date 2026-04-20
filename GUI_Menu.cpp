#include "GUI_Menu.h"
#include "LogicControl.h"

const int TOTAL_MENU_ITEMS = 6;
const int TOTAL_SETTING_ITEMS = 5;

void UpdateMenuScreens(GameState& game, UIState& ui) {
    Vector2 mouse = GetMousePosition();

    if (ui.currentScreen == 0) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            ui.menuSelection--;
            if (ui.menuSelection < 0) ui.menuSelection = TOTAL_MENU_ITEMS - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            ui.menuSelection++;
            if (ui.menuSelection >= TOTAL_MENU_ITEMS) ui.menuSelection = 0;
        }

        float centerX = 400.0f; 
        float startY = 420.0f;
        float gap = ui.btnNewGame.height + 42.0f;

        Rectangle rects[TOTAL_MENU_ITEMS] = {
            { centerX - ui.btnNewGame.width/2.0f, startY, (float)ui.btnNewGame.width, (float)ui.btnNewGame.height },
            { centerX - ui.btnLoadGame.width/2.0f, startY + gap, (float)ui.btnLoadGame.width, (float)ui.btnLoadGame.height },
            { centerX - ui.btnSettings.width/2.0f, startY + gap*2, (float)ui.btnSettings.width, (float)ui.btnSettings.height },
            { centerX - ui.btnHelp.width/2.0f, startY + gap*3, (float)ui.btnHelp.width, (float)ui.btnHelp.height },
            { centerX - ui.btnCredits.width/2.0f, startY + gap*4, (float)ui.btnCredits.width, (float)ui.btnCredits.height },
            { centerX - ui.btnExit.width/2.0f, startY + gap*5, (float)ui.btnExit.width, (float)ui.btnExit.height }
        };

        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            if (CheckCollisionPointRec(mouse, rects[i])) ui.menuSelection = i; 
        }

        bool confirmSelection = false;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, rects[ui.menuSelection])) confirmSelection = true;
        if (IsKeyPressed(KEY_ENTER)) confirmSelection = true;

        if (confirmSelection) {
            switch (ui.menuSelection) {
                case 0: { 
                    ui.currentScreen = 8; 
                    ui.p1HeroSelection = 0;
                    ui.p2HeroSelection = 0; 
                    ui.selectionPhase = 0; 
                    
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

        const int barWidth = 340;
        const int barHeight = 18;
        const int barX = (1920 - barWidth) / 2;
        const int barY = 300 + 2 * 70 + 55;

        Rectangle volumeBar = { (float)barX, (float)barY, (float)barWidth, (float)barHeight };

        Rectangle knob = {
            barX + ui.musicVolume * (barWidth - 24.0f),
            (float)barY - 10.0f,
            24.0f,
            38.0f
        };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, volumeBar) ||
                CheckCollisionPointRec(mouse, knob)) {
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
        }

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
            ui.settingSelection = (ui.settingSelection - 1 + TOTAL_SETTING_ITEMS) % TOTAL_SETTING_ITEMS;

        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
            ui.settingSelection = (ui.settingSelection + 1) % TOTAL_SETTING_ITEMS;

        const char* setOptions[TOTAL_SETTING_ITEMS] = {
            "Dung Chuot (Khuyen Nghi)",
            "Dung Ban Phim (WASD + Enter)",
            "Am Luong Nhac",
            "Bat / Tat Nhac Nen",
            "Quay lai Menu"
        };

        Rectangle setRects[TOTAL_SETTING_ITEMS];
        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            int yPos = 300 + i * 70;
            if (i >= 3) yPos += 50;
            int textWidth = MeasureText(setOptions[i], 30);
            int xPos = (1920 - textWidth) / 2;
            setRects[i] = { (float)(xPos - 40), (float)(yPos - 18), (float)(textWidth + 80), 55 };

            if (CheckCollisionPointRec(mouse, setRects[i]))
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
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            ui.loadSelection--;
            if (ui.loadSelection < 0) ui.loadSelection = 3;
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
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
        // --- CẬP NHẬT ANIMATION CHO CẢ 2 TƯỚNG ---
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

        // --- LOGIC 4 BƯỚC TUẦN TỰ ---
        if (ui.selectionPhase == 0) {
            // BƯỚC 0: P1 CHỌN TƯỚNG
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
                ui.p1HeroSelection = (ui.p1HeroSelection == 0) ? 1 : 0;
            }
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 1; 
        }
        else if (ui.selectionPhase == 1) {
            // BƯỚC 1: P1 GÕ TÊN
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
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 2; 
        }
        else if (ui.selectionPhase == 2) {
            // BƯỚC 2: P2 CHỌN TƯỚNG
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
                ui.p2HeroSelection = (ui.p2HeroSelection == 0) ? 1 : 0;
            }
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 3; 
        }
        else if (ui.selectionPhase == 3) {
            // BƯỚC 3: P2 GÕ TÊN
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
            if (IsKeyPressed(KEY_ENTER)) {
                // VÀO GAME
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

        Texture2D* textures[TOTAL_MENU_ITEMS] = { (Texture2D*)&ui.btnNewGame, (Texture2D*)&ui.btnLoadGame, (Texture2D*)&ui.btnSettings, (Texture2D*)&ui.btnHelp, (Texture2D*)&ui.btnCredits, (Texture2D*)&ui.btnExit };
        float centerX = 1500.0f;
        float startY = 420.0f;
        float gap = ui.btnNewGame.height + 42.0f;
        Rectangle rectsToDraw[TOTAL_MENU_ITEMS] = {
            { centerX - ui.btnNewGame.width/2.0f, startY, (float)ui.btnNewGame.width, (float)ui.btnNewGame.height },
            { centerX - ui.btnLoadGame.width/2.0f, startY + gap, (float)ui.btnLoadGame.width, (float)ui.btnLoadGame.height },
            { centerX - ui.btnSettings.width/2.0f, startY + gap*2, (float)ui.btnSettings.width, (float)ui.btnSettings.height },
            { centerX - ui.btnHelp.width/2.0f, startY + gap*3, (float)ui.btnHelp.width, (float)ui.btnHelp.height },
            { centerX - ui.btnCredits.width/2.0f, startY + gap*4, (float)ui.btnCredits.width, (float)ui.btnCredits.height },
            { centerX - ui.btnExit.width/2.0f, startY + gap*5, (float)ui.btnExit.width, (float)ui.btnExit.height }
        };

        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            Color tint = (ui.menuSelection == i) ? YELLOW : WHITE;
            DrawTexture(*textures[i], (int)rectsToDraw[i].x, (int)rectsToDraw[i].y, tint);

        }
    } 
    else if (ui.currentScreen == 2) {
        static float cloudOffset = 0;
        static int direction = 1;

        cloudOffset += direction * 60 * GetFrameTime();

        DrawTexture(ui.bgSettings, -cloudOffset, 0, WHITE);
        DrawTexture(ui.bgSettings, ui.bgSettings.width - cloudOffset, 0, WHITE);
        if (cloudOffset >= ui.bgSettings.width) direction = -1;
        if (cloudOffset <= 0) direction = 1;

        int panelW = 700;
        int panelH = 550;
        int panelX = (1920 - panelW) / 2;
        int panelY = 250;

        DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.6f));
        DrawRectangleLines(panelX, panelY, panelW, panelH, WHITE);
        const char* title = "CONTROL SETTINGS";
        int titleX = (1920 - MeasureTextCustomX(ui.mainFont, title, 40)) / 2;
        DrawTextCustom(ui.mainFont, title, titleX, panelY - 80, 80, WHITE);

        const char* setOptions[TOTAL_SETTING_ITEMS] = {
          "Dung Chuot (Khuyen Nghi)",
          "Dung Ban Phim (WASD + Enter)",
          "Am Luong Nhac",
          "Bat / Tat Nhac Nen",
          "Quay lai Menu"
        };

        Vector2 mouse = GetMousePosition();

        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            int yPos = 300 + i * 70;
            if (i >= 3) yPos += 50;
            int textWidth = MeasureTextCustomX(ui.mainFont, setOptions[i], 30);
            int xPos = (1920 - textWidth) / 2;

            Color color = WHITE;
            if (i == 0 && game.inputType == 0) color = GREEN;
            if (i == 1 && game.inputType == 1) color = GREEN;
            if (i == ui.settingSelection) color = SKYBLUE;

            if (i == 3) { 
                const char* status = ui.musicEnabled ? " [ON]" : " [OFF]";
                DrawTextCustom(ui.mainFont, TextFormat("%s%s", setOptions[i], status), xPos, yPos, 30, color);
            }
            else {
                DrawTextCustom(ui.mainFont, setOptions[i], xPos, yPos, 30, color);
            }
        }

        const int barWidth = 340;
        const int barHeight = 18;
        const int barX = (1920 - barWidth) / 2;
        const int barY = 300 + 2 * 70 + 55;

        Rectangle volumeBar = { (float)barX, (float)barY, (float)barWidth, (float)barHeight };

        DrawRectangleRec(volumeBar, DARKGRAY);
        DrawRectangle(barX, barY, (int)(barWidth* ui.musicVolume), barHeight, LIME);

        float knobX = barX + ui.musicVolume * (barWidth - 20.0f);
        Rectangle knob = { knobX, (float)barY - 9, 20, 34 };

        DrawRectangleRec(knob, ui.draggingVolume ? YELLOW : RAYWHITE);
        DrawRectangleLinesEx(knob, 3, BLACK);
        DrawText(TextFormat("%d%%", (int)(ui.musicVolume * 100)), barX + barWidth + 30, barY - 15, 26, WHITE);
    }
    else if (ui.currentScreen == 3) {
        DrawTextCustom(ui.mainFont, "Credits: Thien, @b1nhan", 100, 200, 40, DARKBLUE);
        DrawTextCustom(ui.mainFont, "Nhan [M] de quay lai Menu", 250, 250, 20, GRAY);
    }
   else if (ui.currentScreen == 5) {
        // --- MA THUẬT CUỘN MƯỢT (SMOOTH SCROLL) ---
        // Biến static này sẽ tự động chạy theo ui.loadSelection để tạo hiệu ứng trượt
        static float smoothScroll = (float)ui.loadSelection;
        smoothScroll += (ui.loadSelection - smoothScroll) * 12.0f * GetFrameTime();

        // --- 2. VẼ ẢNH NỀN (Có hiệu ứng Parallax trượt nhẹ theo thẻ) ---
        float bgParallax = -smoothScroll * 60.0f; 
        DrawTexturePro(ui.bgLoadGame, { 0, 0, (float)ui.bgLoadGame.width, (float)ui.bgLoadGame.height }, { bgParallax, 0, 1920.0f + 250.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.6f)); // Lớp sương mù làm nền tối đi

        Color goldColor = { 253, 249, 0, 255 }; 
        Color silverColor = { 200, 200, 200, 255 };

        // Tiêu đề
        const char* title = "CHON FILE DE TAI";
        int titleW = MeasureTextCustomX(ui.mainFont, title, 50);
        DrawTextCustom(ui.mainFont, title, 1920 / 2 - titleW / 2, 40, 50, goldColor);
        
        const char* guide = "Dung [A]/[D] de truot the. [ENTER] de Tai. [DEL] de Xoa. [M] ve Menu.";
        int guideW = MeasureTextCustomX(ui.mainFont, guide, 22);
        DrawTextCustom(ui.mainFont, guide, 1920 / 2 - guideW / 2, 100, 22, silverColor);

        // --- 3. GIAO DIỆN CAROUSEL (BĂNG CHUYỀN) ---
        int cardWidth = 600;  // Thẻ siêu to khổng lồ
        int cardHeight = 800; 
        float spacing = 800.0f; // Khoảng cách giữa các thẻ khi trượt
        float startY = 160.0f;

        for (int i = 0; i < 4; i++) {
            // Tọa độ X được tính toán dựa trên độ trượt (smoothScroll)
            float offsetX = (i - smoothScroll) * spacing;
            float currentX = (1920.0f / 2.0f) - (cardWidth / 2.0f) + offsetX;

            Rectangle cardRec = { currentX, startY, (float)cardWidth, (float)cardHeight };

            GameState tempGame;
            bool hasData = PeekGameSlot(i, tempGame);

            // Xác định độ mờ: Thẻ ở giữa sẽ sáng (alpha=1), thẻ dạt ra biên sẽ mờ dần (alpha=0.3)
            float distance = fabs(i - smoothScroll);
            float alpha = 1.0f - (distance * 0.7f);
            if (alpha < 0.2f) alpha = 0.2f;

            // Vẽ Nền thẻ (Phủ pha lê đen) & Viền Kép
            DrawRectangleRec(cardRec, Fade(BLACK, 0.85f * alpha));
            Color borderColor = (i == ui.loadSelection) ? goldColor : silverColor;
            DrawRectangleLinesEx(cardRec, 3.0f, Fade(borderColor, alpha));
            DrawRectangleLinesEx({cardRec.x + 8, cardRec.y + 8, cardRec.width - 16, cardRec.height - 16}, 1.0f, Fade(borderColor, alpha * 0.4f));

            if (i == ui.loadSelection) {
                DrawRectangleRec(cardRec, Fade(goldColor, 0.05f)); 
            }

            if (hasData) {
                // --- HEADER ---
                DrawTextCustom(ui.mainFont, TextFormat("SLOT %d", i + 1), currentX + 30, startY + 30, 40, Fade(goldColor, alpha));
                DrawTextCustom(ui.mainFont, tempGame.saveTime, currentX + cardWidth - MeasureTextCustomX(ui.mainFont, tempGame.saveTime, 22) - 30, startY + 45, 22, Fade(silverColor, alpha));
                DrawLineEx({currentX + 30, startY + 90}, {currentX + cardWidth - 30, startY + 90}, 2.0f, Fade(silverColor, alpha * 0.3f));

                // --- MIDDLE: SA BÀN KHỔNG LỒ CỰC RÕ NÉT ---
                int nameW = MeasureTextCustomX(ui.mainFont, tempGame.saveName, 45);
                DrawTextCustom(ui.mainFont, tempGame.saveName, currentX + cardWidth/2 - nameW/2, startY + 120, 45, Fade(WHITE, alpha));

                float miniBoardSize = 420.0f; // Bàn cờ to gần gấp đôi bản cũ
                float mbX = currentX + (cardWidth - miniBoardSize) / 2.0f;
                float mbY = startY + 200.0f;

                // Khung bàn cờ
                DrawTexturePro(ui.boardFrame, { 0, 0, (float)ui.boardFrame.width, (float)ui.boardFrame.height }, { mbX, mbY, miniBoardSize, miniBoardSize }, {0,0}, 0.0f, Fade(WHITE, alpha));

                // Tính toán tỷ lệ ô cờ
                float innerX = mbX + miniBoardSize * 0.073f;
                float innerY = mbY + miniBoardSize * 0.13f;
                float innerW = miniBoardSize * (1.0f - 0.073f - 0.070f);
                float innerH = miniBoardSize * (1.0f - 0.13f - 0.050f);
                float mCellW = innerW / BOARD_SIZE;
                float mCellH = innerH / BOARD_SIZE;

                // Vẽ 225 ô gạch và các quân cờ
                for (int r = 0; r < BOARD_SIZE; r++) {
                    for (int c = 0; c < BOARD_SIZE; c++) {
                        Rectangle dest = { innerX + c * mCellW, innerY + r * mCellH, mCellW, mCellH };
                        DrawTexturePro(ui.cell, {0,0,(float)ui.cell.width,(float)ui.cell.height}, dest, {0,0}, 0.0f, Fade(WHITE, alpha));
                        
                        if (tempGame.board[r][c].c == 1) DrawTexturePro(ui.pieceX, {0,0,(float)ui.pieceX.width,(float)ui.pieceX.height}, dest, {0,0}, 0.0f, Fade(WHITE, alpha));
                        if (tempGame.board[r][c].c == 2) DrawTexturePro(ui.pieceO, {0,0,(float)ui.pieceO.width,(float)ui.pieceO.height}, dest, {0,0}, 0.0f, Fade(WHITE, alpha));
                    }
                }

                // --- BOTTOM ---
                DrawLineEx({currentX + 30, mbY + miniBoardSize + 40}, {currentX + cardWidth - 30, mbY + miniBoardSize + 40}, 2.0f, Fade(silverColor, alpha * 0.3f));
                
                const char* modeText = (tempGame.gameMode == 0) ? "Che Do: Co Dien" : "Che Do: Booming Caro";
                DrawTextCustom(ui.mainFont, modeText, currentX + 40, mbY + miniBoardSize + 70, 30, Fade(WHITE, alpha));
                DrawTextCustom(ui.mainFont, TextFormat("So Vong: %d", tempGame.roundCount), currentX + cardWidth - 200, mbY + miniBoardSize + 70, 30, Fade(WHITE, alpha));

            } else {
                // Thẻ trống
                const char* emptyText = "--- SLOT TRONG ---";
                int emptyW = MeasureTextCustomX(ui.mainFont, emptyText, 40);
                DrawTextCustom(ui.mainFont, emptyText, currentX + cardWidth/2 - emptyW/2, startY + cardHeight/2, 40, Fade(silverColor, alpha));
            }
        }
    }
    else if (ui.currentScreen == 6) {

        // ==========================================
        // 2. VẼ GIAO DIỆN (Valhalla AAA Style)
        // ==========================================
        
        // 1. Vẽ Ảnh nền Tuyệt đẹp và phủ một lớp đen mờ 50% để nổi chữ
        DrawTexturePro(ui.bgSaveLoad, { 0, 0, (float)ui.bgSaveLoad.width, (float)ui.bgSaveLoad.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.5f));

        // Màu sắc đặc trưng của Valhalla
        Color valhallaTeal = { 40, 200, 200, 255 };  // Xanh Cyan ngọc
        Color textColor = { 230, 230, 230, 255 };    // Trắng hơi xám
        Color mutedText = { 150, 150, 150, 255 };    // Xám mờ

        // --- TIÊU ĐỀ BÊN TRÁI ---
        DrawTextCustom(ui.mainFont, "LUU TRO CHOI", 250, 120, 60, WHITE);
        DrawLineEx({250, 190}, {1670, 190}, 2.0f, Fade(WHITE, 0.2f)); // Đường kẻ mảnh chia layout

        // --- Ô NHẬP TÊN (Minimalist) ---
        DrawTextCustom(ui.mainFont, "Nhap ten ban luu:", 250, 230, 30, valhallaTeal);
        Rectangle inputBox = { 550, 220, 1120, 50 };
        // Không vẽ hộp nữa, chỉ vẽ một đường kẻ gạch dưới (Underline)
        DrawLineEx({inputBox.x, inputBox.y + inputBox.height}, {inputBox.x + inputBox.width, inputBox.y + inputBox.height}, 1.0f, Fade(WHITE, 0.3f));
        DrawTextCustom(ui.mainFont, ui.nameInput, inputBox.x + 10, inputBox.y + 10, 35, WHITE);
        
        if (ui.letterCount < 29 && ((int)(GetTime() * 2) % 2) == 0) {
            int textW = MeasureTextCustomX(ui.mainFont, ui.nameInput, 35);
            DrawTextCustom(ui.mainFont, "_", inputBox.x + 10 + textW, inputBox.y + 10, 35, valhallaTeal);
        }

        // --- DANH SÁCH SLOT (List không viền) ---
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

            // Kẻ đường kẻ ngang mỏng ở ĐỈNH mỗi Slot
            DrawLineEx({(float)startX, (float)currentY}, {(float)(startX + slotWidth), (float)currentY}, 1.0f, Fade(WHITE, 0.15f));

            if (i == ui.saveSelection) {
                // Hiệu ứng Hover: Nền sáng mờ và Vệt kẻ dọc (Vertical Highlight) đặc trưng AAA
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

                // Thời gian ép sang phải
                int dateW = MeasureTextCustomX(ui.mainFont, tempGame.saveTime, 25);
                DrawTextCustom(ui.mainFont, tempGame.saveTime, startX + slotWidth - dateW - 20, currentY + 35, 25, (i == ui.saveSelection) ? valhallaTeal : mutedText);
            } else {
                Color emptyColor = (i == ui.saveSelection) ? valhallaTeal : Fade(mutedText, 0.5f);
                DrawTextCustom(ui.mainFont, "Slot Trong", startX + 40, currentY + 30, 40, emptyColor);
            }
        }
        
        // Đường kẻ ngang cuối cùng chốt sổ danh sách
        DrawLineEx({(float)startX, (float)(startY + 4 * (slotHeight + gap))}, {(float)(startX + slotWidth), (float)(startY + 4 * (slotHeight + gap))}, 1.0f, Fade(WHITE, 0.15f));

        // --- FOOTER CHỈ DẪN ---
        const char* footerBtn = "Nhan [ENTER] de Luu      Nhan [ESC] de Quay lai";
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

        DrawTextCustom(ui.mainFont, "Dung phim [Len]/[Xuong] hoac [Tab] de chuyen o nhap.", 650, 700, 25, GRAY);
        DrawTextCustom(ui.mainFont, "Nhan [ENTER] de bat dau tran chien!", 750, 750, 25, YELLOW);
        DrawTextCustom(ui.mainFont, "Nhan [ESC] de quay lai Menu.", 850, 800, 20, DARKGRAY);
    }
    else if (ui.currentScreen == 8) {
        DrawTexturePro(ui.bgSaveLoad, { 0, 0, (float)ui.bgSaveLoad.width, (float)ui.bgSaveLoad.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.7f)); 

        Color goldColor = { 253, 249, 0, 255 }; 
        Color silverColor = { 200, 200, 200, 255 };
        Color darkSilver = { 100, 100, 100, 255 };

        // Tiêu đề
        const char* title = "TIEU SU CHIEN BINH";
        int titleW = MeasureTextCustomX(ui.mainFont, title, 50);
        DrawTextCustom(ui.mainFont, title, 1920 / 2 - titleW / 2, 80, 50, goldColor);

        int pedestalY = 700;
        DrawEllipseLines(1920/2 - 400, pedestalY, 200, 50, Fade(silverColor, 0.5f)); 
        DrawEllipseLines(1920/2 + 400, pedestalY, 200, 50, Fade(silverColor, 0.5f)); 

        // Hàm helper vẽ Sprite CÓ HIỆU ỨNG CHUYỂN ĐỘNG (Cắt đúng Frame)
        auto DrawBigSprite = [&](CharAnim charAnim, int x, int y) {
            Rectangle source = { (float)charAnim.currentFrame * charAnim.frameWidth, 0.0f, (float)charAnim.frameWidth, (float)charAnim.frameHeight }; 
            Rectangle dest = { (float)x - (charAnim.frameWidth*6)/2.0f, (float)y - charAnim.frameHeight*6 + 25, (float)charAnim.frameWidth * 6, (float)charAnim.frameHeight * 6 }; 
            DrawTexturePro(charAnim.spriteSheet, source, dest, {0,0}, 0.0f, WHITE);
        };

        // Hàm helper vẽ mũi tên Slider nhấp nháy
        auto DrawSliderArrows = [&](int x, int y) {
            if (((int)(GetTime() * 3) % 2) == 0) { // Nhấp nháy theo thời gian
                DrawTextCustom(ui.mainFont, "<", x - 180, y - 180, 60, silverColor);
                DrawTextCustom(ui.mainFont, ">", x + 150, y - 180, 60, silverColor);
            }
        };

        // --- HIỂN THỊ P1 ---
        if (ui.selectionPhase <= 1) {
            if (ui.p1HeroSelection == 0) DrawBigSprite(ui.charP1, 1920/2 - 400, pedestalY);
            else DrawBigSprite(ui.charP2, 1920/2 - 400, pedestalY);
            
            if (ui.selectionPhase == 0) DrawSliderArrows(1920/2 - 400, pedestalY); // Hiện mũi tên lúc chọn

            const char* heroName = (ui.p1HeroSelection == 0) ? "Hiep Si Den" : "Kiem Khach Lua";
            DrawTextCustom(ui.mainFont, heroName, 1920/2 - 400 - MeasureTextCustomX(ui.mainFont, heroName, 30)/2, pedestalY + 50, 30, silverColor);
        } else {
            if (ui.p1HeroSelection == 0) DrawBigSprite(ui.charP1, 1920/2 - 400, pedestalY);
            else DrawBigSprite(ui.charP2, 1920/2 - 400, pedestalY);
            DrawTextCustom(ui.mainFont, ui.p1NameInput, 1920/2 - 400 - MeasureTextCustomX(ui.mainFont, ui.p1NameInput, 35)/2, pedestalY + 50, 35, goldColor);
        }

        // --- HIỂN THỊ P2 ---
        if (ui.selectionPhase >= 2) {
            if (ui.p2HeroSelection == 0) DrawBigSprite(ui.charP1, 1920/2 + 400, pedestalY);
            else DrawBigSprite(ui.charP2, 1920/2 + 400, pedestalY);
            
            if (ui.selectionPhase == 2) DrawSliderArrows(1920/2 + 400, pedestalY); // Hiện mũi tên lúc chọn
            
            if (ui.selectionPhase == 2) {
                const char* heroName = (ui.p2HeroSelection == 0) ? "Hiep Si Den" : "Kiem Khach Lua";
                DrawTextCustom(ui.mainFont, heroName, 1920/2 + 400 - MeasureTextCustomX(ui.mainFont, heroName, 30)/2, pedestalY + 50, 30, silverColor);
            } else {
                DrawTextCustom(ui.mainFont, ui.p2NameInput, 1920/2 + 400 - MeasureTextCustomX(ui.mainFont, ui.p2NameInput, 35)/2, pedestalY + 50, 35, goldColor);
            }
        }

        // --- BẢNG ĐIỀU KHIỂN ---
        int boxY = 820;
        DrawRectangle(0, boxY, 1920, 1080 - boxY, Fade(BLACK, 0.85f));
        DrawLineEx({0, (float)boxY}, {1920, (float)boxY}, 2.0f, darkSilver);

        if (ui.selectionPhase == 0) {
            DrawTextCustom(ui.mainFont, "LUOT NGUOI CHOI 1 (X) - CHON TUONG", 1920/2 - MeasureTextCustomX(ui.mainFont, "LUOT NGUOI CHOI 1 (X) - CHON TUONG", 35)/2, boxY + 40, 35, goldColor);
            DrawTextCustom(ui.mainFont, "Dung [A]/[D] hoac [Trai]/[Phai] de xem. Nhan [ENTER] de Chot.", 1920/2 - MeasureTextCustomX(ui.mainFont, "Dung [A]/[D] hoac [Trai]/[Phai] de xem. Nhan [ENTER] de Chot.", 25)/2, boxY + 110, 25, silverColor);
        }
        else if (ui.selectionPhase == 1) {
            DrawTextCustom(ui.mainFont, "LUOT NGUOI CHOI 1 (X) - NHAP TEN TUNG HOANG", 1920/2 - MeasureTextCustomX(ui.mainFont, "LUOT NGUOI CHOI 1 (X) - NHAP TEN TUNG HOANG", 35)/2, boxY + 40, 35, goldColor);
            DrawTextCustom(ui.mainFont, ui.p1NameInput, 1920/2 - MeasureTextCustomX(ui.mainFont, ui.p1NameInput, 40)/2, boxY + 110, 40, WHITE);
            if (((int)(GetTime() * 2) % 2) == 0) DrawTextCustom(ui.mainFont, "_", 1920/2 + MeasureTextCustomX(ui.mainFont, ui.p1NameInput, 40)/2 + 5, boxY + 110, 40, goldColor);
        }
        else if (ui.selectionPhase == 2) {
             DrawTextCustom(ui.mainFont, "LUOT NGUOI CHOI 2 (O) - CHON TUONG", 1920/2 - MeasureTextCustomX(ui.mainFont, "LUOT NGUOI CHOI 2 (O) - CHON TUONG", 35)/2, boxY + 40, 35, goldColor);
             DrawTextCustom(ui.mainFont, "Dung [A]/[D] hoac [Trai]/[Phai] de xem. Nhan [ENTER] de Chot.", 1920/2 - MeasureTextCustomX(ui.mainFont, "Dung [A]/[D] hoac [Trai]/[Phai] de xem. Nhan [ENTER] de Chot.", 25)/2, boxY + 110, 25, silverColor);
        }
        else if (ui.selectionPhase == 3) {
            DrawTextCustom(ui.mainFont, "LUOT NGUOI CHOI 2 (O) - NHAP TEN TUNG HOANG", 1920/2 - MeasureTextCustomX(ui.mainFont, "LUOT NGUOI CHOI 2 (O) - NHAP TEN TUNG HOANG", 35)/2, boxY + 40, 35, goldColor);
            DrawTextCustom(ui.mainFont, ui.p2NameInput, 1920/2 - MeasureTextCustomX(ui.mainFont, ui.p2NameInput, 40)/2, boxY + 110, 40, WHITE);
            if (((int)(GetTime() * 2) % 2) == 0) DrawTextCustom(ui.mainFont, "_", 1920/2 + MeasureTextCustomX(ui.mainFont, ui.p2NameInput, 40)/2 + 5, boxY + 110, 40, goldColor);
        }
    }
}