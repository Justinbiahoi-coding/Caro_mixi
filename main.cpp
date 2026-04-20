#include "Library.h"
#include "LogicControl.h"
#include "GUI.h"

int main() {
    // 1. Cho phép cửa sổ có thể kéo giãn (Resize) thoải mái
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    
    // 2. Mở cửa sổ nhỏ hơn một chút cho vừa Mac (Ví dụ 1600x900)
    InitWindow(1600, 900, "Do An Caro - Booming Mode");
    SetTargetFPS(60);

    SetExitKey(0);
    GameState game;
    InitGame(game, 0); 
    game.inputType = 0; 

    UIState ui;
    InitGUI(ui); // put pic to ram

    // 3. TẠO TẤM BẠT ẢO ĐÚNG CHUẨN 1920x1080
    RenderTexture2D target = LoadRenderTexture(1920, 1080);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // Chống vỡ hạt khi thu nhỏ

    // Game loop
    while (!ui.shouldExit && !WindowShouldClose()) {

        // --- BỘ TÍNH TOÁN THU PHÓNG (SCALE) ---
        float scaleW = (float)GetScreenWidth() / 1920.0f;
        float scaleH = (float)GetScreenHeight() / 1080.0f;
        float scale = (scaleW < scaleH) ? scaleW : scaleH; // Giữ nguyên tỷ lệ 16:9

        // Báo cho Raylib biết màn hình bị thu nhỏ để nó tính lại tọa độ Click chuột
        SetMouseOffset(-(GetScreenWidth() - (1920.0f * scale)) * 0.5f, -(GetScreenHeight() - (1080.0f * scale)) * 0.5f);
        SetMouseScale(1.0f / scale, 1.0f / scale);

        UpdateGUI(game, ui);
        
        // --- BƯỚC A: VẼ MỌI THỨ LÊN TẤM BẠT ẢO ---
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            DrawGUI(game, ui);
        EndTextureMode();

        // --- BƯỚC B: IN TẤM BẠT ẢO LÊN MÀN HÌNH MÁY TÍNH ---
        BeginDrawing();
            ClearBackground(BLACK); // Viền đen bên ngoài nếu màn hình không phải 16:9

            // Vẽ bạt ảo ra giữa màn hình (Lưu ý height của sourceRec phải là số âm do chuẩn OpenGL)
            Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
            Rectangle destRec = { 
                (GetScreenWidth() - (1920.0f * scale)) * 0.5f, 
                (GetScreenHeight() - (1080.0f * scale)) * 0.5f, 
                1920.0f * scale, 
                1080.0f * scale 
            };
            DrawTexturePro(target.texture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);

        EndDrawing();
    }

    UnloadRenderTexture(target);
    UnloadGUI(ui);
    CloseWindow();
    
    return 0;
}