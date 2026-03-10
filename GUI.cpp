#include "GUI.h"
#include "LogicControl.h"

// const int CELL_SIZE = 45; 
// const int OFFSET_X = (1920 - 15 * CELL_SIZE) / 2; 
// const int OFFSET_Y = (1080 - 15 * CELL_SIZE) / 2; 
const int TOTAL_MENU_ITEMS = 6;
const int TOTAL_SETTING_ITEMS = 3;

void InitGUI(UIState& ui) {
    ui.currentScreen = 0;
    ui.menuSelection = 0;
    ui.settingSelection = 0;
    ui.loadSelection = 0;
    ui.saveSelection = 0;
    ui.endGameSelection = 0; 
    ui.nameInput[0] = '\0';
    ui.letterCount = 0;
    ui.shouldExit = false;
    
    ui.p1NameInput[0] = '\0';
    ui.p2NameInput[0] = '\0';
    ui.p1LetterCount = 0;
    ui.p2LetterCount = 0;
    ui.activeInputField = 0;
    // Load asset for menu
    ui.bgMenu      = LoadTexture("assets/background-new.jpg");
  
    ui.btnNewGame  = LoadTexture("assets/menu/NewGame.png");
    ui.btnLoadGame = LoadTexture("assets/menu/LoadGame.png");
    ui.btnSettings = LoadTexture("assets/menu/Settings.png");
    ui.btnHelp     = LoadTexture("assets/menu/Help.png");
    ui.btnCredits  = LoadTexture("assets/menu/Credits.png");
    ui.btnExit     = LoadTexture("assets/menu/Exit.png");
    //load asset in game
    ui.bgGame      = LoadTexture("assets/bgboard/bg_game.png"); 
    //Load asset for board 
    ui.boardFrame = LoadTexture("assets/board/board_frame.png");
    ui.cell       = LoadTexture("assets/board/cell_custom.png");
    ui.pieceX     = LoadTexture("assets/board/piece_x.png");
    ui.pieceO     = LoadTexture("assets/board/piece_o.png");

    ui.cellSize = 50.0f; 
    ui.cellStartX = (1920.0f - BOARD_SIZE * ui.cellSize) / 2.0f; 
    ui.cellStartY = (1080.0f - BOARD_SIZE * ui.cellSize) / 2.0f; 

   //Layout
    ui.cellSize = 44.0f; // size of cell
    float gridWidth = BOARD_SIZE *1.0275*ui.cellSize;  // board'width
    float gridHeight = BOARD_SIZE *1.0275*ui.cellSize; // board'height

    
    float margin_Left   = 0.073f;  
    float margin_Right  = 0.07f;  
    float margin_Top    = 0.13f;  
    float margin_Bottom = 0.050f; 

    ui.boardFrameRec.width = gridWidth / (1.0f - margin_Left - margin_Right);
    ui.boardFrameRec.height = gridHeight / (1.0f - margin_Top - margin_Bottom);

    ui.boardFrameRec.x = (1920.0f - ui.boardFrameRec.width) / 2.0f;
    ui.boardFrameRec.y = (1080.0f - ui.boardFrameRec.height) / 2.0f + 30.0f; 

    ui.cellStartX = ui.boardFrameRec.x + (ui.boardFrameRec.width * margin_Left);
    ui.cellStartY = ui.boardFrameRec.y + (ui.boardFrameRec.height * margin_Top);

}

void UnloadGUI(UIState& ui) {
    //unload menu
    UnloadTexture(ui.bgMenu);
    UnloadTexture(ui.btnNewGame);
    UnloadTexture(ui.btnLoadGame);
    UnloadTexture(ui.btnSettings);
    UnloadTexture(ui.btnHelp);
    UnloadTexture(ui.btnCredits);
    UnloadTexture(ui.btnExit);
    //unload board
    UnloadTexture(ui.boardFrame);
    UnloadTexture(ui.cell);
    UnloadTexture(ui.pieceX);
    UnloadTexture(ui.pieceO);
    //ul bg game
    UnloadTexture(ui.bgGame);
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
                case 0: { // New Game
                    ui.currentScreen = 7; 
                    ui.activeInputField = 0;
                    ui.p1NameInput[0] = '\0'; ui.p1LetterCount = 0;
                    ui.p2NameInput[0] = '\0'; ui.p2LetterCount = 0;
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
        if (game.matchStatus == 0) { // TRẠNG THÁI: ĐANG CHƠI
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
        else { // TRẠNG THÁI: GAME OVER (Xử lý 2 nút bấm)
            Vector2 mouse = GetMousePosition();
            
            // 1. Phím A/D
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) ui.endGameSelection = 0;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) ui.endGameSelection = 1;

            // 2. Định nghĩa tọa độ 2 nút bấm
            Rectangle btnPlayAgain = { 720, 560, 220, 50 };
            Rectangle btnMenu = { 980, 560, 220, 50 };

            // 3. Hover chuột
            if (CheckCollisionPointRec(mouse, btnPlayAgain)) ui.endGameSelection = 0;
            if (CheckCollisionPointRec(mouse, btnMenu)) ui.endGameSelection = 1;

            // 4. Click hoặc Enter
            bool confirm = false;
            if (IsKeyPressed(KEY_ENTER)) confirm = true;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && 
               (CheckCollisionPointRec(mouse, btnPlayAgain) || CheckCollisionPointRec(mouse, btnMenu))) {
                confirm = true;
            }

            // 5. Thực thi
            if (confirm) {
                if (ui.endGameSelection == 0) {
                    ResetRound(game); // Gọi hàm reset bàn cờ
                } else {
                    ui.currentScreen = 0; // Trở về Menu
                }
            }
        }

        // Các phím tắt M và L giữ nguyên (có thể hoạt động mọi lúc)
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

void DrawGUI(const GameState& game, const UIState& ui) {
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
    else if (ui.currentScreen == 1) {
        DrawTexturePro(ui.bgGame, { 0, 0, (float)ui.bgGame.width, (float)ui.bgGame.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        Rectangle frameSrc = { 0, 0, (float)ui.boardFrame.width, (float)ui.boardFrame.height };
        DrawTexturePro(ui.boardFrame, frameSrc, ui.boardFrameRec, {0, 0}, 0.0f, WHITE);

        // draw board and X,O
        Vector2 mousePos = GetMousePosition();
        
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                float x = ui.cellStartX + j * ui.cellSize;
                float y = ui.cellStartY + i * ui.cellSize;
                Rectangle cellRect = { x, y, ui.cellSize, ui.cellSize };
                Color cellTint = WHITE; 

                // effect lighter
                if (game.inputType == 0 && CheckCollisionPointRec(mousePos, cellRect) && game.board[i][j].c == 0) {
                    cellTint = LIGHTGRAY; 
                } else if (game.inputType == 1 && i == game.cursorRow && j == game.cursorCol) {
                    cellTint = LIGHTGRAY; 
                }

                // background for board
                Rectangle sourceCell = { 0, 0, (float)ui.cell.width, (float)ui.cell.height };
                DrawTexturePro(ui.cell, sourceCell, cellRect, {0, 0}, 0.0f, cellTint);

                // draw x and O
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
        
        // grid 
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
            // draw wwin / lose
            DrawRectangle(ui.cellStartX, ui.cellStartY, BOARD_SIZE * ui.cellSize, BOARD_SIZE * ui.cellSize, Fade(WHITE, 0.7f));
            
            if (game.matchStatus == 1) DrawText(TextFormat("%s (X) THANG!", game.player1.name), 800, 500, 50, RED);
            if (game.matchStatus == 2) DrawText(TextFormat("%s (O) THANG!", game.player2.name), 800, 500, 50, BLUE);
            if (game.matchStatus == 3) DrawText("HOA NHAU!", 850, 500, 50, GRAY);
            Rectangle btnPlayAgain = { 720, 560, 220, 50 };
            Rectangle btnMenu = { 980, 560, 220, 50 };

            // Đổi màu vàng cho nút đang được chọn
            Color colorPlayAgain = (ui.endGameSelection == 0) ? YELLOW : LIGHTGRAY;
            Color colorMenu = (ui.endGameSelection == 1) ? YELLOW : LIGHTGRAY;

            // Vẽ nút "Chơi Ván Mới"
            DrawRectangleRec(btnPlayAgain, colorPlayAgain);
            DrawRectangleLinesEx(btnPlayAgain, 3.0f, BLACK);
            DrawText("Choi Van Moi", 745, 572, 25, BLACK);

            // Vẽ nút "Về Menu"
            DrawRectangleRec(btnMenu, colorMenu);
            DrawRectangleLinesEx(btnMenu, 3.0f, BLACK);
            DrawText("Ve Menu", 1035, 572, 25, BLACK);

            // Hướng dẫn phím
            DrawText("Dung [A]/[D] hoac Chuot de chon. [ENTER] de xac nhan.", 650, 640, 22, DARKGRAY);
        }
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