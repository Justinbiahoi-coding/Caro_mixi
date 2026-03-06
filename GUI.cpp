#include "GUI.h"
#include "LogicControl.h"
const int CELL_SIZE = 30;
const int OFFSET_X = 50; 
const int OFFSET_Y = 50; 
const int TOTAL_MENU_ITEMS = 6;
const int TOTAL_SETTING_ITEMS = 3;

void InitGUI(UIState& ui) {
    ui.currentScreen = 0;
    ui.menuSelection = 0;
    ui.settingSelection = 0;
    ui.loadSelection = 0;
    ui.saveSelection = 0;
    ui.nameInput[0] = '\0';
    ui.letterCount = 0;
    ui.shouldExit = false;

    // Load assets
    ui.bgMenu      = LoadTexture("assets/background-new.png");
    ui.btnNewGame  = LoadTexture("assets/menu/NewGame.png");
    ui.btnLoadGame = LoadTexture("assets/menu/LoadGame.png");
    ui.btnSettings = LoadTexture("assets/menu/Settings.png");
    ui.btnHelp     = LoadTexture("assets/menu/Help.png");
    ui.btnCredits  = LoadTexture("assets/menu/Credits.png");
    ui.btnExit     = LoadTexture("assets/menu/Exit.png");
}

void UnloadGUI(UIState& ui) {
    UnloadTexture(ui.bgMenu);
    UnloadTexture(ui.btnNewGame);
    UnloadTexture(ui.btnLoadGame);
    UnloadTexture(ui.btnSettings);
    UnloadTexture(ui.btnHelp);
    UnloadTexture(ui.btnCredits);
    UnloadTexture(ui.btnExit);
}

void UpdateGUI(GameState& game, UIState& ui) {
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

        float centerX = 1496.2f;
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
                    int savedInput = game.inputType;
                    InitGame(game, 0);
                    game.inputType = savedInput;
                    ui.currentScreen = 1;
                } break;
                case 1: ui.currentScreen = 5; ui.loadSelection = 0; break;
                case 2: ui.currentScreen = 2; ui.settingSelection = game.inputType; break;
                case 3: break;
                case 4: ui.currentScreen = 3; break;
                case 5: ui.shouldExit = true; break;
            }
        }
    }
    else if (ui.currentScreen == 1) {
        // --- LOGIC ĐÁNH CỜ TẠI ĐÂY ---
        if (game.inputType == 0) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (mouse.x >= OFFSET_X && mouse.x < OFFSET_X + BOARD_SIZE * CELL_SIZE &&
                    mouse.y >= OFFSET_Y && mouse.y < OFFSET_Y + BOARD_SIZE * CELL_SIZE) {
                    int col = ((int)mouse.x - OFFSET_X) / CELL_SIZE;
                    int row = ((int)mouse.y - OFFSET_Y) / CELL_SIZE;
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

        if (IsKeyPressed(KEY_M)) ui.currentScreen = 0; 
        if (IsKeyPressed(KEY_L)) {
            ui.currentScreen = 6; 
            ui.nameInput[0] = '\0'; 
            ui.letterCount = 0;
            ui.saveSelection = 0;
        }
    }
    else if (ui.currentScreen == 2) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            ui.settingSelection--;
            if (ui.settingSelection < 0) ui.settingSelection = TOTAL_SETTING_ITEMS - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            ui.settingSelection++;
            if (ui.settingSelection >= TOTAL_SETTING_ITEMS) ui.settingSelection = 0;
        }

        Rectangle setRects[TOTAL_SETTING_ITEMS] = {
            { 250, 250, 600, 40 }, { 250, 310, 600, 40 }, { 250, 370, 600, 40 }  
        };

        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            if (CheckCollisionPointRec(mouse, setRects[i])) ui.settingSelection = i;
        }

        bool confirmSet = false;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, setRects[ui.settingSelection])) confirmSet = true;
        if (IsKeyPressed(KEY_ENTER)) confirmSet = true;

        if (confirmSet) {
            if (ui.settingSelection == 0) game.inputType = 0; 
            else if (ui.settingSelection == 1) game.inputType = 1; 
            else if (ui.settingSelection == 2) ui.currentScreen = 0; 
        }
        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0;
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
}

void DrawGUI(const GameState& game, const UIState& ui) {
    if (ui.currentScreen == 0) {
        DrawTexturePro(ui.bgMenu, { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

        Texture2D* textures[TOTAL_MENU_ITEMS] = { (Texture2D*)&ui.btnNewGame, (Texture2D*)&ui.btnLoadGame, (Texture2D*)&ui.btnSettings, (Texture2D*)&ui.btnHelp, (Texture2D*)&ui.btnCredits, (Texture2D*)&ui.btnExit };
        float centerX = 1496.2f;
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
    else if (ui.currentScreen == 1) {
        //Draw board
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                int x = OFFSET_X + j * CELL_SIZE;
                int y = OFFSET_Y + i * CELL_SIZE;
                DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, LIGHTGRAY);
                if (game.board[i][j].c == 1) {
                    DrawLine(x + 5, y + 5, x + CELL_SIZE - 5, y + CELL_SIZE - 5, RED);
                    DrawLine(x + CELL_SIZE - 5, y + 5, x + 5, y + CELL_SIZE - 5, RED);
                } else if (game.board[i][j].c == 2) {
                    DrawCircleLines(x + CELL_SIZE / 2, y + CELL_SIZE / 2, CELL_SIZE / 2 - 4, BLUE);
                }
            }
        }

        if (game.inputType == 1) {
            int cx = OFFSET_X + game.cursorCol * CELL_SIZE;
            int cy = OFFSET_Y + game.cursorRow * CELL_SIZE;
            DrawRectangleLines(cx, cy, CELL_SIZE, CELL_SIZE, DARKGREEN);
            DrawRectangle(cx + 1, cy + 1, CELL_SIZE - 2, CELL_SIZE - 2, Fade(GREEN, 0.3f));
        }

        //board infor
        int infoX = 1400; 
        DrawText("THONG TIN VAN DAU", infoX, 50, 20, BLACK);
        if (game.isPlayer1Turn) DrawText("Luot cua: X (P1)", infoX, 100, 20, RED);
        else DrawText("Luot cua: O (P2)", infoX, 100, 20, BLUE);

        DrawText(TextFormat("So buoc P1: %d", game.player1.stepCount), infoX, 150, 20, DARKGRAY);
        DrawText(TextFormat("So buoc P2: %d", game.player2.stepCount), infoX, 180, 20, DARKGRAY);
        
        if (game.inputType == 0) DrawText("Dieu khien: Chuot", infoX, 230, 20, GRAY);
        else DrawText("Dieu khien: WASD + Enter", infoX, 230, 20, GRAY);
        
        DrawText("Bam [L] de luu game", infoX, 250, 20, DARKGRAY);
        DrawText("Nhan [M] de ve Menu", infoX, 550, 20, GRAY);

        if (game.matchStatus == 1) DrawText("P1 (X) THANG!", infoX, 300, 30, RED);
        if (game.matchStatus == 2) DrawText("P2 (O) THANG!", infoX, 300, 30, BLUE);
        if (game.matchStatus == 3) DrawText("HOA NHAU!", infoX, 300, 30, GRAY);
    }
    else if (ui.currentScreen == 2) {
        DrawText("CAI DAT DIEU KHIEN", 200, 150, 40, DARKBLUE);
        const char* setOptions[TOTAL_SETTING_ITEMS] = { "Dung Chuot (Khuyen Nghi)", "Dung Ban Phim (WASD + Enter)", "Quay lai Menu" };
        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            int yPos = 250 + i * 60; 
            const char* prefix = "   ";
            if (i == 0 && game.inputType == 0) prefix = "[X]";
            else if (i == 0) prefix = "[ ]";
            if (i == 1 && game.inputType == 1) prefix = "[X]";
            else if (i == 1) prefix = "[ ]";

            Color textColor = (i == ui.settingSelection) ? RED : DARKGRAY;
            const char* pointer = (i == ui.settingSelection) ? "->" : "  ";
            DrawText(TextFormat("%s %s %s", pointer, prefix, setOptions[i]), 210, yPos, 30, textColor);
        }
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
}