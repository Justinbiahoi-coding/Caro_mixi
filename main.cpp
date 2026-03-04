#include "raylib.h"
#include "DataStruct.h"
#include "LogicControl.h"
#include "GraphicView.h"

int main() {
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Do An Caro - KHTN");
    SetTargetFPS(60);

    GameState game;
    InitGame(game, 0); 
    game.inputType = 0; // Mặc định ban đầu là dùng Chuột

    // 0: Menu, 1: Game, 2: Settings, 3: Credits
    int currentScreen = 0; 
    
    int menuSelection = 0; 
    const int TOTAL_MENU_ITEMS = 6;
    
    // --- THÊM BIẾN QUẢN LÝ CHO MÀN HÌNH SETTINGS ---
    int settingSelection = 0; // 0: Dùng chuột, 1: Dùng phím, 2: Quay lại
    const int TOTAL_SETTING_ITEMS = 3;

    bool shouldExit = false; 

    // import hình ảnh
    Texture2D bgMenu      = LoadTexture("assets/background-new.png");
    Texture2D btnNewGame  = LoadTexture("assets/menu/NewGame.png");
    Texture2D btnLoadGame = LoadTexture("assets/menu/LoadGame.png");
    Texture2D btnSettings = LoadTexture("assets/menu/Settings.png");
    Texture2D btnHelp     = LoadTexture("assets/menu/Help.png");
    Texture2D btnCredits  = LoadTexture("assets/menu/Credits.png");
    Texture2D btnExit     = LoadTexture("assets/menu/Exit.png");

    while (!shouldExit && !WindowShouldClose()) {
        
        Vector2 mouse = GetMousePosition();

        // --- PHẦN 1: CẬP NHẬT LOGIC (Update) ---
        if (currentScreen == 0) {
            
            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
                menuSelection--;
                if (menuSelection < 0) menuSelection = TOTAL_MENU_ITEMS - 1;
            }
            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                menuSelection++;
                if (menuSelection >= TOTAL_MENU_ITEMS) menuSelection = 0;
            }

            float centerX = 1496.2;
            float startY = 420;
            float gap = btnNewGame.height + 42;

            Rectangle rects[TOTAL_MENU_ITEMS] = {
                { centerX - btnNewGame.width  / 2.0f, startY,          (float)btnNewGame.width,  (float)btnNewGame.height },
                { centerX - btnLoadGame.width / 2.0f, startY + gap,    (float)btnLoadGame.width, (float)btnLoadGame.height },
                { centerX - btnSettings.width / 2.0f, startY + gap*2,  (float)btnSettings.width, (float)btnSettings.height },
                { centerX - btnHelp.width     / 2.0f, startY + gap*3,  (float)btnHelp.width,     (float)btnHelp.height },
                { centerX - btnCredits.width  / 2.0f, startY + gap*4,  (float)btnCredits.width,  (float)btnCredits.height },
                { centerX - btnExit.width     / 2.0f, startY + gap*5,  (float)btnExit.width,     (float)btnExit.height }
            };

            for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
                if (CheckCollisionPointRec(mouse, rects[i])) {
                    menuSelection = i; 
                }
            }

            bool confirmSelection = false;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, rects[menuSelection])) {
                confirmSelection = true;
            }
            if (IsKeyPressed(KEY_ENTER)) confirmSelection = true;

            if (confirmSelection) {
                switch (menuSelection) {
                    case 0: // New Game
                        {
                            int savedInput = game.inputType;
                            InitGame(game, 0);
                            game.inputType = savedInput;
                            currentScreen = 1;
                        }
                        break;
                    case 1: // Load Game
                        if (LoadGame(game, "caro_save.bin")) currentScreen = 1;
                        break;
                    case 2: // Settings
                        currentScreen = 2;
                        settingSelection = game.inputType; // Đặt con trỏ vào đúng cài đặt hiện tại
                        break;
                    case 3: // Help 
                        break;
                    case 4: // Credits
                        currentScreen = 3;
                        break;
                    case 5: // Exit
                        shouldExit = true;
                        break;
                }
            }

            if (IsKeyPressed(KEY_T)) {
                if (LoadGame(game, "caro_save.bin")) currentScreen = 1;
            }
        }
        else if (currentScreen == 1) {
            HandleInput(game);

            if (IsKeyPressed(KEY_M)) currentScreen = 0; 
            if (IsKeyPressed(KEY_L)) SaveGame(game, "caro_save.bin");
        }
        else if (currentScreen == 2) {
            // --- TỐI ƯU LOGIC ĐIỀU KHIỂN SETTINGS ---
            
            // 1. Phím mũi tên / WASD
            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
                settingSelection--;
                if (settingSelection < 0) settingSelection = TOTAL_SETTING_ITEMS - 1;
            }
            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                settingSelection++;
                if (settingSelection >= TOTAL_SETTING_ITEMS) settingSelection = 0;
            }

            // 2. Định nghĩa các vùng chữ để bắt Chuột
            Rectangle setRects[TOTAL_SETTING_ITEMS] = {
                { 250, 250, 600, 40 }, // Vùng chữ Dùng Chuột
                { 250, 310, 600, 40 }, // Vùng chữ Dùng Bàn phím
                { 250, 370, 600, 40 }  // Vùng chữ Quay lại Menu
            };

            // 3. Đồng bộ Chuột và Phím
            for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
                if (CheckCollisionPointRec(mouse, setRects[i])) {
                    settingSelection = i;
                }
            }

            // 4. Bắt sự kiện Click chuột HOẶC Enter
            bool confirmSet = false;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, setRects[settingSelection])) {
                confirmSet = true;
            }
            if (IsKeyPressed(KEY_ENTER)) confirmSet = true;

            // Xử lý khi xác nhận
            if (confirmSet) {
                if (settingSelection == 0) game.inputType = 0; // Chọn chuột
                else if (settingSelection == 1) game.inputType = 1; // Chọn phím
                else if (settingSelection == 2) currentScreen = 0; // Quay lại
            }

            // Vẫn giữ lối tắt phím M hoặc ESC để thoát nhanh
            if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) currentScreen = 0;
        }
        else if (currentScreen == 3) {
            if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) currentScreen = 0;
        }

        // --- PHẦN 2: VẼ GIAO DIỆN (Draw) ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (currentScreen == 0) {
                DrawTexturePro(bgMenu, { 0, 0, (float)bgMenu.width, (float)bgMenu.height }, { 0, 0, (float)screenWidth, (float)screenHeight }, { 0, 0 }, 0.0f, WHITE);

                Texture2D* textures[TOTAL_MENU_ITEMS] = { &btnNewGame, &btnLoadGame, &btnSettings, &btnHelp, &btnCredits, &btnExit };
                
                float centerX = 1496.2;
                float startY = 420;
                float gap = btnNewGame.height + 42;
                Rectangle rectsToDraw[TOTAL_MENU_ITEMS] = {
                    { centerX - btnNewGame.width  / 2.0f, startY,          (float)btnNewGame.width,  (float)btnNewGame.height },
                    { centerX - btnLoadGame.width / 2.0f, startY + gap,    (float)btnLoadGame.width, (float)btnLoadGame.height },
                    { centerX - btnSettings.width / 2.0f, startY + gap*2,  (float)btnSettings.width, (float)btnSettings.height },
                    { centerX - btnHelp.width     / 2.0f, startY + gap*3,  (float)btnHelp.width,     (float)btnHelp.height },
                    { centerX - btnCredits.width  / 2.0f, startY + gap*4,  (float)btnCredits.width,  (float)btnCredits.height },
                    { centerX - btnExit.width     / 2.0f, startY + gap*5,  (float)btnExit.width,     (float)btnExit.height }
                };

                for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
                    Color tint = (menuSelection == i) ? YELLOW : WHITE;
                    DrawTexture(*textures[i], (int)rectsToDraw[i].x, (int)rectsToDraw[i].y, tint);
                }
            } 
            else if (currentScreen == 1) {
                DrawBoard(game);

                int infoX = 550; 
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
            else if (currentScreen == 2) {
                // --- TỐI ƯU VẼ GIAO DIỆN SETTINGS ---
                DrawText("CAI DAT DIEU KHIEN", 200, 150, 40, DARKBLUE);
                
                const char* setOptions[TOTAL_SETTING_ITEMS] = {
                    "Dung Chuot (Khuyen Nghi)",
                    "Dung Ban Phim (WASD + Enter)",
                    "Quay lai Menu"
                };

                for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
                    int yPos = 250 + i * 60; // Khoảng cách các mục là 60px
                    
                    // Thêm tick [X] cho mục điều khiển đang được kích hoạt
                    const char* prefix = "   ";
                    if (i == 0 && game.inputType == 0) prefix = "[X]";
                    else if (i == 0) prefix = "[ ]";
                    
                    if (i == 1 && game.inputType == 1) prefix = "[X]";
                    else if (i == 1) prefix = "[ ]";

                    // Vẽ hiệu ứng Hover (chuột hoặc phím trỏ tới thì đổi màu Đỏ)
                    Color textColor = DARKGRAY;
                    if (i == settingSelection) {
                        textColor = RED;
                        DrawText(TextFormat("-> %s %s", prefix, setOptions[i]), 210, yPos, 30, textColor);
                    } else {
                        DrawText(TextFormat("   %s %s", prefix, setOptions[i]), 210, yPos, 30, textColor);
                    }
                }
            }
            else if (currentScreen == 3) {
                DrawText("Credits: Thien", 100, 200, 40, DARKBLUE);
                DrawText("Nhan [M] de quay lai Menu", 250, 250, 20, GRAY);
            }

        EndDrawing();
    }

    UnloadTexture(bgMenu);
    UnloadTexture(btnNewGame);
    UnloadTexture(btnLoadGame);
    UnloadTexture(btnSettings);
    UnloadTexture(btnHelp);
    UnloadTexture(btnCredits);
    UnloadTexture(btnExit);
    CloseWindow();
    return 0;
}