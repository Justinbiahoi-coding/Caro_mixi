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
                    ui.currentScreen = 7; 
                    ui.activeInputField = 0;
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
                if (ui.activeInputField == 0 && ui.p1LetterCount < 15) {
                    ui.p1NameInput[ui.p1LetterCount] = (char)key;
                    ui.p1NameInput[ui.p1LetterCount+1] = '\0';
                    ui.p1LetterCount++;
                } else if (ui.activeInputField == 1 && ui.p2LetterCount < 15) {
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
}

void DrawMenuScreens(const GameState& game, const UIState& ui) {
    if (ui.currentScreen == 0) {
        DrawTexturePro(ui.bgMenu, { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

        Texture2D* textures[TOTAL_MENU_ITEMS] = { (Texture2D*)&ui.btnNewGame, (Texture2D*)&ui.btnLoadGame, (Texture2D*)&ui.btnSettings, (Texture2D*)&ui.btnHelp, (Texture2D*)&ui.btnCredits, (Texture2D*)&ui.btnExit };
        float centerX = 400.0f;
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
        const char* title = "CAI DAT DIEU KHIEN";
        int titleX = (1920 - MeasureText(title, 40)) / 2;
        DrawText(title, titleX, panelY - 80, 40, WHITE);

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
            int textWidth = MeasureText(setOptions[i], 30);
            int xPos = (1920 - textWidth) / 2;

            Color color = WHITE;
            if (i == 0 && game.inputType == 0) color = GREEN;
            if (i == 1 && game.inputType == 1) color = GREEN;
            if (i == ui.settingSelection) color = SKYBLUE;

            if (i == 3) { 
                const char* status = ui.musicEnabled ? " [ON]" : " [OFF]";
                DrawText(TextFormat("%s%s", setOptions[i], status), xPos, yPos, 30, color);
            }
            else {
                DrawText(setOptions[i], xPos, yPos, 30, color);
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
        DrawText("Credits: Thien", 100, 200, 40, DARKBLUE);
        DrawText("Nhan [M] de quay lai Menu", 250, 250, 20, GRAY);
    }
    else if (ui.currentScreen == 5) {
        DrawText("CHON FILE DE TAI", 800, 100, 50, DARKBLUE);
        DrawText("Dung [A]/[D] hoac [Trai]/[Phai] de chon. [ENTER] de Tai. [DEL] de Xoa. [M] ve Menu.", 450, 180, 25, DARKGRAY);

        int cardWidth = 360, cardHeight = 480, gap = 60;
        int startX = (1920 - (4 * cardWidth + 3 * gap)) / 2; 
        int startY = 300;

        for (int i = 0; i < 4; i++) {
            int currentX = startX + i * (cardWidth + gap);
            Rectangle cardRec = { (float)currentX, (float)startY, (float)cardWidth, (float)cardHeight };

            if (i == ui.loadSelection) {
                DrawRectangleRec(cardRec, Fade(SKYBLUE, 0.4f));     
                DrawRectangleLinesEx(cardRec, 6, BLUE);             
            } else {
                DrawRectangleRec(cardRec, Fade(LIGHTGRAY, 0.3f));   
                DrawRectangleLinesEx(cardRec, 2, GRAY);             
            }

            GameState tempGame;
            bool hasData = PeekGameSlot(i, tempGame);

            if (hasData) {
                int miniCell = 16; 
                int boardWidth = BOARD_SIZE * miniCell; 
                int boardOffsetX = currentX + (cardWidth - boardWidth) / 2;
                int boardOffsetY = startY + 30;

                DrawRectangle(boardOffsetX, boardOffsetY, boardWidth, boardWidth, WHITE);
                for(int r = 0; r < BOARD_SIZE; r++) {
                    for(int c = 0; c < BOARD_SIZE; c++) {
                        int cx = boardOffsetX + c * miniCell;
                        int cy = boardOffsetY + r * miniCell;
                        DrawRectangleLines(cx, cy, miniCell, miniCell, Fade(LIGHTGRAY, 0.5f));
                        if (tempGame.board[r][c].c == 1) {
                            DrawLine(cx + 2, cy + 2, cx + miniCell - 2, cy + miniCell - 2, RED);
                            DrawLine(cx + miniCell - 2, cy + 2, cx + 2, cy + miniCell - 2, RED);
                        } else if (tempGame.board[r][c].c == 2) {
                            DrawCircleLines(cx + miniCell/2, cy + miniCell/2, miniCell/2 - 2, BLUE);
                        }
                    }
                }

                const char* modeText = (tempGame.gameMode == 0) ? "Che Do: Co Dien" : "Che Do: Booming Caro";
                int textY = startY + boardWidth + 50; 
                DrawText(TextFormat("Slot %d", i+1), currentX + 20, textY, 30, DARKBLUE);
                DrawText(tempGame.saveName, currentX + 20, textY + 40, 25, BLACK);
                DrawText(modeText, currentX + 20, textY + 75, 20, DARKGRAY);
                DrawText(tempGame.saveTime, currentX + 20, textY + 105, 20, GRAY);
            } else {
                DrawText(TextFormat("Slot %d", i+1), currentX + 20, startY + 20, 30, GRAY);
                const char* emptyText = "--- TRONG ---";
                DrawText(emptyText, currentX + cardWidth/2 - MeasureText(emptyText, 25)/2, startY + cardHeight/2, 25, GRAY);
            }
        }
    }
    else if (ui.currentScreen == 6) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.8f));
        DrawText("LUU TRO CHOI", 800, 300, 40, WHITE);
        DrawText("1. Nhap ten cho ban luu nay (Toi da 29 ky tu):", 800, 380, 20, LIGHTGRAY);
        DrawRectangle(800, 420, 500, 50, WHITE);
        DrawText(ui.nameInput, 810, 435, 20, BLACK);
        
        if (ui.letterCount < 29 && ((int)(GetTime() * 2) % 2) == 0) {
            DrawText("_", 810 + MeasureText(ui.nameInput, 20), 435, 20, BLACK);
        }

        DrawText("2. Chon vi tri Slot de ghi de (Dung Len/Xuong):", 800, 520, 20, LIGHTGRAY);
        for (int i = 0; i < 4; i++) {
            int yPos = 560 + i * 50;
            GameState tempGame;
            bool hasData = PeekGameSlot(i, tempGame);
            Color textColor = (i == ui.saveSelection) ? YELLOW : DARKGRAY;
            const char* cursor = (i == ui.saveSelection) ? "=>" : "  ";

            if (hasData) DrawText(TextFormat("%s Slot %d: %s", cursor, i+1, tempGame.saveName), 800, yPos, 25, textColor);
            else DrawText(TextFormat("%s Slot %d: -- TRONG --", cursor, i+1), 800, yPos, 25, textColor);
        }
        DrawText("Nhan [ENTER] de luu. Nhan [ESC] de quay lai game.", 800, 800, 20, GRAY);
    }
    else if (ui.currentScreen == 7) {
        DrawTexturePro(ui.bgMenu, { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.85f)); 

        DrawText("THONG TIN NGUOI CHOI", 750, 300, 40, WHITE);
        
        //Player 1
        DrawText("Ten Player 1 (X) - Toi da 15 ky tu:", 750, 400, 25, LIGHTGRAY);
        Color p1BoxColor = (ui.activeInputField == 0) ? RAYWHITE : DARKGRAY;
        DrawRectangle(750, 440, 400, 50, p1BoxColor);
        DrawText(ui.p1NameInput, 760, 455, 25, BLACK);
        if (ui.activeInputField == 0 && ((int)(GetTime() * 2) % 2) == 0) {
            DrawText("_", 760 + MeasureText(ui.p1NameInput, 25), 455, 25, BLACK);
        }

        //Player 2
        DrawText("Ten Player 2 (O) - Toi da 15 ky tu:", 750, 530, 25, LIGHTGRAY);
        Color p2BoxColor = (ui.activeInputField == 1) ? RAYWHITE : DARKGRAY;
        DrawRectangle(750, 570, 400, 50, p2BoxColor);
        DrawText(ui.p2NameInput, 760, 585, 25, BLACK);
        if (ui.activeInputField == 1 && ((int)(GetTime() * 2) % 2) == 0) {
            DrawText("_", 760 + MeasureText(ui.p2NameInput, 25), 585, 25, BLACK);
        }

        DrawText("Dung phim [Len]/[Xuong] hoac [Tab] de chuyen o nhap.", 650, 700, 25, GRAY);
        DrawText("Nhan [ENTER] de bat dau tran chien!", 750, 750, 25, YELLOW);
        DrawText("Nhan [ESC] de quay lai Menu.", 850, 800, 20, DARKGRAY);
    }
}