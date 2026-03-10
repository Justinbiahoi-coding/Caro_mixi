#include "GUI.h"
#include "LogicControl.h"

<<<<<<< HEAD
// const int CELL_SIZE = 45; 
// const int OFFSET_X = (1920 - 15 * CELL_SIZE) / 2; 
// const int OFFSET_Y = (1080 - 15 * CELL_SIZE) / 2; 
const int TOTAL_MENU_ITEMS = 6;
const int TOTAL_SETTING_ITEMS = 3;
=======
// ============================================================
//  Hằng số nội bộ
// ============================================================
static const int TOTAL_MENU_ITEMS    = 6;
static const int TOTAL_SETTING_ITEMS = 3;

// ============================================================
//  HELPERS NỘI BỘ (chỉ dùng trong file này)
// ============================================================

// Trả về mảng Rectangle của 6 nút menu, dựa trên kích thước texture thực.
static void BuildMenuRects(const UIState& ui, Rectangle out[TOTAL_MENU_ITEMS]) {
    const float centerX = 1496.2f;
    const float startY  = 420.0f;
    const float gap     = ui.btnNewGame.height + 42.0f;

    const Texture2D* textures[TOTAL_MENU_ITEMS] = {
        &ui.btnNewGame, &ui.btnLoadGame, &ui.btnSettings,
        &ui.btnHelp,    &ui.btnCredits,  &ui.btnExit
    };
    for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
        out[i] = {
            centerX - textures[i]->width  / 2.0f,
            startY  + gap * i,
            (float)textures[i]->width,
            (float)textures[i]->height
        };
    }
}

// Vẽ bàn cờ (khung, ô, quân X/O, cursor bàn phím).
// Tách riêng để DrawGUI không bị quá dài.
static void DrawBoard(const GameState& game, const UIState& ui) {
    const BoardLayout& layout = ui.boardLayout;
    const Vector2 origin = { 0.0f, 0.0f };
    const bool useMouse  = (game.inputType == 0);

    // ----------------------------------------------------------
    // 1. Khung bàn cờ
    // ----------------------------------------------------------
    Rectangle frameSrc = {
        0.0f, 0.0f,
        (float)ui.boardFrame.width,
        (float)ui.boardFrame.height
    };
    DrawTexturePro(ui.boardFrame, frameSrc, layout.frameRect, origin, 0.0f, WHITE);

    // ----------------------------------------------------------
    // 2 & 3. Ô cờ + quân cờ
    // ----------------------------------------------------------
    const Vector2 mousePos = GetMousePosition();

    Rectangle cellSrc = {
        0.0f, 0.0f,
        (float)ui.cell.width,
        (float)ui.cell.height
    };

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            Rectangle cellDest = {
                layout.gridX + col * layout.cellSize,
                layout.gridY + row * layout.cellSize,
                layout.cellSize,
                layout.cellSize
            };

            // Highlight hover (chuột) hoặc cursor (bàn phím), chỉ khi ô trống
            const bool isEmpty   = (game.board[row][col].c == 0);
            const bool isHovered = useMouse
                ? CheckCollisionPointRec(mousePos, cellDest)
                : (row == game.cursorRow && col == game.cursorCol);

            Color cellTint = (isEmpty && isHovered) ? LIGHTGRAY : WHITE;
            DrawTexturePro(ui.cell, cellSrc, cellDest, origin, 0.0f, cellTint);

            // Quân X / O
            const int piece = game.board[row][col].c;
            if (piece == 1) {
                Rectangle xSrc = { 0, 0, (float)ui.pieceX.width, (float)ui.pieceX.height };
                DrawTexturePro(ui.pieceX, xSrc, cellDest, origin, 0.0f, WHITE);
            }
            else if (piece == 2) {
                Rectangle oSrc = { 0, 0, (float)ui.pieceO.width, (float)ui.pieceO.height };
                DrawTexturePro(ui.pieceO, oSrc, cellDest, origin, 0.0f, WHITE);
            }
        }
    }

    // ----------------------------------------------------------
    // 4. Viền + fill con trỏ bàn phím
    // ----------------------------------------------------------
    if (!useMouse) {
        Rectangle cursorDest = {
            layout.gridX + game.cursorCol * layout.cellSize,
            layout.gridY + game.cursorRow * layout.cellSize,
            layout.cellSize,
            layout.cellSize
        };
        DrawRectangle(
            (int)cursorDest.x + 2, (int)cursorDest.y + 2,
            (int)cursorDest.width - 4, (int)cursorDest.height - 4,
            Fade(GREEN, 0.4f)
        );
        DrawRectangleLinesEx(cursorDest, 3.0f, DARKGREEN);
    }
}

// Xử lý click chuột / WASD+Enter khi đang ở màn hình game.
static void HandleGameInput(GameState& game, const BoardLayout& layout) {
    if (game.matchStatus != 0) return; // Ván đã kết thúc, bỏ qua

    if (game.inputType == 0) {
        // --- Chuột ---
        if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return;

        const Vector2 mouse = GetMousePosition();
        const bool inGrid =
            mouse.x >= layout.gridX && mouse.x < layout.gridX + layout.gridWidth &&
            mouse.y >= layout.gridY && mouse.y < layout.gridY + layout.gridHeight;

        if (!inGrid) return;

        const int col = (int)((mouse.x - layout.gridX) / layout.cellSize);
        const int row = (int)((mouse.y - layout.gridY) / layout.cellSize);
        MakeMove(game, row, col);
    }
    else if (game.inputType == 1) {
        // --- Bàn phím WASD + Enter ---
        if (IsKeyPressed(KEY_W) && game.cursorRow > 0)              game.cursorRow--;
        if (IsKeyPressed(KEY_S) && game.cursorRow < BOARD_SIZE - 1) game.cursorRow++;
        if (IsKeyPressed(KEY_A) && game.cursorCol > 0)              game.cursorCol--;
        if (IsKeyPressed(KEY_D) && game.cursorCol < BOARD_SIZE - 1) game.cursorCol++;
        if (IsKeyPressed(KEY_ENTER)) MakeMove(game, game.cursorRow, game.cursorCol);
    }
}

// ============================================================
//  VÒNG ĐỜI
// ============================================================
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19

void InitGUI(UIState& ui) {
    // --- Trạng thái điều hướng ---
    ui.currentScreen  = 0;
    ui.menuSelection  = 0;
    ui.settingSelection = 0;
    ui.loadSelection  = 0;
    ui.saveSelection  = 0;
    ui.nameInput[0]   = '\0';
    ui.letterCount    = 0;
    ui.shouldExit     = false;

    ui.p1NameInput[0] = '\0';
    ui.p2NameInput[0] = '\0';
    ui.p1LetterCount  = 0;
    ui.p2LetterCount  = 0;
    ui.activeInputField = 0;
<<<<<<< HEAD
    // Load asset for menu
=======

    // --- Texture menu ---
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19
    ui.bgMenu      = LoadTexture("assets/background-new.png");
    ui.btnNewGame  = LoadTexture("assets/menu/NewGame.png");
    ui.btnLoadGame = LoadTexture("assets/menu/LoadGame.png");
    ui.btnSettings = LoadTexture("assets/menu/Settings.png");
    ui.btnHelp     = LoadTexture("assets/menu/Help.png");
    ui.btnCredits  = LoadTexture("assets/menu/Credits.png");
    ui.btnExit     = LoadTexture("assets/menu/Exit.png");
<<<<<<< HEAD
    //Load asset for board 
=======

    // --- Texture bàn cờ ---
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19
    ui.boardFrame = LoadTexture("assets/board/board_frame.png");
    ui.cell       = LoadTexture("assets/board/cell_custom.png");
    ui.pieceX     = LoadTexture("assets/board/piece_x.png");
    ui.pieceO     = LoadTexture("assets/board/piece_o.png");

<<<<<<< HEAD
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
=======
    // --- Layout bàn cờ ---
    // Chỉ cần chỉnh 4 dòng này để thay đổi toàn bộ layout.
    // gridWidth / gridHeight / frameRect sẽ tự tính.
    ui.boardLayout.cellSize     = 45.0f;   // Kích thước 1 ô (px)
    ui.boardLayout.gridX        = (1920.0f - BOARD_SIZE * 45.0f) / 2.0f; // Căn giữa ngang
    ui.boardLayout.gridY        = (1080.0f - BOARD_SIZE * 45.0f) / 2.0f; // Căn giữa dọc
    ui.boardLayout.framePadding = 20.0f;   // Khoảng cách từ lưới tới viền khung
    ComputeBoardLayout(ui.boardLayout);
}

void UnloadGUI(UIState& ui) {
    // Menu
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19
    UnloadTexture(ui.bgMenu);
    UnloadTexture(ui.btnNewGame);
    UnloadTexture(ui.btnLoadGame);
    UnloadTexture(ui.btnSettings);
    UnloadTexture(ui.btnHelp);
    UnloadTexture(ui.btnCredits);
    UnloadTexture(ui.btnExit);
<<<<<<< HEAD
    //unload board
=======

    // Bàn cờ
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19
    UnloadTexture(ui.boardFrame);
    UnloadTexture(ui.cell);
    UnloadTexture(ui.pieceX);
    UnloadTexture(ui.pieceO);
}

// ============================================================
//  UPDATE — xử lý input theo từng màn hình
// ============================================================

void UpdateGUI(GameState& game, UIState& ui) {
    const Vector2 mouse = GetMousePosition();

    // ----------------------------------------------------------
    // Màn hình 0: Menu chính
    // ----------------------------------------------------------
    if (ui.currentScreen == 0) {
        // Di chuyển bằng bàn phím
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            ui.menuSelection = (ui.menuSelection - 1 + TOTAL_MENU_ITEMS) % TOTAL_MENU_ITEMS;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            ui.menuSelection = (ui.menuSelection + 1) % TOTAL_MENU_ITEMS;
        }

        Rectangle rects[TOTAL_MENU_ITEMS];
        BuildMenuRects(ui, rects);

        // Hover chuột cập nhật selection
        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            if (CheckCollisionPointRec(mouse, rects[i])) ui.menuSelection = i;
        }

        // Xác nhận lựa chọn (click hoặc Enter)
        bool confirm =
            (IsKeyPressed(KEY_ENTER)) ||
            (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
             CheckCollisionPointRec(mouse, rects[ui.menuSelection]));

        if (confirm) {
            switch (ui.menuSelection) {
                case 0: // New Game → nhập tên
                    ui.currentScreen    = 7;
                    ui.activeInputField = 0;
                    ui.p1NameInput[0]   = '\0'; ui.p1LetterCount = 0;
                    ui.p2NameInput[0]   = '\0'; ui.p2LetterCount = 0;
                    break;
                case 1: // Load Game
                    ui.currentScreen = 5;
                    ui.loadSelection = 0;
                    break;
                case 2: // Settings
                    ui.currentScreen    = 2;
                    ui.settingSelection = game.inputType;
                    break;
                case 3: // Help (chưa làm)
                    break;
                case 4: // Credits
                    ui.currentScreen = 3;
                    break;
                case 5: // Exit
                    ui.shouldExit = true;
                    break;
            }
        }
    }
<<<<<<< HEAD
  else if (ui.currentScreen == 1) {
        if (game.inputType == 0) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Sửa lại logic bắt tọa độ dùng UI Layoutf
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
=======
    // ----------------------------------------------------------
    // Màn hình 1: Game
    // ----------------------------------------------------------
    else if (ui.currentScreen == 1) {
        HandleGameInput(game, ui.boardLayout);

        if (IsKeyPressed(KEY_M)) ui.currentScreen = 0;
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19

        if (IsKeyPressed(KEY_L)) {
            ui.currentScreen = 6;
            ui.nameInput[0]  = '\0';
            ui.letterCount   = 0;
            ui.saveSelection = 0;
        }
    }
    // ----------------------------------------------------------
    // Màn hình 2: Settings
    // ----------------------------------------------------------
    else if (ui.currentScreen == 2) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            ui.settingSelection = (ui.settingSelection - 1 + TOTAL_SETTING_ITEMS) % TOTAL_SETTING_ITEMS;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            ui.settingSelection = (ui.settingSelection + 1) % TOTAL_SETTING_ITEMS;
        }

        Rectangle setRects[TOTAL_SETTING_ITEMS] = {
            { 250, 250, 600, 40 },
            { 250, 310, 600, 40 },
            { 250, 370, 600, 40 }
        };

        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            if (CheckCollisionPointRec(mouse, setRects[i])) ui.settingSelection = i;
        }

        bool confirmSet =
            (IsKeyPressed(KEY_ENTER)) ||
            (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
             CheckCollisionPointRec(mouse, setRects[ui.settingSelection]));

        if (confirmSet) {
            if      (ui.settingSelection == 0) game.inputType   = 0;
            else if (ui.settingSelection == 1) game.inputType   = 1;
            else if (ui.settingSelection == 2) ui.currentScreen = 0;
        }

        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0;
    }
    // ----------------------------------------------------------
    // Màn hình 3: Credits
    // ----------------------------------------------------------
    else if (ui.currentScreen == 3) {
        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0;
    }
    // ----------------------------------------------------------
    // Màn hình 5: Load Game
    // ----------------------------------------------------------
    else if (ui.currentScreen == 5) {
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            ui.loadSelection = (ui.loadSelection - 1 + 4) % 4;
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            ui.loadSelection = (ui.loadSelection + 1) % 4;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (LoadGameSlot(game, ui.loadSelection)) ui.currentScreen = 1;
        }
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_DELETE)) {
            DeleteGameSlot(ui.loadSelection);
        }
        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0;
    }
    // ----------------------------------------------------------
    // Màn hình 6: Save Game
    // ----------------------------------------------------------
    else if (ui.currentScreen == 6) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (ui.letterCount < 29)) {
                ui.nameInput[ui.letterCount]     = (char)key;
                ui.nameInput[ui.letterCount + 1] = '\0';
                ui.letterCount++;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && ui.letterCount > 0) {
            ui.nameInput[--ui.letterCount] = '\0';
        }
        if (IsKeyPressed(KEY_UP)) {
            ui.saveSelection = (ui.saveSelection - 1 + 4) % 4;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            ui.saveSelection = (ui.saveSelection + 1) % 4;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            const char* saveName = (ui.letterCount > 0) ? ui.nameInput : "Chua Dat Ten";
            SaveGameSlot(game, ui.saveSelection, saveName);
            ui.currentScreen = 1;
        }
        if (IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 1;
    }
    // ----------------------------------------------------------
    // Màn hình 7: Nhập tên người chơi (trước New Game)
    // ----------------------------------------------------------
    else if (ui.currentScreen == 7) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125)) {
                if (ui.activeInputField == 0 && ui.p1LetterCount < 15) {
                    ui.p1NameInput[ui.p1LetterCount]     = (char)key;
                    ui.p1NameInput[ui.p1LetterCount + 1] = '\0';
                    ui.p1LetterCount++;
                }
                else if (ui.activeInputField == 1 && ui.p2LetterCount < 15) {
                    ui.p2NameInput[ui.p2LetterCount]     = (char)key;
                    ui.p2NameInput[ui.p2LetterCount + 1] = '\0';
                    ui.p2LetterCount++;
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (ui.activeInputField == 0 && ui.p1LetterCount > 0)
                ui.p1NameInput[--ui.p1LetterCount] = '\0';
            else if (ui.activeInputField == 1 && ui.p2LetterCount > 0)
                ui.p2NameInput[--ui.p2LetterCount] = '\0';
        }

        // Tab / Lên / Xuống: chuyển ô nhập
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_TAB)) {
            ui.activeInputField = 1 - ui.activeInputField;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (ui.activeInputField == 0) {
                ui.activeInputField = 1; // Chuyển sang P2 trước
            }
            else {
                // Bắt đầu ván mới, giữ lại inputType
                int savedInput = game.inputType;
                InitGame(game, 0);
                game.inputType = savedInput;

                strcpy(game.player1.name, ui.p1LetterCount > 0 ? ui.p1NameInput : "Player 1");
                strcpy(game.player2.name, ui.p2LetterCount > 0 ? ui.p2NameInput : "Player 2");

                ui.currentScreen = 1;
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0;
    }
}

// ============================================================
//  DRAW — vẽ từng màn hình
// ============================================================

void DrawGUI(const GameState& game, const UIState& ui) {
    // Cursor nhấp nháy (dùng chung cho các ô text input)
    const bool showCursor = ((int)(GetTime() * 2) % 2 == 0);

    // ----------------------------------------------------------
    // Màn hình 0: Menu chính
    // ----------------------------------------------------------
    if (ui.currentScreen == 0) {
        // Background
        DrawTexturePro(
            ui.bgMenu,
            { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height },
            { 0, 0, 1920.0f, 1080.0f },
            { 0, 0 }, 0.0f, WHITE
        );

        // Các nút menu
        const Texture2D* textures[TOTAL_MENU_ITEMS] = {
            &ui.btnNewGame, &ui.btnLoadGame, &ui.btnSettings,
            &ui.btnHelp,    &ui.btnCredits,  &ui.btnExit
        };
        Rectangle rects[TOTAL_MENU_ITEMS];
        BuildMenuRects(ui, rects);

        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            Color tint = (ui.menuSelection == i) ? YELLOW : WHITE;
            DrawTexture(*textures[i], (int)rects[i].x, (int)rects[i].y, tint);
        }
    }
    // ----------------------------------------------------------
    // Màn hình 1: Game
    // ----------------------------------------------------------
    else if (ui.currentScreen == 1) {
<<<<<<< HEAD
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
=======
        // Bàn cờ (khung + ô + quân + cursor)
        DrawBoard(game, ui);

        // --- Panel thông tin trên (căn giữa phía trên bàn cờ) ---
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19
        DrawText("THONG TIN VAN DAU", 820, 50, 30, BLACK);

        if (game.matchStatus == 0) {
            if (game.isPlayer1Turn)
                DrawText(TextFormat(">> LUOT CUA: %s (X) <<", game.player1.name), 790, 100, 30, RED);
            else
                DrawText(TextFormat(">> LUOT CUA: %s (O) <<", game.player2.name), 790, 100, 30, BLUE);
        }

<<<<<<< HEAD
        if (game.inputType == 0) DrawText("Dieu khien: Chuot", 880, 140, 20, GRAY);
        else DrawText("Dieu khien: WASD + Enter", 850, 140, 20, GRAY);
        
        DrawText("Bam [L] de luu game | Nhan [M] de ve Menu", 720, 980, 25, DARKGRAY);
=======
        if (game.inputType == 0) DrawText("Dieu khien: Chuot",           880, 140, 20, GRAY);
        else                     DrawText("Dieu khien: WASD + Enter",    850, 140, 20, GRAY);

        DrawText("Bam [L] de luu game | Nhan [M] de ve Menu", 720, 950, 25, DARKGRAY);
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19

        // --- Panel bên trái: Player 1 ---
        const int p1X = 200, p1Y = 350;
        DrawText(game.player1.name,                              p1X, p1Y,       50, RED);
        DrawText("Phe: X",                                       p1X, p1Y + 70,  30, DARKGRAY);
        DrawText(TextFormat("So buoc: %d", game.player1.stepCount), p1X, p1Y + 120, 30, BLACK);
        if (game.gameMode == 1) {
            DrawText(TextFormat("HP: %d / 3",    game.player1.hp),        p1X, p1Y + 170, 30, MAROON);
            DrawText(TextFormat("Scan: %d",      game.player1.scansLeft), p1X, p1Y + 220, 30, ORANGE);
        }

        // --- Panel bên phải: Player 2 ---
        const int p2X = 1500, p2Y = 350;
        DrawText(game.player2.name,                              p2X, p2Y,       50, BLUE);
        DrawText("Phe: O",                                       p2X, p2Y + 70,  30, DARKGRAY);
        DrawText(TextFormat("So buoc: %d", game.player2.stepCount), p2X, p2Y + 120, 30, BLACK);
        if (game.gameMode == 1) {
            DrawText(TextFormat("HP: %d / 3",    game.player2.hp),        p2X, p2Y + 170, 30, MAROON);
            DrawText(TextFormat("Scan: %d",      game.player2.scansLeft), p2X, p2Y + 220, 30, ORANGE);
        }

<<<<<<< HEAD
       if (game.matchStatus != 0) {
            // draw wwin / lose
            DrawRectangle(ui.cellStartX, ui.cellStartY, BOARD_SIZE * ui.cellSize, BOARD_SIZE * ui.cellSize, Fade(WHITE, 0.7f));
            
            if (game.matchStatus == 1) DrawText(TextFormat("%s (X) THANG!", game.player1.name), 800, 500, 50, RED);
            if (game.matchStatus == 2) DrawText(TextFormat("%s (O) THANG!", game.player2.name), 800, 500, 50, BLUE);
            if (game.matchStatus == 3) DrawText("HOA NHAU!", 850, 500, 50, GRAY);
=======
        // --- Overlay kết quả (đè lên bàn cờ khi ván kết thúc) ---
        if (game.matchStatus != 0) {
            const BoardLayout& L = ui.boardLayout;
            DrawRectangle(
                (int)L.gridX, (int)L.gridY,
                (int)L.gridWidth, (int)L.gridHeight,
                Fade(WHITE, 0.7f)
            );
            if (game.matchStatus == 1)
                DrawText(TextFormat("%s (X) THANG!", game.player1.name), 800, 500, 50, RED);
            else if (game.matchStatus == 2)
                DrawText(TextFormat("%s (O) THANG!", game.player2.name), 800, 500, 50, BLUE);
            else if (game.matchStatus == 3)
                DrawText("HOA NHAU!", 850, 500, 50, GRAY);
>>>>>>> 671801499e9c4640b2a77385b90e5bdb1fadfe19
        }
    }
    // ----------------------------------------------------------
    // Màn hình 2: Settings
    // ----------------------------------------------------------
    else if (ui.currentScreen == 2) {
        DrawText("CAI DAT DIEU KHIEN", 200, 150, 40, DARKBLUE);

        const char* options[TOTAL_SETTING_ITEMS] = {
            "Dung Chuot (Khuyen Nghi)",
            "Dung Ban Phim (WASD + Enter)",
            "Quay lai Menu"
        };

        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            int yPos = 250 + i * 60;

            // Checkbox: chỉ áp dụng cho 2 tuỳ chọn đầu
            const char* checkbox = "   ";
            if      (i == 0) checkbox = (game.inputType == 0) ? "[X]" : "[ ]";
            else if (i == 1) checkbox = (game.inputType == 1) ? "[X]" : "[ ]";

            Color      textColor = (i == ui.settingSelection) ? RED : DARKGRAY;
            const char* pointer  = (i == ui.settingSelection) ? "->" : "  ";
            DrawText(TextFormat("%s %s %s", pointer, checkbox, options[i]), 210, yPos, 30, textColor);
        }
    }
    // ----------------------------------------------------------
    // Màn hình 3: Credits
    // ----------------------------------------------------------
    else if (ui.currentScreen == 3) {
        DrawText("Credits: Thien",               100, 200, 40, DARKBLUE);
        DrawText("Nhan [M] de quay lai Menu",    250, 250, 20, GRAY);
    }
    // ----------------------------------------------------------
    // Màn hình 5: Load Game
    // ----------------------------------------------------------
    else if (ui.currentScreen == 5) {
        DrawText("CHON FILE DE TAI", 800, 100, 50, DARKBLUE);
        DrawText(
            "Dung [A]/[D] hoac [Trai]/[Phai] de chon. "
            "[ENTER] de Tai. [DEL] de Xoa. [M] ve Menu.",
            450, 180, 25, DARKGRAY
        );

        const int cardWidth = 360, cardHeight = 480, cardGap = 60;
        const int startX = (1920 - (4 * cardWidth + 3 * cardGap)) / 2;
        const int startY = 300;

        for (int i = 0; i < 4; i++) {
            int currentX = startX + i * (cardWidth + cardGap);
            Rectangle cardRec = { (float)currentX, (float)startY, (float)cardWidth, (float)cardHeight };

            if (i == ui.loadSelection) {
                DrawRectangleRec(cardRec, Fade(SKYBLUE, 0.4f));
                DrawRectangleLinesEx(cardRec, 6, BLUE);
            }
            else {
                DrawRectangleRec(cardRec, Fade(LIGHTGRAY, 0.3f));
                DrawRectangleLinesEx(cardRec, 2, GRAY);
            }

            GameState tempGame;
            bool hasData = PeekGameSlot(i, tempGame);

            if (hasData) {
                // Mini bàn cờ bên trong card
                const int miniCell   = 16;
                const int boardWidth = BOARD_SIZE * miniCell;
                int boardOffsetX = currentX + (cardWidth - boardWidth) / 2;
                int boardOffsetY = startY + 30;

                DrawRectangle(boardOffsetX, boardOffsetY, boardWidth, boardWidth, WHITE);
                for (int r = 0; r < BOARD_SIZE; r++) {
                    for (int c = 0; c < BOARD_SIZE; c++) {
                        int cx = boardOffsetX + c * miniCell;
                        int cy = boardOffsetY + r * miniCell;
                        DrawRectangleLines(cx, cy, miniCell, miniCell, Fade(LIGHTGRAY, 0.5f));
                        if (tempGame.board[r][c].c == 1) {
                            DrawLine(cx + 2, cy + 2, cx + miniCell - 2, cy + miniCell - 2, RED);
                            DrawLine(cx + miniCell - 2, cy + 2, cx + 2, cy + miniCell - 2, RED);
                        }
                        else if (tempGame.board[r][c].c == 2) {
                            DrawCircleLines(cx + miniCell / 2, cy + miniCell / 2, miniCell / 2 - 2, BLUE);
                        }
                    }
                }

                // Metadata save
                const char* modeText = (tempGame.gameMode == 0) ? "Che Do: Co Dien" : "Che Do: Booming Caro";
                int textY = startY + boardWidth + 50;
                DrawText(TextFormat("Slot %d", i + 1), currentX + 20, textY,        30, DARKBLUE);
                DrawText(tempGame.saveName,             currentX + 20, textY + 40,   25, BLACK);
                DrawText(modeText,                      currentX + 20, textY + 75,   20, DARKGRAY);
                DrawText(tempGame.saveTime,             currentX + 20, textY + 105,  20, GRAY);
            }
            else {
                DrawText(TextFormat("Slot %d", i + 1), currentX + 20, startY + 20, 30, GRAY);
                const char* emptyText = "--- TRONG ---";
                DrawText(
                    emptyText,
                    currentX + cardWidth / 2 - MeasureText(emptyText, 25) / 2,
                    startY + cardHeight / 2,
                    25, GRAY
                );
            }
        }
    }
    // ----------------------------------------------------------
    // Màn hình 6: Save Game
    // ----------------------------------------------------------
    else if (ui.currentScreen == 6) {
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.8f));
        DrawText("LUU TRO CHOI", 800, 300, 40, WHITE);

        DrawText("1. Nhap ten cho ban luu nay (Toi da 29 ky tu):", 800, 380, 20, LIGHTGRAY);
        DrawRectangle(800, 420, 500, 50, WHITE);
        DrawText(ui.nameInput, 810, 435, 20, BLACK);
        if (ui.letterCount < 29 && showCursor) {
            DrawText("_", 810 + MeasureText(ui.nameInput, 20), 435, 20, BLACK);
        }

        DrawText("2. Chon vi tri Slot de ghi de (Dung Len/Xuong):", 800, 520, 20, LIGHTGRAY);
        for (int i = 0; i < 4; i++) {
            int yPos = 560 + i * 50;
            GameState tempGame;
            bool      hasData  = PeekGameSlot(i, tempGame);
            Color     textColor = (i == ui.saveSelection) ? YELLOW : DARKGRAY;
            const char* cursor  = (i == ui.saveSelection) ? "=>" : "  ";

            if (hasData)
                DrawText(TextFormat("%s Slot %d: %s", cursor, i + 1, tempGame.saveName), 800, yPos, 25, textColor);
            else
                DrawText(TextFormat("%s Slot %d: -- TRONG --", cursor, i + 1),          800, yPos, 25, textColor);
        }
        DrawText("Nhan [ENTER] de luu. Nhan [ESC] de quay lai game.", 800, 800, 20, GRAY);
    }
    // ----------------------------------------------------------
    // Màn hình 7: Nhập tên người chơi
    // ----------------------------------------------------------
    else if (ui.currentScreen == 7) {
        DrawTexturePro(
            ui.bgMenu,
            { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height },
            { 0, 0, 1920.0f, 1080.0f },
            { 0, 0 }, 0.0f, WHITE
        );
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.85f));

        DrawText("THONG TIN NGUOI CHOI", 750, 300, 40, WHITE);

        // Player 1
        DrawText("Ten Player 1 (X) - Toi da 15 ky tu:", 750, 400, 25, LIGHTGRAY);
        Color p1BoxColor = (ui.activeInputField == 0) ? RAYWHITE : DARKGRAY;
        DrawRectangle(750, 440, 400, 50, p1BoxColor);
        DrawText(ui.p1NameInput, 760, 455, 25, BLACK);
        if (ui.activeInputField == 0 && showCursor) {
            DrawText("_", 760 + MeasureText(ui.p1NameInput, 25), 455, 25, BLACK);
        }

        // Player 2
        DrawText("Ten Player 2 (O) - Toi da 15 ky tu:", 750, 530, 25, LIGHTGRAY);
        Color p2BoxColor = (ui.activeInputField == 1) ? RAYWHITE : DARKGRAY;
        DrawRectangle(750, 570, 400, 50, p2BoxColor);
        DrawText(ui.p2NameInput, 760, 585, 25, BLACK);
        if (ui.activeInputField == 1 && showCursor) {
            DrawText("_", 760 + MeasureText(ui.p2NameInput, 25), 585, 25, BLACK);
        }

        DrawText("Dung phim [Len]/[Xuong] hoac [Tab] de chuyen o nhap.", 650, 700, 25, GRAY);
        DrawText("Nhan [ENTER] de bat dau tran chien!",                   750, 750, 25, YELLOW);
        DrawText("Nhan [ESC] de quay lai Menu.",                          850, 800, 20, DARKGRAY);
    }
}