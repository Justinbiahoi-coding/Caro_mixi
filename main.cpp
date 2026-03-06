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

    // 0: Menu, 1: Game, 2: Settings
    int currentScreen = 0; 

    BoardLayout layout;
    
    // 1. Cấu hình Ô cờ trước
    layout.cellSize = 50.0f;
    layout.cellStartX = 125.0f; 
    layout.cellStartY = 150.0f;

    // float padding = 30.0f;
    // layout.frame.x = layout.cellStartX - padding;
    // layout.frame.y = layout.cellStartY - padding;
    layout.frame.width = (BOARD_SIZE * layout.cellSize*1.3);
    layout.frame.height = (BOARD_SIZE * layout.cellSize*1.3);

    // import
    Texture2D bgMenu      = LoadTexture("assets/background-new.png");
    Texture2D btnNewGame  = LoadTexture("assets/menu/NewGame.png");
    Texture2D btnLoadGame = LoadTexture("assets/menu/LoadGame.png");
    Texture2D btnSettings = LoadTexture("assets/menu/Settings.png");
    Texture2D btnHelp     = LoadTexture("assets/menu/Help.png");
    Texture2D btnCredits  = LoadTexture("assets/menu/Credits.png");
    Texture2D btnExit     = LoadTexture("assets/menu/Exit.png");
    GameAssets assets;
    assets.boardFrame = LoadTexture("assets/board/board_frame.png"); // Ảnh khung gỗ/kim loại bọc ngoài
    assets.cell       = LoadTexture("assets/board/cell_custom.png");
    assets.pieceX     = LoadTexture("assets/board/piece_x.png");
    assets.pieceO     = LoadTexture("assets/board/piece_o.png");

    while (!WindowShouldClose()) {
        
        // --- PHẦN 1: CẬP NHẬT LOGIC (Update) ---
        if (currentScreen == 0) {
            // Vẽ background
            DrawTexturePro(
                bgMenu,
                { 0, 0, (float)bgMenu.width, (float)bgMenu.height },
                { 0, 0, (float)screenWidth, (float)screenHeight },
                { 0, 0 }, 0.0f, WHITE
            );

            Vector2 mouse = GetMousePosition();

            // --- Định nghĩa vị trí và kích thước các nút ---
            float centerX = 1496.2;
            float startY = 420;
            float gap = btnNewGame.height + 42;

            Rectangle rectNewGame  = { centerX - btnNewGame.width  / 2.0f, startY,        (float)btnNewGame.width,  (float)btnNewGame.height };
            Rectangle rectLoadGame = { centerX - btnLoadGame.width / 2.0f, startY + gap,   (float)btnLoadGame.width, (float)btnLoadGame.height };
            Rectangle rectSettings = { centerX - btnSettings.width/ 2.0f, startY + gap*2, (float)btnSettings.width, (float)btnSettings.height };
            Rectangle rectHelp     = { centerX - btnHelp.width    / 2.0f, startY + gap*3, (float)btnHelp.width,     (float)btnHelp.height };
            Rectangle rectCredits  = { centerX - btnCredits.width / 2.0f, startY + gap*4, (float)btnCredits.width,  (float)btnCredits.height };
            Rectangle rectExit     = { centerX - btnExit.width    / 2.0f, startY + gap*5, (float)btnExit.width,     (float)btnExit.height };

            // --- Hàm vẽ nút với hover effect (làm sáng khi hover) ---
            auto DrawMenuBtn = [&](Texture2D& tex, Rectangle rect) {
                Color tint = CheckCollisionPointRec(mouse, rect) ? YELLOW : WHITE;
                DrawTexture(tex, (int)rect.x, (int)rect.y, tint);
            };

            DrawMenuBtn(btnNewGame,  rectNewGame);
            DrawMenuBtn(btnLoadGame, rectLoadGame);
            DrawMenuBtn(btnSettings, rectSettings);
            DrawMenuBtn(btnHelp,     rectHelp);
            DrawMenuBtn(btnCredits,  rectCredits);
            DrawMenuBtn(btnExit,     rectExit);

            // Click chuột vào nút
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, rectNewGame)) {
                    int savedInput = game.inputType;
                    InitGame(game, 0);
                    game.inputType = savedInput;
                    currentScreen = 1;
                }
                else if (CheckCollisionPointRec(mouse, rectSettings)) {
                    currentScreen = 2;
                }
                else if (CheckCollisionPointRec(mouse, rectLoadGame)) {
                    if (LoadGame(game, "caro_save.bin")) {
                        currentScreen = 1;
                    }
                }
                else if (CheckCollisionPointRec(mouse, rectCredits)) {
                    currentScreen = 3;
                }
                else if (CheckCollisionPointRec(mouse, rectExit)) {
                    break; // Thoát vòng lặp
                }
            }

            // Đang ở Menu: Lắng nghe phím
            if (IsKeyPressed(KEY_ENTER)) {
                // Lưu lại tùy chọn điều khiển trước khi reset ván mới
                int savedInput = game.inputType; 
                
                InitGame(game, 0); 
                game.inputType = savedInput; // Phục hồi lại cài đặt điều khiển
                
                currentScreen = 1; // Vào game
            }
            if (IsKeyPressed(KEY_S)) {
                currentScreen = 2; // Chuyển sang màn hình Settings
            }
            if (IsKeyPressed(KEY_T)) {
                if (LoadGame(game, "caro_save.bin")) {
                    currentScreen = 1; // Load thành công thì nhảy thẳng vào game
                }
            }
            // credit
            if(IsKeyPressed(KEY_I)){
                currentScreen = 3;
            }
        }
        else if (currentScreen == 1) {
            // Đang chơi game
            HandleInput(game, layout);

            if (IsKeyPressed(KEY_M)) {
                currentScreen = 0; // Về Menu
            }
            if (IsKeyPressed(KEY_L)) {
                SaveGame(game, "caro_save.bin");
            }
        }
        else if (currentScreen == 2) {
            // Đang ở màn hình Settings
            // Nhấn phím 1 để chọn Chuột, phím 2 để chọn Bàn phím
            if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) game.inputType = 0;
            if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) game.inputType = 1;
            
            // Nhấn M (hoặc ESC) để quay về Menu chính
            if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) {
                currentScreen = 0;
            }
        }
        else if (currentScreen == 3) {

            // Nhấn ESC để quay về Menu chính
            if (IsKeyPressed(KEY_M)) {
                currentScreen = 0;
            }
        }

        // --- PHẦN 2: VẼ GIAO DIỆN (Draw) ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (currentScreen == 0) {
                // --- Vẽ giao diện Menu ---

                float btnW = 300, btnH = 70;
                float btnX = (screenWidth - btnW) / 2.0f;
                Rectangle btnPlay     = { btnX, 300, btnW, btnH };
                Rectangle btnLoad     = { btnX, 390, btnW, btnH };
                Rectangle btnSettings = { btnX, 480, btnW, btnH };
                Rectangle btnInfo     = { btnX, 570, btnW, btnH };
                Rectangle btnCredits  = { btnX, 660, btnW, btnH };
                Rectangle btnExit     = { btnX, 750, btnW, btnH };

            } 
            else if (currentScreen == 1) {
                // --- Vẽ giao diện Bàn cờ ---
                DrawBoard(game, assets, layout);

                int infoX = 1500; 
                DrawText("THONG TIN VAN DAU", infoX, 50, 20, BLACK);
                
                if (game.isPlayer1Turn) DrawText("Luot cua: X (P1)", infoX, 100, 20, RED);
                else DrawText("Luot cua: O (P2)", infoX, 100, 20, BLUE);

                DrawText(TextFormat("So buoc P1: %d", game.player1.stepCount), infoX, 150, 20, DARKGRAY);
                DrawText(TextFormat("So buoc P2: %d", game.player2.stepCount), infoX, 180, 20, DARKGRAY);
                
                // Hiển thị kiểu điều khiển đang dùng để người chơi dễ biết
                if (game.inputType == 0) DrawText("Dieu khien: Chuot", infoX, 230, 20, GRAY);
                else DrawText("Dieu khien: WASD + Enter", infoX, 230, 20, GRAY);
                DrawText(TextFormat("Bam [L] de luu game "), infoX, 250, 20, DARKGRAY);
                DrawText("Nhan [M] de ve Menu", infoX, 550, 20, GRAY);

                if (game.matchStatus == 1) DrawText("P1 (X) THANG!", infoX, 300, 30, RED);
                if (game.matchStatus == 2) DrawText("P2 (O) THANG!", infoX, 300, 30, BLUE);
                if (game.matchStatus == 3) DrawText("HOA NHAU!", infoX, 300, 30, GRAY);
            }
            else if (currentScreen == 2) {
                // --- Vẽ giao diện Settings ---
                DrawText("CAI DAT DIEU KHIEN", 200, 150, 40, DARKBLUE);
                
                // Đổi màu dòng chữ để làm nổi bật tùy chọn đang được tick
                if (game.inputType == 0) {
                    DrawText("-> 1. Dung Chuot (Dang chon)", 250, 250, 20, RED);
                    DrawText("   2. Dung Ban Phim (WASD + Enter)", 250, 300, 20, DARKGRAY);
                } else {
                    DrawText("   1. Dung Chuot", 250, 250, 20, DARKGRAY);
                    DrawText("-> 2. Dung Ban Phim (WASD + Enter) (Dang chon)", 250, 300, 20, RED);
                }

                DrawText("Nhan phim [1] hoac [2] de thay doi.", 250, 400, 20, GRAY);
                DrawText("Nhan [M] de luu va quay lai Menu", 250, 450, 20, GRAY);
            }
            else if (currentScreen == 3) {
                // --- Vẽ giao diện Settings ---
                DrawText("Credit hihi", 100, 200, 40, DARKBLUE);

                DrawText("Nhan [M] de  quay lai Menu", 250, 250, 20, GRAY);
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
    UnloadTexture(assets.boardFrame);
    UnloadTexture(assets.cell);
    UnloadTexture(assets.pieceX);
    UnloadTexture(assets.pieceO);
    CloseWindow();
    return 0;
}