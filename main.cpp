#include "raylib.h"
#include "DataStruct.h"
#include "LogicControl.h"
#include "GraphicView.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Do An Caro - KHTN");
    SetTargetFPS(60);

    GameState game;
    InitGame(game, 0); 
    game.inputType = 0; // Mặc định ban đầu là dùng Chuột

    // 0: Menu, 1: Game, 2: Settings
    int currentScreen = 0; 

    while (!WindowShouldClose()) {
        
        // --- PHẦN 1: CẬP NHẬT LOGIC (Update) ---
        if (currentScreen == 0) {
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
        } 
        else if (currentScreen == 1) {
            // Đang chơi game
            HandleInput(game);

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

        // --- PHẦN 2: VẼ GIAO DIỆN (Draw) ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (currentScreen == 0) {
                // --- Vẽ giao diện Menu ---
                DrawText("GAME CO CARO", 200, 150, 50, DARKBLUE);
                DrawText("Nhan [ENTER] de Choi Moi", 250, 250, 20, DARKGRAY);
                DrawText("Nhan [S] de vao Cai Dat (Settings)", 250, 300, 20, DARKGRAY);
                DrawText("Nhan [T] de Tai Game ", 250, 350, 20, LIGHTGRAY);
            } 
            else if (currentScreen == 1) {
                // --- Vẽ giao diện Bàn cờ ---
                DrawBoard(game);

                int infoX = 550; 
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

        EndDrawing();
    }

    CloseWindow();
    return 0;
}