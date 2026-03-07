#include "GUI.h"
#include "LogicControl.h"


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

    // --- Texture menu ---

    ui.bgMenu      = LoadTexture("assets/background-new.png");
    ui.btnNewGame  = LoadTexture("assets/menu/NewGame.png");
    ui.btnLoadGame = LoadTexture("assets/menu/LoadGame.png");
    ui.btnSettings = LoadTexture("assets/menu/Settings.png");
    ui.btnHelp     = LoadTexture("assets/menu/Help.png");
    ui.btnCredits  = LoadTexture("assets/menu/Credits.png");
    ui.btnExit     = LoadTexture("assets/menu/Exit.png");

    // --- Texture bàn cờ ---

    ui.boardFrame = LoadTexture("assets/board/board_frame.png");
    ui.cell       = LoadTexture("assets/board/cell_custom.png");
    ui.pieceX     = LoadTexture("assets/board/piece_x.png");
    ui.pieceO     = LoadTexture("assets/board/piece_o.png");

    // --- Texture Settings
    ui.bgSettings = LoadTexture("assets/menu/bgSettings.png");
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

    UnloadTexture(ui.bgMenu);
    UnloadTexture(ui.btnNewGame);
    UnloadTexture(ui.btnLoadGame);
    UnloadTexture(ui.btnSettings);
    UnloadTexture(ui.btnHelp);
    UnloadTexture(ui.btnCredits);
    UnloadTexture(ui.btnExit);

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

    // ----------------------------------------------------------
    // Màn hình 1: Game
    // ----------------------------------------------------------
    else if (ui.currentScreen == 1) {
        HandleGameInput(game, ui.boardLayout);

        if (IsKeyPressed(KEY_M)) ui.currentScreen = 0;

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
        // Lam moi phan settings
        Rectangle mouseBtn = { 850, 450, 200, 50 };
        Rectangle keyboardBtn = { 850, 520, 200, 50 };
        Rectangle backBtn = { 850, 590, 200, 50 };

        if (CheckCollisionPointRec(mouse, mouseBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            game.inputType = 0;
        }

        if (CheckCollisionPointRec(mouse, keyboardBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            game.inputType = 1;
        }

        if (CheckCollisionPointRec(mouse, backBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            ui.currentScreen = 0;
        }

        if (CheckCollisionPointRec(mouse, mouseBtn))
            ui.settingSelection = 0;

        if (CheckCollisionPointRec(mouse, keyboardBtn))
            ui.settingSelection = 1;

        if (CheckCollisionPointRec(mouse, backBtn))
            ui.settingSelection = 2;


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
        // Bàn cờ (khung + ô + quân + cursor)
        DrawBoard(game, ui);

        // --- Panel thông tin trên (căn giữa phía trên bàn cờ) ---
        DrawText("THONG TIN VAN DAU", 820, 50, 30, BLACK);

        if (game.matchStatus == 0) {
            if (game.isPlayer1Turn)
                DrawText(TextFormat(">> LUOT CUA: %s (X) <<", game.player1.name), 790, 100, 30, RED);
            else
                DrawText(TextFormat(">> LUOT CUA: %s (O) <<", game.player2.name), 790, 100, 30, BLUE);
        }

        if (game.inputType == 0) DrawText("Dieu khien: Chuot",           880, 140, 20, GRAY);
        else                     DrawText("Dieu khien: WASD + Enter",    850, 140, 20, GRAY);

        DrawText("Bam [L] de luu game | Nhan [M] de ve Menu", 720, 950, 25, DARKGRAY);


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

        }
    }
    // ----------------------------------------------------------
    // Màn hình 2: Settings
    // ----------------------------------------------------------
    else if (ui.currentScreen == 2) {

        // Background
       static float cloudOffset = 0;

        cloudOffset += 20 * GetFrameTime();

        DrawTexture(ui.bgSettings, -cloudOffset, 0, WHITE);
        DrawTexture(ui.bgSettings, ui.bgSettings.width - cloudOffset, 0, WHITE);

        if (cloudOffset > ui.bgSettings.width)
            cloudOffset = 0;

      
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.7f));

        DrawText("SETTINGS", 820, 150, 50, WHITE);

        const char* options[3] = {
            "    Dieu khien bang Chuot",
            "  Dieu khien bang Ban Phim",
            "         Quay lai Menu"
        };

        Color mouseColor = (game.inputType == 0) ? SKYBLUE : GRAY;
        Color keyboardColor = (game.inputType == 1) ? SKYBLUE : GRAY;

        for (int i = 0; i < 3; i++)
        {
            int y = 350 + i * 100;

            Color boxColor = DARKGRAY;

            if (i == game.inputType)
                boxColor = GREEN;

            DrawRectangle(700, y, 500, 60, Fade(boxColor, 0.6f));

            // viền nút
            DrawRectangleLines(700, y, 500, 60, WHITE);

            // nếu đang hover --> viền sáng
            if (i == ui.settingSelection)
                DrawRectangleLinesEx({ 700, (float)y, 500, 60 }, 3, YELLOW);

            DrawText(options[i], 760, y + 18, 28, WHITE);
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