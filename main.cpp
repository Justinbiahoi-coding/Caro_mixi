#include "raylib.h"
#include "DataStruct.h"
int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Khởi tạo cửa sổ
    InitWindow(screenWidth, screenHeight, "Do An Caro");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        
        //PHẦN 1: CẬP NHẬT LOGIC (Update)
        //PHẦN 2: VẼ GIAO DIỆN (Draw)
        BeginDrawing();

            // Xóa khung hình cũ và đổ màu nền mới (RAYWHITE là màu trắng hơi ngà của Raylib)
            ClearBackground(RAYWHITE);

            //test
            DrawText("Chao mung den voi Booming Caro!", 220, 280, 20, DARKGRAY);
        EndDrawing();
    }

    // 3. Dọn dẹp tài nguyên và đóng cửa sổ khi thoát game (Giống GabageCollect)
    CloseWindow();

    return 0;
}