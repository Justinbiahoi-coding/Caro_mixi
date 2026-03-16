#include "Library.h"
#include "LogicControl.h"
#include "GUI.h"

int main() {
    InitWindow(1920, 1080, "Do An Caro - Booming Mode");
    SetTargetFPS(60);

    SetExitKey(0);
    GameState game;
    InitGame(game, 0); 
    game.inputType = 0; 

    UIState ui;
    InitGUI(ui); // put pic to ram

    // Game loop
    while (!ui.shouldExit && !WindowShouldClose()) {

        UpdateGUI(game, ui);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawGUI(game, ui);
        EndDrawing();
    }

    UnloadGUI(ui);
    CloseWindow();
    
    return 0;
}