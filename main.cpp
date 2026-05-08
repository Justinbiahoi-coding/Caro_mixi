#include "Library.h"

int main() {
    //make sure the window is resizable and has VSync enabled for smoother animation
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    
    InitWindow(1600, 900, "Do An Caro");
    SetTargetFPS(60);

    SetExitKey(0);
    GameState game;
    InitGame(game, 0); 
    game.inputType = 0; 

    UIState ui;
    InitGUI(ui); // put pic to ram

    //1920x1080
    RenderTexture2D target = LoadRenderTexture(1920, 1080);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); 

    // Game loop
    while (!ui.shouldExit && !WindowShouldClose()) {

        // scale factor
        float scaleW = (float)GetScreenWidth() / 1920.0f;
        float scaleH = (float)GetScreenHeight() / 1080.0f;
        float scale = (scaleW < scaleH) ? scaleW : scaleH; //16:9

        // adjust mouse input to account for letterboxing
        SetMouseOffset(-(GetScreenWidth() - (1920.0f * scale)) * 0.5f, -(GetScreenHeight() - (1080.0f * scale)) * 0.5f);
        SetMouseScale(1.0f / scale, 1.0f / scale);

        UpdateGUI(game, ui);
        
        // draw to the render texture 
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            DrawGUI(game, ui);
        EndTextureMode();

        // scaling to fit while maintaining aspect ratio
        BeginDrawing();
            ClearBackground(BLACK); // draw a black background to create letterboxing effect when aspect ratio doesn't match

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