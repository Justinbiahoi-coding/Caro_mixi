#include "GUI_Menu.h"
#include "LogicControl.h"

const int TOTAL_MENU_ITEMS = 6;
const int TOTAL_SETTING_ITEMS = 5;

void UpdateMenuScreens(GameState& game, UIState& ui) {
    Vector2 mouse = GetMousePosition();

    // Update embers + smooth scroll whenever we are in the menu
    if (ui.currentScreen == 0) {
        float dt = GetFrameTime();
        float targetY = 350.0f + ui.menuSelection * (80.0f + 35.0f);
        ui.menuScrollY += (targetY - ui.menuScrollY) * 18.0f * dt;
        for (int i = 0; i < UIState::MAX_EMBERS; i++) {
            UIState::Ember& e = ui.embers[i];
            e.x    += e.vx * 60.0f * dt;
            e.y    += e.vy * 60.0f * dt;
            e.life += dt * 0.18f;
            e.alpha = (1.0f - e.life) * 0.85f;
            // Respawn when the ember's life ends
            if (e.life >= 1.0f) {
                e.x    = (float)(rand() % 1920);
                e.y    = 1080.0f + (float)(rand() % 60);
                e.vx   = ((rand() % 100) - 50) * 0.006f;
                e.vy   = -((rand() % 40) + 25) * 0.014f;
                e.size = (float)(rand() % 3 + 2);
                e.life = 0.0f;
            }
        }
    }

    if (ui.currentScreen == 0) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            ui.menuSelection--;
            if (ui.menuSelection < 0) ui.menuSelection = TOTAL_MENU_ITEMS - 1;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            ui.menuSelection++;
            if (ui.menuSelection >= TOTAL_MENU_ITEMS) ui.menuSelection = 0;
        }

        float btnW = 800.0f; // very wide clickable area
        float btnH = 80.0f;
        float centerX = 1500.0f; 
        float startY = 350.0f;
        float gap = btnH + 35.0f;

        Rectangle rects[TOTAL_MENU_ITEMS];
        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            rects[i] = { centerX - btnW/2.0f, startY + i*gap, btnW, btnH };
        }

        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            if (CheckCollisionPointRec(mouse, rects[i])) ui.menuSelection = i; 
        }

        bool confirmSelection = false;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, rects[ui.menuSelection])) confirmSelection = true;
        if (IsKeyPressed(KEY_ENTER)) confirmSelection = true;

        if (confirmSelection) {
            const char* helpText[] = {
                "CARO BATTLE - LUAT CHOI",
                "- Hai ben luan phien danh X va O.",
                "- Ben nao co 5 quan lien tiep se thang.",
                "- Chuc ban choi vui ve!"
            };        
            switch (ui.menuSelection) {
                case 0: { 
                    ui.currentScreen = 8; 
                    ui.p1HeroSelection = 0;
                    ui.p2HeroSelection = 0; 
                    ui.selectionPhase = -1; 
                    
                    ui.p1NameInput[0] = '\0'; ui.p1LetterCount = 0;
                    ui.p2NameInput[0] = '\0'; ui.p2LetterCount = 0;
                } break;
                case 1: ui.currentScreen = 5; ui.loadSelection = 0; break;
                case 2: ui.currentScreen = 2; ui.settingSelection = game.inputType; ui.draggingVolume = false; break;
                case 3: ui.currentScreen = 3; ui.helpScrollTimer = 0.0f; break; // HELP
                case 4: break; // CREDITS (chua lam)
                case 5: ui.shouldExit = true; break;
            }
        }
    }
   
    else if (ui.currentScreen == 2) {
        Vector2 mouse = GetMousePosition();

        const int barWidth = 400;
        const int barHeight = 24;
        const int barX = (1920 - barWidth) / 2;
        const int barY = 350 + 2 * 80 + 60;

        // Enlarge the click area so the slider is easy to grab
        Rectangle volumeHitbox = { (float)barX - 20, (float)barY - 20, (float)barWidth + 40, (float)barHeight + 40 };

        if (CheckCollisionPointRec(mouse, volumeHitbox)) {
            ui.settingSelection = -1; // hide highlight when hovering the volume bar
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, volumeHitbox)) {
                ui.draggingVolume = true;
            }
        }

        if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            ui.draggingVolume = false;
        }

        if (ui.draggingVolume) {
            float t = (mouse.x - barX) / (float)barWidth;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            ui.musicVolume = t;
            SetMusicVolume(ui.bgMusic, ui.musicVolume);
            
            ui.settingSelection = -1; // keep highlight hidden while dragging volume
        }

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            if (ui.settingSelection == -1) ui.settingSelection = 3; // if pointing at volume, jump to the item below
            else {
                ui.settingSelection = (ui.settingSelection - 1 + TOTAL_SETTING_ITEMS) % TOTAL_SETTING_ITEMS;
                if (ui.settingSelection == 2) ui.settingSelection = 1;
            }
        }

        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            if (ui.settingSelection == -1) ui.settingSelection = 3;
            else {
                ui.settingSelection = (ui.settingSelection + 1) % TOTAL_SETTING_ITEMS;
                if (ui.settingSelection == 2) ui.settingSelection = 3;
            }
        }

    const char* menuOptions[TOTAL_MENU_ITEMS] = {
        "NEW GAME",
        "LOAD GAME",
        "SETTINGS",
        "HELP",
        "CREDITS",
        "EXIT"
    };
        const char* setOptions[TOTAL_SETTING_ITEMS] = {
            "Mouse Control (Recommended)",
            "Keyboard Control (WASD + Enter)",
            "Music Volume",
            "Toggle Music",
            "Back to Menu"
        };

        Rectangle setRects[TOTAL_SETTING_ITEMS];
        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            if (i == 2) continue; // the "Music Volume" row is not selectable here
            int yPos = 350 + i * 80;
            if (i >= 3) yPos += 40;
            int textWidth = MeasureTextCustomX(ui.mainFont, setOptions[i], 40);
            int xPos = (1920 - textWidth) / 2;
            setRects[i] = { (float)(xPos - 40), (float)(yPos - 10), (float)(textWidth + 80), 60 };

            if (!ui.draggingVolume && CheckCollisionPointRec(mouse, setRects[i]))
                ui.settingSelection = i;
        }

        if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, setRects[ui.settingSelection])
            && !ui.draggingVolume) || IsKeyPressed(KEY_ENTER)) {
            if (ui.settingSelection == 0) game.inputType = 0;
            else if (ui.settingSelection == 1) game.inputType = 1;
            else if (ui.settingSelection == 3) {
                ui.musicEnabled = !ui.musicEnabled;
                if (ui.musicEnabled) PlayMusicStream(ui.bgMusic);
                else PauseMusicStream(ui.bgMusic);
            }
            else if (ui.settingSelection == 4) {
                ui.currentScreen = 0;
            }
        }

        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE))
            ui.currentScreen = 0;
    }
    else if (ui.currentScreen == 3) {
        ui.helpScrollTimer += GetFrameTime();

        Vector2 mouse3 = GetMousePosition();
        Rectangle closeBtn3 = { 1920.0f - 90.0f, 30.0f, 56.0f, 56.0f };
        bool closeHover = CheckCollisionPointRec(mouse3, closeBtn3);
        bool closeClick = closeHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || closeClick)
            ui.currentScreen = 0;
    }
    else if (ui.currentScreen == 5) {
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            ui.loadSelection--;
            if (ui.loadSelection < 0) ui.loadSelection = 3;
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            ui.loadSelection++;
            if (ui.loadSelection > 3) ui.loadSelection = 0;
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (LoadGameSlot(game, ui.loadSelection)) {
                ui.p1HeroSelection = game.p1HeroSelection;
                ui.p2HeroSelection = game.p2HeroSelection;
                ResetHeroAnimState(ui);
                ui.currentScreen = 1;
            }
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
                if (ui.activeInputField == 0 && ui.p1LetterCount < 10) {
                    ui.p1NameInput[ui.p1LetterCount] = (char)key;
                    ui.p1NameInput[ui.p1LetterCount+1] = '\0';
                    ui.p1LetterCount++;
                } else if (ui.activeInputField == 1 && ui.p2LetterCount < 10) {
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
                InitGame(game);
                game.inputType = savedInput;
                
                if (ui.p1LetterCount > 0) strcpy(game.player1.name, ui.p1NameInput);
                else strcpy(game.player1.name, "Player 1");
                
                if (ui.p2LetterCount > 0) strcpy(game.player2.name, ui.p2NameInput);
                else strcpy(game.player2.name, "Player 2");

                ResetHeroAnimState(ui);
                ui.currentScreen = 1; 
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0; 
    }
    else if (ui.currentScreen == 8) {
        // Animate all hero idle sprites (indices 0-5)
        float dt8 = GetFrameTime();
        for (int i = 0; i < 6; i++) {
            CharAnim& anim = ui.heroIdle[i];
            if (anim.frameCount <= 0) continue;
            anim.frameTimer += dt8;
            if (anim.frameTimer >= anim.frameDuration) {
                anim.currentFrame = (anim.currentFrame + 1) % anim.frameCount;
                anim.frameTimer = 0.0f;
            }
        }

        const int MAX_HEROES = 5; // black_knight hidden (index 0 reserved)
        // Maps selection index (0,1,2,3,4) → asset index (1,2,3,4,5) skipping black_knight
        const int heroMap[5] = {1, 2, 3, 4, 5};

        if (ui.selectionPhase == -1) {
            // Chon che do choi: VS Player hoac VS Bot hoac Bot vs Bot
            int currentMode = game.isBotVsBot ? 2 : (game.isVsBot ? 1 : 0);
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) currentMode = (currentMode - 1 + 3) % 3;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) currentMode = (currentMode + 1) % 3;
            game.isBotVsBot = (currentMode == 2);
            game.isVsBot = (currentMode == 1);
            if (IsKeyPressed(KEY_ENTER)) ui.selectionPhase = 0;
            if (IsKeyPressed(KEY_ESCAPE)) ui.currentScreen = 0;
        }
        else if (ui.selectionPhase == 0) {
            // p1 pick hero
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) ui.p1HeroSelection = (ui.p1HeroSelection - 1 + MAX_HEROES) % MAX_HEROES;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) ui.p1HeroSelection = (ui.p1HeroSelection + 1) % MAX_HEROES;
            if (IsKeyPressed(KEY_ENTER)) {
                if (game.isBotVsBot) {
                    // Bot vs Bot: skip name input, randomize bot hero and start the game
                    const char* heroNamesUpd[5] = {"Fire Knight", "Green Archer", "Earth Assassin", "Metal Blade", "Water Mage"};
                    int botIdx = rand() % 4;
                    if (botIdx >= ui.p1HeroSelection) botIdx++;
                    ui.p2HeroSelection = botIdx;

                    int savedInput = game.inputType;
                    InitGame(game);
                    game.inputType = savedInput;

                    snprintf(game.player1.name, sizeof(game.player1.name),
                        "BOT 1 - %s", heroNamesUpd[ui.p1HeroSelection]);
                    snprintf(game.player2.name, sizeof(game.player2.name),
                        "BOT 2 - %s", heroNamesUpd[botIdx]);

                    game.p1HeroSelection = ui.p1HeroSelection;
                    game.p2HeroSelection = ui.p2HeroSelection;
                    ResetHeroAnimState(ui);
                    ui.currentScreen = 1;
                } else {
                    ui.selectionPhase = 1;
                }
            }
            if (IsKeyPressed(KEY_ESCAPE)) ui.selectionPhase = -1;
        }
        else if (ui.selectionPhase == 1) {
            // p1 set name
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && ui.p1LetterCount < 10) {
                    ui.p1NameInput[ui.p1LetterCount] = (char)key;
                    ui.p1NameInput[ui.p1LetterCount+1] = '\0';
                    ui.p1LetterCount++;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && ui.p1LetterCount > 0) {
                ui.p1LetterCount--;
                ui.p1NameInput[ui.p1LetterCount] = '\0';
            }
            if (IsKeyPressed(KEY_ESCAPE)) ui.selectionPhase = 0;
            else if (IsKeyPressed(KEY_ENTER)) {
                // Hero names (same order as heroMap: asset 1-5)
                const char* heroNamesUpd[5] = {"Fire Knight", "Green Archer", "Earth Assassin", "Metal Blade", "Water Mage"};
                if (game.isVsBot || game.isBotVsBot) {
                    // Random bot hero: pick from 0-4 excluding p1HeroSelection
                    int botIdx = rand() % 4; // 4 remaining choices
                    if (botIdx >= ui.p1HeroSelection) botIdx++; // skip p1's slot
                    ui.p2HeroSelection = botIdx;

                    int savedInput = game.inputType;
                    InitGame(game);
                    game.inputType = savedInput;

                    // P1 name: typed or "P1 - HeroName" (or BOT 1)
                    if (game.isBotVsBot) {
                        snprintf(game.player1.name, sizeof(game.player1.name),
                            "BOT 1 - %s", heroNamesUpd[ui.p1HeroSelection]);
                    } else {
                        if (ui.p1LetterCount > 0)
                            strcpy(game.player1.name, ui.p1NameInput);
                        else {
                            snprintf(game.player1.name, sizeof(game.player1.name),
                                "P1 - %s", heroNamesUpd[ui.p1HeroSelection]);
                        }
                    }
                    // BOT name: "BOT 2 - HeroName" or "BOT - HeroName"
                    const char* botPrefix = game.isBotVsBot ? "BOT 2" : "BOT";
                    snprintf(game.player2.name, sizeof(game.player2.name),
                        "%s - %s", botPrefix, heroNamesUpd[ui.p2HeroSelection]);

                    game.p1HeroSelection = ui.p1HeroSelection;
                    game.p2HeroSelection = ui.p2HeroSelection;
                    ResetHeroAnimState(ui);
                    ui.currentScreen = 1;
                } else {
                    // Make sure P2 starts on a different hero than P1
                    if (ui.p2HeroSelection == ui.p1HeroSelection)
                        ui.p2HeroSelection = (ui.p1HeroSelection + 1) % MAX_HEROES;
                    ui.selectionPhase = 2;
                }
            }
        }
        else if (ui.selectionPhase == 2) {
            // p2 picks hero, skipping the one P1 already chose
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
                do { ui.p2HeroSelection = (ui.p2HeroSelection - 1 + MAX_HEROES) % MAX_HEROES; }
                while (ui.p2HeroSelection == ui.p1HeroSelection);
            }
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
                do { ui.p2HeroSelection = (ui.p2HeroSelection + 1) % MAX_HEROES; }
                while (ui.p2HeroSelection == ui.p1HeroSelection);
            }
            if (IsKeyPressed(KEY_ENTER) && ui.p2HeroSelection != ui.p1HeroSelection) ui.selectionPhase = 3;
            if (IsKeyPressed(KEY_ESCAPE)) ui.selectionPhase = 1;
        }
        else if (ui.selectionPhase == 3) {
            // p2 set name
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && ui.p2LetterCount < 10) {
                    ui.p2NameInput[ui.p2LetterCount] = (char)key;
                    ui.p2NameInput[ui.p2LetterCount+1] = '\0';
                    ui.p2LetterCount++;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && ui.p2LetterCount > 0) {
                ui.p2LetterCount--;
                ui.p2NameInput[ui.p2LetterCount] = '\0';
            }
            if (IsKeyPressed(KEY_ESCAPE)) ui.selectionPhase = 2;
            else if (IsKeyPressed(KEY_ENTER)) {
                const char* heroNamesUpd[5] = {"Fire Knight", "Green Archer", "Earth Assassin", "Metal Blade", "Water Mage"};
                int savedInput = game.inputType;
                InitGame(game);
                game.inputType = savedInput;
                // P1 name: typed or "P1 - HeroName"
                if (ui.p1LetterCount > 0)
                    strcpy(game.player1.name, ui.p1NameInput);
                else
                    snprintf(game.player1.name, sizeof(game.player1.name),
                        "P1 - %s", heroNamesUpd[ui.p1HeroSelection]);
                // P2 name: typed or "P2 - HeroName"
                if (ui.p2LetterCount > 0)
                    strcpy(game.player2.name, ui.p2NameInput);
                else
                    snprintf(game.player2.name, sizeof(game.player2.name),
                        "P2 - %s", heroNamesUpd[ui.p2HeroSelection]);
                game.p1HeroSelection = ui.p1HeroSelection;
                game.p2HeroSelection = ui.p2HeroSelection;
                ResetHeroAnimState(ui);
                ui.currentScreen = 1;
            }
        }

        // ── Update char select particles ──
        {
            const float cardW   = 320.0f;
            const float cardH   = 510.0f;
            const float cardGap = 24.0f;
            const float totalCW = 5*cardW + 4*cardGap;
            const float cardStartX = (1920.0f - totalCW) / 2.0f;
            const float cardY      = 310.0f;
            int curSel = (ui.selectionPhase <= 1) ? ui.p1HeroSelection : ui.p2HeroSelection;
            float dt = GetFrameTime();

            // Spawn new particles for selected card
            int spawnSlot = -1;
            for (int k = 0; k < UIState::MAX_CHAR_PARTICLES; k++) {
                if (ui.charParticles[k].life <= 0.0f) { spawnSlot = k; break; }
            }
            // Spawn ~2 per frame for selected card
            for (int spawn = 0; spawn < 2 && spawnSlot >= 0; spawn++) {
                float cx = cardStartX + curSel * (cardW + cardGap);
                float footY = cardY + cardH * 0.88f - (curSel == (int)curSel ? 16.0f : 0.0f);
                auto& p = ui.charParticles[spawnSlot];
                p.heroID = curSel;
                p.life   = 1.0f;
                p.type   = (rand() % 5 == 0) ? 1 : 0; // 20% rune, 80% ember
                if (p.type == 0) {
                    // Rising ember from foot area
                    p.x     = cx + cardW * 0.2f + (rand() % (int)(cardW * 0.6f));
                    p.y     = footY - (rand() % 30);
                    p.vx    = ((rand() % 60) - 30) * 0.01f;
                    p.vy    = -((rand() % 50) + 30) * 0.012f;
                    p.size  = (float)(rand() % 4 + 2);
                    p.alpha = 0.7f + (rand() % 30) / 100.0f;
                    p.angle = 0.0f; p.angleV = 0.0f;
                } else {
                    // Rune diamond floating around card border
                    int side = rand() % 4;
                    if (side == 0) { p.x = cx + rand() % (int)cardW; p.y = cardY; }
                    else if (side == 1) { p.x = cx + cardW; p.y = cardY + rand() % (int)cardH; }
                    else if (side == 2) { p.x = cx + rand() % (int)cardW; p.y = cardY + cardH; }
                    else { p.x = cx; p.y = cardY + rand() % (int)cardH; }
                    p.vx    = ((rand() % 40) - 20) * 0.008f;
                    p.vy    = -((rand() % 30) + 10) * 0.010f;
                    p.size  = (float)(rand() % 5 + 4);
                    p.alpha = 0.5f + (rand() % 40) / 100.0f;
                    p.angle = (float)(rand() % 360);
                    p.angleV = ((rand() % 60) - 30) * 0.05f;
                }
                // find next free slot
                spawnSlot = -1;
                for (int k = spawnSlot + 1; k < UIState::MAX_CHAR_PARTICLES; k++) {
                    if (ui.charParticles[k].life <= 0.0f) { spawnSlot = k; break; }
                }
            }

            // Update all particles
            for (int k = 0; k < UIState::MAX_CHAR_PARTICLES; k++) {
                auto& p = ui.charParticles[k];
                if (p.life <= 0.0f) continue;
                p.life  -= dt * 0.55f;
                p.x     += p.vx;
                p.y     += p.vy;
                p.vx    *= 0.995f;
                p.angle += p.angleV;
                p.alpha  = p.life * 0.9f;
                if (p.life < 0.0f) p.life = 0.0f;
            }
        }
    }
}

void DrawMenuScreens(const GameState& game, const UIState& ui) {
    if (ui.currentScreen == 0) {
        DrawTexturePro(ui.bgMenu, { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

        // Moody gradient sweeping in from the right
        DrawRectangleGradientH(950, 0, 970, 1080, Fade(BLACK, 0.0f), Fade(BLACK, 0.85f));

        // Draw the game title above the menu
        const char* titleText = "CARO BATTLE";
        int titleSize = 120;
        int titleW = MeasureTextCustomX(ui.mainFont, titleText, titleSize);
        int titleY = 130;

        float t = (float)GetTime();
        float pulse = 0.5f + 0.5f * sinf(t * 2.0f); // oscillates 0.0 -> 1.0
        Color goldColor2 = { 255, 200, 50, 255 };
        float tx0 = 1500.0f - titleW / 2.0f;

        // Dark shadow
        DrawTextCustom(ui.mainFont, titleText, tx0 + 6, titleY + 6, titleSize, Fade(BLACK, 0.9f));
        // Wide red glow (strong pulse)
        for (int d = 1; d <= 6; d++)
            DrawTextCustom(ui.mainFont, titleText, tx0, titleY, titleSize, Fade(RED, pulse * 0.13f));
        // Spreading gold glow (softer pulse, always on)
        for (int d = 2; d <= 5; d++) {
            DrawTextCustom(ui.mainFont, titleText, tx0 - d, titleY,     titleSize, Fade(goldColor2, 0.18f + pulse * 0.12f));
            DrawTextCustom(ui.mainFont, titleText, tx0 + d, titleY,     titleSize, Fade(goldColor2, 0.18f + pulse * 0.12f));
            DrawTextCustom(ui.mainFont, titleText, tx0,     titleY - d, titleSize, Fade(goldColor2, 0.18f + pulse * 0.12f));
            DrawTextCustom(ui.mainFont, titleText, tx0,     titleY + d, titleSize, Fade(goldColor2, 0.18f + pulse * 0.12f));
        }
        // Main bright gold text
        DrawTextCustom(ui.mainFont, titleText, tx0, titleY, titleSize, goldColor2);

        // --- PARTICLES: embers rising ---
        for (int i = 0; i < UIState::MAX_EMBERS; i++) {
            const UIState::Ember& e = ui.embers[i];
            if (e.alpha <= 0.0f) continue;
            Color emberColor = Fade((Color){255, 160, 40, 255}, e.alpha);
            DrawCircleV({e.x, e.y}, e.size, emberColor);
            DrawCircleV({e.x, e.y}, e.size * 0.45f, Fade(WHITE, e.alpha * 0.7f));
        }

        // --- SMOOTH HIGHLIGHT BAR ---
        const float menuBlockX = 1500.0f;
        const float btnWidth   = 800.0f;
        const float btnHeight  = 80.0f;
        const float startY2    = 350.0f;
        const float gap2       = btnHeight + 35.0f;
        float targetY = startY2 + ui.menuSelection * gap2;

        // Highlight bar, many thin layers stacked for a blur/glow feel
        float hy = ui.menuScrollY + btnHeight / 2.0f; // vertical center of the item
        float hw = btnWidth;
        Color gc = {255, 200, 50, 255};
        // Outermost layer, very wide and faint
        DrawRectangleGradientH((int)(menuBlockX - hw/2), (int)(hy - 28), (int)(hw/2), 56,
            Fade(gc, 0.0f), Fade(gc, 0.06f));
        DrawRectangleGradientH((int)(menuBlockX),        (int)(hy - 28), (int)(hw/2), 56,
            Fade(gc, 0.06f), Fade(gc, 0.0f));
        // Middle layer
        DrawRectangleGradientH((int)(menuBlockX - hw/2 + 60), (int)(hy - 16), (int)(hw/2 - 60), 32,
            Fade(gc, 0.0f), Fade(gc, 0.12f));
        DrawRectangleGradientH((int)(menuBlockX),               (int)(hy - 16), (int)(hw/2 - 60), 32,
            Fade(gc, 0.12f), Fade(gc, 0.0f));
        // Inner layer, thin and brightest at the center
        DrawRectangleGradientH((int)(menuBlockX - hw/2 + 120), (int)(hy - 6), (int)(hw/2 - 120), 12,
            Fade(gc, 0.0f), Fade(gc, 0.22f));
        DrawRectangleGradientH((int)(menuBlockX),                (int)(hy - 6), (int)(hw/2 - 120), 12,
            Fade(gc, 0.22f), Fade(gc, 0.0f));

        const char* menuOptions[TOTAL_MENU_ITEMS] = {
            "NEW GAME",
            "LOAD GAME",
            "SETTINGS",
            "HELP",
            "CREDITS",
            "EXIT"
        };
        
        float centerX = 1500.0f;
        float startY = 350.0f;
        float gap = btnHeight + 35.0f;

        Color goldColor = { 255, 200, 50, 255 };

        for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
            Rectangle rect = { centerX - btnWidth/2.0f, startY + gap*i, btnWidth, btnHeight };
            bool isSelected = (ui.menuSelection == i);

            int fontSize     = isSelected ? 52 : 42;
            int textW = MeasureTextCustomX(ui.mainFont, menuOptions[i], fontSize);
            int textH = MeasureTextCustomY(ui.mainFont, menuOptions[i], fontSize);
            float tx = rect.x + (btnWidth - textW) / 2.0f;
            float ty = rect.y + (btnHeight - textH) / 2.0f;

            if (isSelected) {
                // strong gold glow spreading in all directions
                for (int d = 1; d <= 5; d++) {
                    float a = 0.35f - d * 0.06f;
                    DrawTextCustom(ui.mainFont, menuOptions[i], tx - d, ty,     fontSize, Fade(goldColor, a));
                    DrawTextCustom(ui.mainFont, menuOptions[i], tx + d, ty,     fontSize, Fade(goldColor, a));
                    DrawTextCustom(ui.mainFont, menuOptions[i], tx,     ty - d, fontSize, Fade(goldColor, a));
                    DrawTextCustom(ui.mainFont, menuOptions[i], tx,     ty + d, fontSize, Fade(goldColor, a));
                }
                // arrows on both sides
                DrawTextCustom(ui.mainFont, ">", tx - 45, ty, fontSize, goldColor);
                DrawTextCustom(ui.mainFont, "<", tx + textW + 15, ty, fontSize, goldColor);
                // main bright gold text
                DrawTextCustom(ui.mainFont, menuOptions[i], tx, ty, fontSize, goldColor);
            } else {
                DrawTextCustom(ui.mainFont, menuOptions[i], tx, ty, fontSize, Fade(LIGHTGRAY, 0.55f));
            }
        }
    }
    else if (ui.currentScreen == 3) {
        // Sunset battlefield background, covering the full 1920x1080 canvas
        DrawTexturePro(ui.helpBg, { 0, 0, (float)ui.helpBg.width, (float)ui.helpBg.height },
            { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

        // Full-screen dark scrim, same darkness as the Settings screen
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.78f));

        // Scale-in: the panel opens over 250ms when entering the screen
        float introT = ui.helpScrollTimer / 0.25f;
        if (introT > 1.0f) introT = 1.0f;
        float easeOut = 1.0f - (1.0f - introT) * (1.0f - introT); // ease-out quadratic
        float a3 = easeOut; // overall alpha

        float t3 = (float)GetTime();
        float pulse3 = 0.5f + 0.5f * sinf(t3 * 2.0f);

        Color goldColor3 = Fade(Color{ 255, 180, 0, 255 }, a3);   // Gothic Gold (same as Settings)
        Color goldBright = Fade(Color{ 255, 205, 70, 255 }, a3);

        float centerX3 = 960.0f;

        // Helper to draw a gothic ornament bar (diamonds + crosses), matching Settings
        auto DrawOrnateLine3 = [](float x, float y, float width, Color c) {
            DrawLineEx({ x + 40, y }, { x + width - 40, y }, 4.0f, c);
            DrawLineEx({ x + 40, y - 6 }, { x + width - 40, y - 6 }, 1.0f, Fade(c, 0.6f));
            DrawLineEx({ x + 40, y + 6 }, { x + width - 40, y + 6 }, 1.0f, Fade(c, 0.6f));
            auto DrawDiamond = [](float cx, float cy, float w, float h, Color color) {
                DrawTriangle({ cx - w / 2, cy }, { cx, cy + h / 2 }, { cx + w / 2, cy }, color);
                DrawTriangle({ cx - w / 2, cy }, { cx + w / 2, cy }, { cx, cy - h / 2 }, color);
            };
            auto DrawCross = [](float cx, float cy, float size, Color color) {
                DrawRectangle(cx - size / 6, cy - size / 2, size / 3, size, color);
                DrawRectangle(cx - size / 2, cy - size / 6, size, size / 3, color);
            };
            float cx = x + width / 2;
            // Center medallion
            DrawDiamond(cx, y, 40, 40, c);
            DrawDiamond(cx, y, 30, 30, BLACK);
            DrawDiamond(cx, y, 16, 16, c);
            DrawCross(cx, y, 12, BLACK);
            // End caps on both ends
            DrawDiamond(x + 20, y, 30, 30, c);
            DrawDiamond(x + 20, y, 16, 16, BLACK);
            DrawCross(x + 20, y, 10, c);
            DrawDiamond(x + width - 20, y, 30, 30, c);
            DrawDiamond(x + width - 20, y, 16, 16, BLACK);
            DrawCross(x + width - 20, y, 10, c);
        };

        // === GOTHIC PANEL: dark background + double iron border + corner diamonds ===
        float panelW = 1100.0f;
        float panelH = 640.0f;
        float panelX = centerX3 - panelW * 0.5f;
        float panelY = 540.0f - panelH * 0.5f;

        // Panel background: dark and slightly warm (brown-black) for an old stone/iron feel
        DrawRectangleRounded({ panelX, panelY, panelW, panelH }, 0.03f, 8, Fade(Color{ 18, 12, 8, 255 }, 0.92f * a3));
        // Double gold iron border
        DrawRectangleRoundedLines({ panelX, panelY, panelW, panelH }, 0.03f, 8, Fade(goldColor3, 0.85f));
        DrawRectangleRoundedLines({ panelX + 6, panelY + 6, panelW - 12, panelH - 12 }, 0.03f, 8, Fade(goldColor3, 0.30f));

        // Diamonds at the 4 panel corners
        auto DrawCornerDiamond = [&](float cx, float cy) {
            DrawTriangle({ cx - 14, cy }, { cx, cy + 14 }, { cx + 14, cy }, goldColor3);
            DrawTriangle({ cx - 14, cy }, { cx + 14, cy }, { cx, cy - 14 }, goldColor3);
            DrawTriangle({ cx - 7, cy }, { cx, cy + 7 }, { cx + 7, cy }, Fade(BLACK, a3));
            DrawTriangle({ cx - 7, cy }, { cx + 7, cy }, { cx, cy - 7 }, Fade(BLACK, a3));
        };
        DrawCornerDiamond(panelX, panelY);
        DrawCornerDiamond(panelX + panelW, panelY);
        DrawCornerDiamond(panelX, panelY + panelH);
        DrawCornerDiamond(panelX + panelW, panelY + panelH);

        // Text color on the dark panel: light cream for high contrast
        Color bodyCol3 = Fade(Color{ 235, 220, 195, 255 }, a3);
        Color hintCol3 = Fade(Color{ 190, 165, 120, 255 }, a3);

        // === LAYOUT: vertically center the content block inside the panel ===
        const char* helpTitle = "HOW TO PLAY";
        int titleSize3 = 56;
        const char* helpLines[] = {
            "Two players take turns placing X and O.",
            "The first to line up 5 marks in a row",
            "horizontally, vertically or diagonally wins.",
            "",
            "Click a cell on the board to place a mark,",
            "or use the W A S D keys + Enter.",
            "",
            "Have a great game!"
        };
        int lineCount3 = 8;
        int bodySize3 = 28;
        float bodyLineHeight = bodySize3 * 1.55f;

        float gapTitleToLine = 22.0f;
        float gapLineToBody = 40.0f;
        float gapBodyToHint = 36.0f;
        int hintSize3 = 22;

        float blockH = titleSize3 + gapTitleToLine + 4.0f + gapLineToBody
                     + lineCount3 * bodyLineHeight + gapBodyToHint + hintSize3;
        float cursorY = panelY + (panelH - blockH) * 0.5f;

        // --- Title: shadow + red glow + gold, matching the SETTINGS title ---
        int titleW3 = MeasureTextCustomX(ui.mainFont, helpTitle, titleSize3);
        float titleX = centerX3 - titleW3 * 0.5f;
        DrawTextCustom(ui.mainFont, helpTitle, (int)(titleX + 4), (int)(cursorY + 4), titleSize3, Fade(BLACK, 0.8f * a3));
        DrawTextCustom(ui.mainFont, helpTitle, (int)titleX, (int)cursorY, titleSize3, Fade(RED, 0.5f * a3));
        for (int d = 2; d <= 4; d++) {
            DrawTextCustom(ui.mainFont, helpTitle, (int)(titleX - d), (int)cursorY, titleSize3, Fade(goldBright, (0.10f + pulse3 * 0.06f) * a3));
            DrawTextCustom(ui.mainFont, helpTitle, (int)(titleX + d), (int)cursorY, titleSize3, Fade(goldBright, (0.10f + pulse3 * 0.06f) * a3));
        }
        DrawTextCustom(ui.mainFont, helpTitle, (int)titleX, (int)cursorY, titleSize3, goldColor3);
        cursorY += titleSize3 + gapTitleToLine;

        // --- Gothic ornament bar below the title ---
        DrawOrnateLine3(centerX3 - 240.0f, cursorY + 2.0f, 480.0f, goldColor3);
        cursorY += 4.0f + gapLineToBody;

        // --- Rule text ---
        for (int i = 0; i < lineCount3; i++) {
            if (helpLines[i][0] != '\0') { // skip empty paragraph-spacer lines
                int lw = MeasureTextCustomX(ui.mainFont, helpLines[i], bodySize3);
                DrawTextCustom(ui.mainFont, helpLines[i], (int)(centerX3 - lw * 0.5f), (int)(cursorY + i * bodyLineHeight), bodySize3, bodyCol3);
            }
        }
        cursorY += lineCount3 * bodyLineHeight + gapBodyToHint;

        // --- Exit hint ---
        const char* hintText = "Press ESC / ENTER to go back";
        int hintW3 = MeasureTextCustomX(ui.mainFont, hintText, hintSize3);
        DrawTextCustom(ui.mainFont, hintText, (int)(centerX3 - hintW3 * 0.5f), (int)cursorY, hintSize3, hintCol3);

        // Top-right X button, an always-visible escape route, not just a key shortcut
        Vector2 mouseHelp = GetMousePosition();
        Rectangle closeBtnDraw = { 1920.0f - 90.0f, 30.0f, 56.0f, 56.0f };
        bool closeHoverDraw = CheckCollisionPointRec(mouseHelp, closeBtnDraw);
        Color closeBg = Fade(BLACK, closeHoverDraw ? 0.85f : 0.65f);
        Color closeFg = closeHoverDraw ? Color{ 255, 200, 50, 255 } : Color{ 220, 200, 170, 220 };
        DrawRectangleRounded(closeBtnDraw, 0.25f, 6, closeBg);
        DrawRectangleRoundedLines(closeBtnDraw, 0.25f, 6, Fade(closeFg, 0.8f));
        int xW = MeasureTextCustomX(ui.mainFont, "X", 26);
        DrawTextCustom(ui.mainFont, "X", (int)(closeBtnDraw.x + (closeBtnDraw.width - xW) * 0.5f), (int)(closeBtnDraw.y + 13), 26, closeFg);
    }
    else if (ui.currentScreen == 2) {
        DrawTexturePro(ui.bgSettings, { 0, 0, (float)ui.bgSettings.width, (float)ui.bgSettings.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

        // Full-screen dark background instead of a plain rectangle
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.75f));

        Color selectedColor = { 255, 180, 0, 255 }; // Gothic Gold

        // Two ornament bars (top & bottom)
        auto DrawOrnateLine = [](float x, float y, float width, Color c) {
            // Main horizontal lines (Double line effect)
            DrawLineEx({x + 40, y}, {x + width - 40, y}, 4.0f, c);
            DrawLineEx({x + 40, y - 6}, {x + width - 40, y - 6}, 1.0f, Fade(c, 0.6f));
            DrawLineEx({x + 40, y + 6}, {x + width - 40, y + 6}, 1.0f, Fade(c, 0.6f));
            
            auto DrawDiamond = [](float cx, float cy, float w, float h, Color color) {
                Vector2 top = {cx, cy - h/2};
                Vector2 left = {cx - w/2, cy};
                Vector2 right = {cx + w/2, cy};
                Vector2 bottom = {cx, cy + h/2};
                DrawTriangle(left, bottom, right, color);
                DrawTriangle(left, right, top, color);
            };

            auto DrawCross = [](float cx, float cy, float size, Color color) {
                DrawRectangle(cx - size/6, cy - size/2, size/3, size, color);
                DrawRectangle(cx - size/2, cy - size/6, size, size/3, color);
            };

            // Center Medallion
            float cx = x + width / 2;
            DrawDiamond(cx, y, 40, 40, c);
            DrawDiamond(cx, y, 30, 30, BLACK);
            DrawDiamond(cx, y, 16, 16, c);
            DrawCross(cx, y, 12, BLACK); // Inner cross

            // Repeated tiny diamonds along the line
            int segments = 12;
            for(int i = 1; i < segments; i++) {
                float px = x + 40 + i * ((width - 80) / segments);
                if (abs(px - cx) > 50) { // Don't draw over center medallion
                    DrawDiamond(px, y, 10, 10, c);
                    DrawCircle(px, y, 2.0f, BLACK); // Small hole
                }
            }

            // Elaborate End Caps (Left)
            DrawDiamond(x + 20, y, 30, 30, c);
            DrawDiamond(x + 20, y, 16, 16, BLACK);
            DrawCross(x + 20, y, 10, c);
            DrawDiamond(x, y, 14, 14, c);
            
            // Elaborate End Caps (Right)
            DrawDiamond(x + width - 20, y, 30, 30, c);
            DrawDiamond(x + width - 20, y, 16, 16, BLACK);
            DrawCross(x + width - 20, y, 10, c);
            DrawDiamond(x + width, y, 14, 14, c);
        };

        DrawOrnateLine(1920/2 - 600, 100, 1200, selectedColor);
        DrawOrnateLine(1920/2 - 600, 850, 1200, selectedColor);

        // Moody SETTINGS title
        const char* titleText = "SETTINGS";
        int titleSize = 100;
        int titleW = MeasureTextCustomX(ui.mainFont, titleText, titleSize);
        int titleY = 150;
        
        DrawTextCustom(ui.mainFont, titleText, (1920 - titleW)/2 + 6, titleY + 6, titleSize, Fade(BLACK, 0.8f));
        DrawTextCustom(ui.mainFont, titleText, (1920 - titleW)/2, titleY, titleSize, Fade(RED, 0.6f));
        DrawTextCustom(ui.mainFont, titleText, (1920 - titleW)/2, titleY, titleSize, (Color){ 255, 200, 50, 255 });

        const char* setOptions[TOTAL_SETTING_ITEMS] = {
          "Mouse Control (Recommended)",
          "Keyboard Control (WASD + Enter)",
          "Music Volume",
          "Toggle Music",
          "Back to Menu"
        };

        Vector2 mouse = GetMousePosition();

        for (int i = 0; i < TOTAL_SETTING_ITEMS; i++) {
            int yPos = 350 + i * 80;
            if (i >= 3) yPos += 40; // extra spacing for the volume bar
            
            Color textColor = LIGHTGRAY;
            if (i == ui.settingSelection) textColor = selectedColor;

            // Highlight the currently active input mode
            if (i == 0 && game.inputType == 0) textColor = WHITE;
            if (i == 1 && game.inputType == 1) textColor = WHITE;
            if (i == ui.settingSelection) textColor = selectedColor;

            char displayText[256];
            if (i == 3) { 
                const char* status = ui.musicEnabled ? " [ON]" : " [OFF]";
                snprintf(displayText, sizeof(displayText), "%s%s", setOptions[i], status);
            } else {
                snprintf(displayText, sizeof(displayText), "%s", setOptions[i]);
            }
            if (i == 0 && game.inputType == 0) snprintf(displayText, sizeof(displayText), "%s [ACTIVE]", setOptions[i]);
            if (i == 1 && game.inputType == 1) snprintf(displayText, sizeof(displayText), "%s [ACTIVE]", setOptions[i]);

            if (i == ui.settingSelection) {
                // Add angle brackets when selected
                char selectedText[256];
                snprintf(selectedText, sizeof(selectedText), "> %s <", displayText);
                int w = MeasureTextCustomX(ui.mainFont, selectedText, 40);
                DrawTextCustom(ui.mainFont, selectedText, (1920 - w)/2, yPos, 40, textColor);
            } else {
                int w = MeasureTextCustomX(ui.mainFont, displayText, 40);
                DrawTextCustom(ui.mainFont, displayText, (1920 - w)/2, yPos, 40, textColor);
            }
        }

        const int barWidth = 400;
        const int barHeight = 24;
        const int barX = (1920 - barWidth) / 2;
        const int barY = 350 + 2 * 80 + 60;
        
        Rectangle volumeBar = { (float)barX, (float)barY, (float)barWidth, (float)barHeight };
        DrawRectangleRec(volumeBar, DARKGRAY);
        DrawRectangle(barX, barY, (int)(barWidth * ui.musicVolume), barHeight, MAROON);
        
        Rectangle knob = {
            barX + ui.musicVolume * barWidth - 8.0f,
            (float)barY - 12.0f,
            16.0f,
            48.0f
        };
        DrawRectangleRec(knob, selectedColor);

        DrawTextCustom(ui.mainFont, TextFormat("%i%%", (int)(ui.musicVolume * 100)), barX + barWidth + 20, barY - 5, 30, WHITE);
    }
   else if (ui.currentScreen == 5) {
        DrawTexturePro(ui.bgLoadGame, { 0, 0, (float)ui.bgLoadGame.width, (float)ui.bgLoadGame.height },
            { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.52f));
        // Vignette, dark edges and bright center
        DrawRectangleGradientH(0, 0, 500, 1080, Fade(BLACK, 0.55f), Fade(BLACK, 0.0f));
        DrawRectangleGradientH(1420, 0, 500, 1080, Fade(BLACK, 0.0f), Fade(BLACK, 0.55f));
        DrawRectangleGradientV(0, 0, 1920, 160, Fade(BLACK, 0.50f), Fade(BLACK, 0.0f));
        DrawRectangleGradientV(0, 880, 1920, 200, Fade(BLACK, 0.0f), Fade(BLACK, 0.70f));

        float t = (float)GetTime();
        float glowPulse = 0.5f + 0.5f * sinf(t * 2.0f);

        Color gothicGold  = { 255, 180,   0, 255 };
        Color accentDim   = { 160, 110,   0, 255 };
        Color redAccent   = { 200,  60,  30, 255 };
        Color silverColor = { 220, 210, 195, 255 };
        Color dimColor    = { 150, 130, 100, 255 };
        Color warmWhite   = { 255, 245, 220, 255 };

        // ── DrawOrnateLine ──
        auto DD5 = [](float cx, float cy, float w, float h, Color col) {
            DrawTriangle({cx-w/2,cy},{cx,cy+h/2},{cx+w/2,cy},col);
            DrawTriangle({cx-w/2,cy},{cx+w/2,cy},{cx,cy-h/2},col);
        };
        auto DC5 = [](float cx, float cy, float sz, Color col) {
            DrawRectangle((int)(cx-sz/6),(int)(cy-sz/2),(int)(sz/3),(int)sz,col);
            DrawRectangle((int)(cx-sz/2),(int)(cy-sz/6),(int)sz,(int)(sz/3),col);
        };
        auto DrawOrnateLine5 = [&](float x, float y, float width, Color c) {
            DrawLineEx({x+40,y},{x+width-40,y},4.0f,c);
            DrawLineEx({x+40,y-6},{x+width-40,y-6},1.0f,Fade(c,0.6f));
            DrawLineEx({x+40,y+6},{x+width-40,y+6},1.0f,Fade(c,0.6f));
            float cx=x+width/2;
            DD5(cx,y,40,40,c); DD5(cx,y,30,30,BLACK); DD5(cx,y,16,16,c); DC5(cx,y,12,BLACK);
            for(int s=1;s<12;s++){float px=x+40+s*((width-80)/12);if(fabsf(px-cx)>50){DD5(px,y,10,10,c);DrawCircle((int)px,(int)y,2.0f,BLACK);}}
            DD5(x+20,y,30,30,c);DD5(x+20,y,16,16,BLACK);DC5(x+20,y,10,c);DD5(x,y,14,14,c);
            DD5(x+width-20,y,30,30,c);DD5(x+width-20,y,16,16,BLACK);DC5(x+width-20,y,10,c);DD5(x+width,y,14,14,c);
        };

        // ── Top ornament line ──
        DrawOrnateLine5(80, 92, 1760, gothicGold);

        // ── Title ──
        const char* title = "LOAD GAME";
        int titleW = MeasureTextCustomX(ui.mainFont, title, 80);
        DrawTextCustom(ui.mainFont, title, 1920/2 - titleW/2 + 3, 108, 80, Fade(BLACK, 0.9f));
        DrawTextCustom(ui.mainFont, title, 1920/2 - titleW/2,     104, 80, gothicGold);

        // ── Layout constants ──
        const float listX  = 80.0f;
        const float listY  = 210.0f;
        const float listW  = 620.0f;
        const float slotH  = 170.0f;
        const float slotGap = 10.0f;
        float panelH = 4 * slotH + 3 * slotGap;

        // ── Left panel: slot list ──
        for (int i = 0; i < 4; i++) {
            float sy = listY + i * (slotH + slotGap);
            bool isSelected = (i == ui.loadSelection);
            GameState tempGame;
            bool hasData = PeekGameSlot(i, tempGame);

            // Selected: glow background + border
            if (isSelected) {
                // Warm gradient background
                DrawRectangleGradientH((int)listX, (int)sy+1, (int)listW, (int)slotH-1,
                    Fade(gothicGold, 0.18f), Fade(BLACK, 0.0f));
                // Gold border around the selected slot
                DrawRectangleLinesEx({listX, sy, listW, slotH}, 2.0f, Fade(gothicGold, 0.70f));
                // Bright left accent bar
                DrawRectangle((int)listX, (int)sy+1, 5, (int)slotH-1, Fade(gothicGold, 0.95f));
                // Outer glow pulse
                DrawRectangleLinesEx({listX-3, sy-3, listW+6, slotH+6}, 1.5f,
                    Fade(gothicGold, 0.25f * glowPulse));
            }

            // Top separator
            Color lineCol = isSelected ? Fade(gothicGold, 0.85f) : Fade(dimColor, 0.35f);
            DrawLineEx({listX, sy}, {listX + listW, sy}, isSelected ? 2.5f : 1.0f, lineCol);

            // Diamond bullet
            {
                float dcx = listX + 10, dcy = sy, dw = isSelected ? 16.0f : 10.0f, dh = isSelected ? 16.0f : 10.0f;
                Color dc = isSelected ? gothicGold : Fade(dimColor, 0.5f);
                DrawTriangle({dcx-dw/2,dcy},{dcx,dcy+dh/2},{dcx+dw/2,dcy},dc);
                DrawTriangle({dcx-dw/2,dcy},{dcx+dw/2,dcy},{dcx,dcy-dh/2},dc);
            }

            // Slot number label (top right), brighter gold
            const char* slotLabel = TextFormat("SLOT %d", i + 1);
            int slW = MeasureTextCustomX(ui.mainFont, slotLabel, 24);
            DrawTextCustom(ui.mainFont, slotLabel, (int)(listX + listW - slW - 12), (int)sy + 8, 24,
                isSelected ? gothicGold : Fade(gothicGold, 0.55f));

            if (hasData) {
                // ">" + save name — large
                Color nameCol = isSelected ? warmWhite : Fade(warmWhite, 0.75f);
                int nameSize  = isSelected ? 50 : 44;
                if (isSelected) {
                    DrawTextCustom(ui.mainFont, ">", (int)listX + 18, (int)sy + 34, 44, Fade(gothicGold, 0.9f));
                    DrawTextCustom(ui.mainFont, tempGame.saveName, (int)listX + 62, (int)sy + 34, nameSize,
                        Fade(BLACK, 0.6f));
                }
                DrawTextCustom(ui.mainFont, tempGame.saveName,
                    (int)listX + (isSelected ? 62 : 26), (int)sy + (isSelected ? 32 : 34), nameSize, nameCol);

                // Mode badge
                Color modeCol = isSelected ? gothicGold : Fade(gothicGold, 0.70f);
                DrawTextCustom(ui.mainFont, "Classic", (int)listX + 26, (int)(sy + slotH - 50), 30, modeCol);

                // Round (bottom right), stronger silver color
                const char* roundStr = TextFormat("Round %d", tempGame.roundCount);
                int rw = MeasureTextCustomX(ui.mainFont, roundStr, 26);
                DrawTextCustom(ui.mainFont, roundStr, (int)(listX + listW - rw - 14), (int)(sy + slotH - 48), 26,
                    isSelected ? silverColor : Fade(silverColor, 0.70f));

                // Save time, bolder with its own color
                DrawTextCustom(ui.mainFont, tempGame.saveTime, (int)listX + 26, (int)sy + 8, 22,
                    isSelected ? Fade(gothicGold, 0.80f) : Fade({200, 180, 130, 255}, 0.75f));
            } else {
                // Empty slot
                const char* emStr = "— Empty —";
                int emW = MeasureTextCustomX(ui.mainFont, emStr, 34);
                DrawTextCustom(ui.mainFont, emStr,
                    (int)(listX + listW/2 - emW/2), (int)(sy + slotH/2 - 20), 34,
                    Fade(dimColor, isSelected ? 0.55f : 0.25f));
            }
        }

        // Bottom separator
        float bottomY = listY + panelH;
        DrawLineEx({listX, bottomY}, {listX + listW, bottomY}, 1.5f, Fade(dimColor, 0.4f));
        { // small diamond at bottom left
            float dcx = listX + 10, dcy = bottomY;
            DrawTriangle({dcx-8,dcy},{dcx,dcy+8},{dcx+8,dcy},Fade(dimColor,0.4f));
            DrawTriangle({dcx-8,dcy},{dcx+8,dcy},{dcx,dcy-8},Fade(dimColor,0.4f));
        }

        // Vertical divider, more visible
        float divX = listX + listW + 40.0f;
        DrawLineEx({divX,   listY - 6}, {divX,   bottomY + 6}, 2.5f, Fade(gothicGold, 0.80f));
        DrawLineEx({divX+6, listY - 6}, {divX+6, bottomY + 6}, 1.0f, Fade(gothicGold, 0.25f));
        // Bright diamond at the midpoint
        float dmY = listY + panelH / 2.0f;
        DD5(divX+3, dmY, 24, 24, gothicGold); DD5(divX+3, dmY, 14, 14, BLACK); DC5(divX+3, dmY, 10, gothicGold);

        // ── Right panel: preview ──
        const float prevX = divX + 28.0f;
        const float prevY = listY;
        const float prevW = 1920.0f - prevX - 60.0f;
        const float prevH = panelH;

        GameState selGame;
        bool selHasData = PeekGameSlot(ui.loadSelection, selGame);

        if (selHasData) {
            // Save name, largest, gothicGold, glow pulse
            float snX = prevX;
            int snSize = 72;
            // Outer glow
            for (int g = 3; g >= 1; g--)
                DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX+g, (int)prevY+g, snSize,
                    Fade(gothicGold, 0.08f * glowPulse * (4-g)));
            DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX+3, (int)prevY+4, snSize, Fade(BLACK,0.90f));
            DrawTextCustom(ui.mainFont, selGame.saveName, (int)snX,   (int)prevY,   snSize, gothicGold);

            // Ornate underline
            DrawLineEx({prevX, prevY+82}, {prevX + prevW*0.44f, prevY+82}, 2.0f, Fade(gothicGold, 0.70f));
            DD5(prevX + prevW*0.44f + 12, prevY+82, 16, 16, gothicGold);

            // Mode badge
            float contentY = prevY + 100.0f;
            float infoX    = prevX;
            float infoW    = prevW * 0.40f;

            const char* modeStr = "Classic Mode";
            Color modeColor = gothicGold;
            int modeW = MeasureTextCustomX(ui.mainFont, modeStr, 32);
            DrawRectangleRounded({infoX, contentY, (float)(modeW+30), 46}, 0.3f, 6, Fade(modeColor, 0.20f));
            DrawRectangleRoundedLines({infoX, contentY, (float)(modeW+30), 46}, 0.3f, 6, Fade(modeColor, 0.75f));
            DrawTextCustom(ui.mainFont, modeStr, (int)infoX+15, (int)contentY+9, 32, modeColor);

            contentY += 62.0f;

            // Stats, brighter labels and larger values
            Color labelColor = {190, 170, 120, 255};
            auto DrawStat5 = [&](const char* label, const char* value, Color valCol, float y) {
                DrawTextCustom(ui.mainFont, label, (int)infoX, (int)y, 22, labelColor);
                // Value glow
                DrawTextCustom(ui.mainFont, value, (int)infoX+2, (int)(y+28), 44, Fade(valCol, 0.20f*glowPulse));
                DrawTextCustom(ui.mainFont, value, (int)infoX,   (int)(y+28), 44, valCol);
                DrawLineEx({infoX, y+76}, {infoX+infoW, y+76}, 1.0f, Fade(gothicGold, 0.20f));
            };
            DrawStat5("ROUND",     TextFormat("%d", selGame.roundCount), gothicGold,   contentY);
            DrawStat5("SAVED ON",  selGame.saveTime,                     silverColor,  contentY + 92);
            DrawStat5("NEXT TURN",
                selGame.isPlayer1Turn ? selGame.player1.name : selGame.player2.name,
                warmWhite, contentY + 184);

            // ── Players ──
            float pY = contentY + 284.0f;
            DrawLineEx({infoX, pY}, {infoX+infoW, pY}, 1.5f, Fade(gothicGold, 0.45f));
            DrawTextCustom(ui.mainFont, "PLAYERS", (int)infoX, (int)pY+8, 22, Fade(gothicGold, 0.85f));

            // P1, ember dot + bright name
            DrawCircleV({infoX+12, pY+58}, 6.0f, {255,140,30,255});
            DrawCircleV({infoX+12, pY+58}, 2.5f, WHITE);
            DrawTextCustom(ui.mainFont, selGame.player1.name, (int)infoX+30, (int)(pY+40), 40, warmWhite);

            DrawTextCustom(ui.mainFont, "vs", (int)infoX+30, (int)(pY+88), 24, Fade(dimColor, 0.70f));

            // P2, red dot + silver name
            DrawCircleV({infoX+12, pY+132}, 6.0f, redAccent);
            DrawCircleV({infoX+12, pY+132}, 2.5f, WHITE);
            DrawTextCustom(ui.mainFont, selGame.player2.name, (int)infoX+30, (int)(pY+112), 40, silverColor);

            // ── Board preview ──
            float boardX   = prevX + infoW + 50.0f;
            float boardAvW = prevX + prevW - boardX - 10.0f;
            float boardAvH = prevH - 4.0f;
            float boardSize = (boardAvW < boardAvH) ? boardAvW : boardAvH;
            float boardY    = prevY + (prevH - boardSize) * 0.5f;

            // Glow rings, brighter
            for (int g = 4; g >= 1; g--) {
                float go = g * 9.0f;
                DrawRectangleLinesEx({boardX-go, boardY-go, boardSize+go*2, boardSize+go*2},
                    2.0f, Fade(gothicGold, 0.10f * glowPulse * (5-g)));
            }
            DrawTexturePro(ui.boardFrame,
                {0,0,(float)ui.boardFrame.width,(float)ui.boardFrame.height},
                {boardX, boardY, boardSize, boardSize}, {0,0}, 0.0f, WHITE);

            float margin = 0.055f;
            float ix2 = boardX + boardSize*margin;
            float iy2 = boardY + boardSize*margin;
            float iw2 = boardSize*(1.0f - margin*2.0f);
            float ih2 = boardSize*(1.0f - margin*2.0f);
            float cw2 = iw2/BOARD_SIZE, ch2 = ih2/BOARD_SIZE;

            const Texture2D& icon1 = ui.heroIcon[HERO_MAP[selGame.p1HeroSelection]];
            const Texture2D& icon2 = ui.heroIcon[HERO_MAP[selGame.p2HeroSelection]];
            for (int r = 0; r < BOARD_SIZE; r++) {
                for (int c = 0; c < BOARD_SIZE; c++) {
                    Rectangle dest = {ix2+c*cw2, iy2+r*ch2, cw2, ch2};
                    DrawTexturePro(ui.cell, {0,0,(float)ui.cell.width,(float)ui.cell.height}, dest, {0,0}, 0.0f, WHITE);
                    if (selGame.board[r][c].c == 1) {
                        float pad = cw2 * 0.10f;
                        Rectangle src = {0,0,(float)icon1.width,(float)icon1.height};
                        Rectangle dst = {dest.x+pad, dest.y+pad, cw2-pad*2, ch2-pad*2};
                        DrawTexturePro(icon1, src, dst, {0,0}, 0.0f, WHITE);
                    }
                    if (selGame.board[r][c].c == 2) {
                        float pad = cw2 * 0.10f;
                        Rectangle src = {0,0,(float)icon2.width,(float)icon2.height};
                        Rectangle dst = {dest.x+pad, dest.y+pad, cw2-pad*2, ch2-pad*2};
                        DrawTexturePro(icon2, src, dst, {0,0}, 0.0f, WHITE);
                    }
                }
            }

        } else {
            // Empty slot placeholder
            float cx2 = prevX + prevW / 2.0f;
            float cy2 = prevY + prevH / 2.0f;
            const char* emMsg = "EMPTY SLOT";
            int emMsgW = MeasureTextCustomX(ui.mainFont, emMsg, 52);
            DrawTextCustom(ui.mainFont, emMsg, (int)(cx2-emMsgW/2), (int)(cy2-30), 52, Fade(dimColor, 0.30f));
            const char* hint = "No save data found";
            int hintW = MeasureTextCustomX(ui.mainFont, hint, 28);
            DrawTextCustom(ui.mainFont, hint, (int)(cx2-hintW/2), (int)(cy2+36), 28, Fade(dimColor, 0.20f));
        }

        // ── Bottom guide bar ──
        DrawRectangleGradientV(0, 1010, 1920, 70, Fade(BLACK, 0.0f), Fade(BLACK, 0.96f));
        DrawLineEx({80, 1022}, {1840, 1022}, 1.2f, Fade(accentDim, 0.50f));
        const char* guide = "[W] / [S]  navigate      [ENTER]  load      [DEL]  delete      [ESC] / [M]  back";
        int gW2 = MeasureTextCustomX(ui.mainFont, guide, 28);
        DrawTextCustom(ui.mainFont, guide, 1920/2 - gW2/2, 1036, 28, Fade(silverColor, 0.80f));
    }
    else if (ui.currentScreen == 6) {
        // Background
        DrawTexturePro(ui.bgSaveLoad, { 0, 0, (float)ui.bgSaveLoad.width, (float)ui.bgSaveLoad.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.62f));

        // Color palette — gold/cream theme
        Color goldColor    = { 215, 175, 80,  255 };
        Color goldDim      = { 160, 125, 50,  255 };
        Color creamColor   = { 235, 225, 200, 255 };
        Color mutedColor   = { 170, 158, 130, 255 };
        Color panelBg      = { 18,  14,  8,   210 };
        Color panelBorder  = { 215, 175, 80,  180 };
        Color selectedBg   = { 45,  35,  10,  220 };
        Color selectedAcct = { 215, 175, 80,  255 };

        float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 2.5f);

        // ── Main panel ──
        float panelX = 200.0f, panelY = 80.0f;
        float panelW = 1520.0f, panelH = 900.0f;
        DrawRectangleRounded({panelX, panelY, panelW, panelH}, 0.03f, 8, panelBg);
        DrawRectangleRoundedLines({panelX, panelY, panelW, panelH}, 0.03f, 8, Fade(panelBorder, 0.7f));
        // Corner accents
        float ca = 30.0f;
        DrawLineEx({panelX, panelY}, {panelX+ca, panelY}, 2.5f, goldColor);
        DrawLineEx({panelX, panelY}, {panelX, panelY+ca}, 2.5f, goldColor);
        DrawLineEx({panelX+panelW, panelY}, {panelX+panelW-ca, panelY}, 2.5f, goldColor);
        DrawLineEx({panelX+panelW, panelY}, {panelX+panelW, panelY+ca}, 2.5f, goldColor);
        DrawLineEx({panelX, panelY+panelH}, {panelX+ca, panelY+panelH}, 2.5f, goldColor);
        DrawLineEx({panelX, panelY+panelH}, {panelX, panelY+panelH-ca}, 2.5f, goldColor);
        DrawLineEx({panelX+panelW, panelY+panelH}, {panelX+panelW-ca, panelY+panelH}, 2.5f, goldColor);
        DrawLineEx({panelX+panelW, panelY+panelH}, {panelX+panelW, panelY+panelH-ca}, 2.5f, goldColor);

        // ── Title ──
        const char* titleTxt = "SAVE GAME";
        int titleW = MeasureTextCustomX(ui.mainFont, titleTxt, 58);
        DrawTextCustom(ui.mainFont, titleTxt, (int)(panelX + panelW*0.5f - titleW*0.5f + 2), 112, 58, Fade(BLACK, 0.6f));
        DrawTextCustom(ui.mainFont, titleTxt, (int)(panelX + panelW*0.5f - titleW*0.5f), 110, 58, goldColor);
        // Title underline
        DrawLineEx({panelX+60, 180}, {panelX+panelW-60, 180}, 1.5f, Fade(goldDim, 0.5f));

        // ── Input box ──
        float inpY = 200.0f;
        DrawTextCustom(ui.mainFont, "Save Name:", (int)(panelX+60), (int)inpY+8, 28, mutedColor);
        Rectangle inputBox = { panelX+230, inpY, panelW-290, 48 };
        // Box background + border
        DrawRectangleRec(inputBox, {10, 8, 4, 200});
        DrawRectangleRoundedLines({inputBox.x-1, inputBox.y-1, inputBox.width+2, inputBox.height+2}, 0.1f, 4,
            Fade(goldColor, 0.55f + 0.25f*pulse));
        DrawTextCustom(ui.mainFont, ui.nameInput, (int)(inputBox.x+14), (int)(inputBox.y+10), 32, creamColor);
        if (ui.letterCount < 29 && ((int)(GetTime() * 2) % 2) == 0) {
            int textW = MeasureTextCustomX(ui.mainFont, ui.nameInput, 32);
            DrawTextCustom(ui.mainFont, "|", (int)(inputBox.x+14+textW), (int)(inputBox.y+8), 32,
                Fade(goldColor, 0.8f + 0.2f*pulse));
        }

        // ── Slot list ──
        float startX = panelX + 40;
        float startY = 280.0f;
        float slotW  = panelW - 80;
        float slotH  = 120.0f;
        float gap    = 18.0f;

        for (int i = 0; i < 4; i++) {
            float sy = startY + i * (slotH + gap);
            Rectangle slotRec = { startX, sy, slotW, slotH };
            GameState tempGame;
            bool hasData = PeekGameSlot(i, tempGame);
            bool isSel   = (i == ui.saveSelection);

            // Slot background
            DrawRectangleRounded(slotRec, 0.08f, 6, isSel ? selectedBg : Color{25, 20, 10, 160});
            // Border
            Color bdCol = isSel ? Fade(selectedAcct, 0.8f + 0.2f*pulse) : Fade(goldDim, 0.25f);
            DrawRectangleRoundedLines(slotRec, 0.08f, 6, bdCol);
            // Left accent bar khi selected
            if (isSel) {
                DrawRectangleRounded({startX, sy+8, 5, slotH-16}, 0.5f, 4,
                    Fade(goldColor, 0.8f + 0.2f*pulse));
            }

            // Slot number badge
            const char* numTxt = TextFormat("%d", i+1);
            DrawTextCustom(ui.mainFont, numTxt, (int)(startX+22), (int)(sy+slotH*0.5f-18), 30,
                isSel ? Fade(goldColor, 0.9f) : Fade(mutedColor, 0.5f));

            // Content
            if (hasData) {
                Color nameCol   = isSel ? creamColor : Fade(creamColor, 0.75f);
                Color detailCol = isSel ? Fade(mutedColor, 0.9f) : Fade(mutedColor, 0.6f);
                Color timeCol   = isSel ? Fade(goldColor, 0.85f) : Fade(goldDim, 0.6f);

                DrawTextCustom(ui.mainFont, tempGame.saveName, (int)(startX+55), (int)(sy+16), 38, nameCol);
                const char* detail  = TextFormat("Mode: Classic   |   Round: %d", tempGame.roundCount);
                DrawTextCustom(ui.mainFont, detail, (int)(startX+55), (int)(sy+66), 24, detailCol);
                int timeW = MeasureTextCustomX(ui.mainFont, tempGame.saveTime, 24);
                DrawTextCustom(ui.mainFont, tempGame.saveTime, (int)(startX+slotW-timeW-30), (int)(sy+slotH*0.5f-12), 24, timeCol);
            } else {
                Color emptyCol = isSel ? Fade(mutedColor, 0.85f) : Fade(mutedColor, 0.38f);
                DrawTextCustom(ui.mainFont, "— Empty Slot —", (int)(startX+55), (int)(sy+slotH*0.5f-16), 32, emptyCol);
            }
        }

        // ── Footer guide ──
        const char* footer = "[W] / [S]  select slot      [ENTER]  Save      [ESC]  Cancel";
        int footerW = MeasureTextCustomX(ui.mainFont, footer, 26);
        // Footer background pill
        float fy = panelY + panelH - 52.0f;
        DrawRectangleRounded({960.0f - footerW*0.5f - 24, fy, (float)footerW+48, 38}, 0.5f, 8,
            {10, 8, 4, 180});
        DrawRectangleRoundedLines({960.0f - footerW*0.5f - 24, fy, (float)footerW+48, 38}, 0.5f, 8,
            Fade(goldDim, 0.4f));
        DrawTextCustom(ui.mainFont, footer, (int)(960 - footerW*0.5f), (int)(fy+7), 26, mutedColor);
    }
    else if (ui.currentScreen == 7) {
        DrawTexturePro(ui.bgMenu, { 0, 0, (float)ui.bgMenu.width, (float)ui.bgMenu.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.80f));

        // Color palette
        Color goldColor7   = { 215, 175, 80,  255 };
        Color goldDim7     = { 150, 115, 40,  255 };
        Color creamColor7  = { 235, 225, 200, 255 };
        Color mutedColor7  = { 170, 158, 130, 255 };
        Color p1ColorHud   = { 255, 160,  40,  255 };
        Color p2ColorHud   = { 100, 160, 255,  255 };

        float pulse7 = 0.5f + 0.5f * sinf((float)GetTime() * 2.2f);

        // Main panel
        float px7 = 480.0f, py7 = 160.0f, pw7 = 960.0f, ph7 = 730.0f;
        DrawRectangleRounded({px7, py7, pw7, ph7}, 0.04f, 8, {12, 10, 5, 220});
        DrawRectangleRoundedLines({px7, py7, pw7, ph7}, 0.04f, 8, Fade(goldDim7, 0.70f));
        // Corner L-accents
        float ca7 = 32.0f;
        DrawLineEx({px7, py7}, {px7+ca7, py7}, 2.5f, goldColor7);
        DrawLineEx({px7, py7}, {px7, py7+ca7}, 2.5f, goldColor7);
        DrawLineEx({px7+pw7, py7}, {px7+pw7-ca7, py7}, 2.5f, goldColor7);
        DrawLineEx({px7+pw7, py7}, {px7+pw7, py7+ca7}, 2.5f, goldColor7);
        DrawLineEx({px7, py7+ph7}, {px7+ca7, py7+ph7}, 2.5f, goldColor7);
        DrawLineEx({px7, py7+ph7}, {px7, py7+ph7-ca7}, 2.5f, goldColor7);
        DrawLineEx({px7+pw7, py7+ph7}, {px7+pw7-ca7, py7+ph7}, 2.5f, goldColor7);
        DrawLineEx({px7+pw7, py7+ph7}, {px7+pw7, py7+ph7-ca7}, 2.5f, goldColor7);

        // Title
        const char* titleTxt7 = "PLAYER SETUP";
        int titW7 = MeasureTextCustomX(ui.mainFont, titleTxt7, 56);
        DrawTextCustom(ui.mainFont, titleTxt7, (int)(px7 + pw7*0.5f - titW7*0.5f + 2), (int)py7+26, 56, Fade(BLACK, 0.6f));
        DrawTextCustom(ui.mainFont, titleTxt7, (int)(px7 + pw7*0.5f - titW7*0.5f), (int)py7+24, 56, goldColor7);
        DrawLineEx({px7+50, py7+94}, {px7+pw7-50, py7+94}, 1.5f, Fade(goldDim7, 0.45f));

        // Player 1 section
        float secY1 = py7 + 116.0f;
        // Section label
        Color p1Indicator = (ui.activeInputField == 0) ? p1ColorHud : Fade(p1ColorHud, 0.38f);
        // Dot indicator
        DrawCircleV({px7+60, secY1+18}, (ui.activeInputField == 0) ? 8.0f : 5.0f, p1Indicator);
        if (ui.activeInputField == 0) DrawCircleV({px7+60, secY1+18}, 3.5f, WHITE);
        DrawTextCustom(ui.mainFont, "PLAYER 1", (int)(px7+78), (int)secY1+4, 28,
            (ui.activeInputField == 0) ? p1ColorHud : Fade(mutedColor7, 0.70f));

        // P1 Input box
        float inpY1 = secY1 + 42.0f;
        bool p1Active = (ui.activeInputField == 0);
        Color p1BoxBg  = p1Active ? Color{45, 35, 10, 210} : Color{18, 14, 6, 180};
        Color p1BoxBd  = p1Active ? Fade(p1ColorHud, 0.70f + 0.20f*pulse7) : Fade(goldDim7, 0.25f);
        DrawRectangleRounded({px7+50, inpY1, pw7-100, 54}, 0.15f, 6, p1BoxBg);
        DrawRectangleRoundedLines({px7+50, inpY1, pw7-100, 54}, 0.15f, 6, p1BoxBd);
        if (p1Active) {
            // Left accent bar
            DrawRectangleRounded({px7+50, inpY1+4, 4, 46}, 0.5f, 4, Fade(p1ColorHud, 0.80f + 0.20f*pulse7));
            // Top shine
            DrawRectangleGradientV((int)(px7+56), (int)(inpY1+2), (int)(pw7-112), 12,
                Fade(WHITE, 0.06f), Fade(WHITE, 0.0f));
        }
        // Placeholder or text
        if (ui.p1LetterCount == 0 && !p1Active) {
            DrawTextCustom(ui.mainFont, "Enter name...", (int)(px7+66), (int)(inpY1+14), 28, Fade(mutedColor7, 0.30f));
        } else {
            DrawTextCustom(ui.mainFont, ui.p1NameInput, (int)(px7+66), (int)(inpY1+12), 30, creamColor7);
        }
        // Cursor blink
        if (p1Active && ((int)(GetTime()*2)%2)==0) {
            int cxOff = MeasureTextCustomX(ui.mainFont, ui.p1NameInput, 30);
            DrawTextCustom(ui.mainFont, "|", (int)(px7+66+cxOff), (int)(inpY1+10), 32,
                Fade(p1ColorHud, 0.80f + 0.20f*pulse7));
        }

        // Divider between players
        float divY7 = inpY1 + 72.0f;
        DrawLineEx({px7+50, divY7}, {px7+pw7-50, divY7}, 1.0f, Fade(goldDim7, 0.30f));

        // Player 2 section
        float secY2 = divY7 + 28.0f;
        Color p2Indicator = (ui.activeInputField == 1) ? p2ColorHud : Fade(p2ColorHud, 0.38f);
        DrawCircleV({px7+60, secY2+18}, (ui.activeInputField == 1) ? 8.0f : 5.0f, p2Indicator);
        if (ui.activeInputField == 1) DrawCircleV({px7+60, secY2+18}, 3.5f, WHITE);
        DrawTextCustom(ui.mainFont, "PLAYER 2", (int)(px7+78), (int)secY2+4, 28,
            (ui.activeInputField == 1) ? p2ColorHud : Fade(mutedColor7, 0.70f));

        // P2 Input box
        float inpY2 = secY2 + 42.0f;
        bool p2Active = (ui.activeInputField == 1);
        Color p2BoxBg  = p2Active ? Color{10, 25, 50, 210} : Color{18, 14, 6, 180};
        Color p2BoxBd  = p2Active ? Fade(p2ColorHud, 0.70f + 0.20f*pulse7) : Fade(goldDim7, 0.25f);
        DrawRectangleRounded({px7+50, inpY2, pw7-100, 54}, 0.15f, 6, p2BoxBg);
        DrawRectangleRoundedLines({px7+50, inpY2, pw7-100, 54}, 0.15f, 6, p2BoxBd);
        if (p2Active) {
            DrawRectangleRounded({px7+50, inpY2+4, 4, 46}, 0.5f, 4, Fade(p2ColorHud, 0.80f + 0.20f*pulse7));
            DrawRectangleGradientV((int)(px7+56), (int)(inpY2+2), (int)(pw7-112), 12,
                Fade(WHITE, 0.06f), Fade(WHITE, 0.0f));
        }
        if (ui.p2LetterCount == 0 && !p2Active) {
            DrawTextCustom(ui.mainFont, "Enter name...", (int)(px7+66), (int)(inpY2+14), 28, Fade(mutedColor7, 0.30f));
        } else {
            DrawTextCustom(ui.mainFont, ui.p2NameInput, (int)(px7+66), (int)(inpY2+12), 30, creamColor7);
        }
        if (p2Active && ((int)(GetTime()*2)%2)==0) {
            int cxOff2 = MeasureTextCustomX(ui.mainFont, ui.p2NameInput, 30);
            DrawTextCustom(ui.mainFont, "|", (int)(px7+66+cxOff2), (int)(inpY2+10), 32,
                Fade(p2ColorHud, 0.80f + 0.20f*pulse7));
        }

        // Tip: Tab/Up/Down to switch
        float tipY = inpY2 + 72.0f;
        DrawLineEx({px7+50, tipY}, {px7+pw7-50, tipY}, 1.0f, Fade(goldDim7, 0.25f));
        // Key badges row
        float badgeY = tipY + 18.0f;
        // TAB badge
        DrawRectangleRounded({px7+60, badgeY, 56, 28}, 0.3f, 4, {35, 28, 8, 180});
        DrawRectangleRoundedLines({px7+60, badgeY, 56, 28}, 0.3f, 4, Fade(goldDim7, 0.55f));
        DrawTextCustom(ui.mainFont, "TAB", (int)(px7+66), (int)(badgeY+5), 18, Fade(goldColor7, 0.80f));
        DrawTextCustom(ui.mainFont, "/", (int)(px7+122), (int)(badgeY+5), 18, Fade(mutedColor7, 0.50f));
        // UP badge
        DrawRectangleRounded({px7+140, badgeY, 28, 28}, 0.3f, 4, {35, 28, 8, 180});
        DrawRectangleRoundedLines({px7+140, badgeY, 28, 28}, 0.3f, 4, Fade(goldDim7, 0.55f));
        DrawTextCustom(ui.mainFont, "W", (int)(px7+148), (int)(badgeY+5), 18, Fade(goldColor7, 0.80f));
        DrawTextCustom(ui.mainFont, "/ UP", (int)(px7+172), (int)(badgeY+6), 18, Fade(mutedColor7, 0.50f));
        DrawTextCustom(ui.mainFont, "Switch field", (int)(px7+218), (int)(badgeY+6), 18, Fade(mutedColor7, 0.55f));

        // Footer guide bar
        float footY7 = py7 + ph7 - 58.0f;
        DrawLineEx({px7+50, footY7}, {px7+pw7-50, footY7}, 1.0f, Fade(goldDim7, 0.30f));
        // Key hints in footer
        const char* fh1 = "[ENTER] Next / Start";
        const char* fh2 = "[ESC] Back";
        int fh1W = MeasureTextCustomX(ui.mainFont, fh1, 22);
        int fh2W = MeasureTextCustomX(ui.mainFont, fh2, 22);
        // ENTER hint
        DrawRectangleRounded({px7+60, footY7+12, (float)fh1W+24, 30}, 0.4f, 4, {25, 20, 6, 180});
        DrawRectangleRoundedLines({px7+60, footY7+12, (float)fh1W+24, 30}, 0.4f, 4, Fade(goldDim7, 0.45f));
        DrawTextCustom(ui.mainFont, fh1, (int)(px7+72), (int)(footY7+18), 22, Fade(goldColor7, 0.75f));
        // ESC hint
        float esc7X = px7 + pw7 - 60 - fh2W - 24;
        DrawRectangleRounded({esc7X, footY7+12, (float)fh2W+24, 30}, 0.4f, 4, {25, 20, 6, 180});
        DrawRectangleRoundedLines({esc7X, footY7+12, (float)fh2W+24, 30}, 0.4f, 4, Fade(goldDim7, 0.35f));
        DrawTextCustom(ui.mainFont, fh2, (int)(esc7X+12), (int)(footY7+18), 22, Fade(mutedColor7, 0.65f));
    }
    else if (ui.currentScreen == 8) {
        // Background
        DrawTexturePro(ui.bgSelect, {0,0,(float)ui.bgSelect.width,(float)ui.bgSelect.height},
            {0,0,1920.0f,1080.0f}, {0,0}, 0.0f, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.42f));
        // Vignette
        DrawRectangleGradientH(0, 0, 300, 1080, Fade(BLACK, 0.60f), Fade(BLACK, 0.0f));
        DrawRectangleGradientH(1620, 0, 300, 1080, Fade(BLACK, 0.0f), Fade(BLACK, 0.60f));
        DrawRectangleGradientV(0, 0, 1920, 130, Fade(BLACK, 0.55f), Fade(BLACK, 0.0f));
        DrawRectangleGradientV(0, 920, 1920, 160, Fade(BLACK, 0.0f), Fade(BLACK, 0.85f));

        float t8 = (float)GetTime();
        float gp8 = 0.5f + 0.5f * sinf(t8 * 2.2f);
        float gp8b = 0.5f + 0.5f * sinf(t8 * 1.3f + 1.0f);

        Color gothicGold  = {255, 180,   0, 255};
        Color accentDim   = {160, 110,   0, 255};
        Color redAccent   = {200,  60,  30, 255};
        Color silverColor = {220, 210, 195, 255};
        Color dimColor    = {150, 130, 100, 255};
        Color warmWhite   = {255, 245, 220, 255};
        Color p1Color     = {255, 160,  40, 255};
        Color p2Color     = {100, 160, 255, 255};

        // ── Hero names (3 active heroes, black_knight hidden) ──
        const char* heroNames[5] = {"Fire Knight", "Green Archer", "Earth Assassin", "Metal Blade", "Water Mage"};
        // Maps selection index (0,1,2,3,4) → asset index (1,2,3,4,5)
        const int heroMap[5] = {1, 2, 3, 4, 5};

        // ── Ornate helpers ──
        auto DD8 = [](float cx, float cy, float w, float h, Color col) {
            DrawTriangle({cx-w/2,cy},{cx,cy+h/2},{cx+w/2,cy},col);
            DrawTriangle({cx-w/2,cy},{cx+w/2,cy},{cx,cy-h/2},col);
        };
        auto DC8 = [](float cx, float cy, float sz, Color col) {
            DrawRectangle((int)(cx-sz/6),(int)(cy-sz/2),(int)(sz/3),(int)sz,col);
            DrawRectangle((int)(cx-sz/2),(int)(cy-sz/6),(int)sz,(int)(sz/3),col);
        };
        auto DrawOrnateLine8 = [&](float x, float y, float width, Color c) {
            DrawLineEx({x+40,y},{x+width-40,y},4.0f,c);
            DrawLineEx({x+40,y-5},{x+width-40,y-5},1.0f,Fade(c,0.5f));
            DrawLineEx({x+40,y+5},{x+width-40,y+5},1.0f,Fade(c,0.5f));
            float cx=x+width/2;
            DD8(cx,y,40,40,c); DD8(cx,y,28,28,BLACK); DD8(cx,y,16,16,c); DC8(cx,y,12,BLACK);
            for(int s=1;s<12;s++){float px=x+40+s*((width-80)/12);if(fabsf(px-cx)>50){DD8(px,y,10,10,c);DrawCircle((int)px,(int)y,2,BLACK);}}
            DD8(x+20,y,28,28,c);DD8(x+20,y,16,16,BLACK);DC8(x+20,y,10,c);DD8(x,y,14,14,c);
            DD8(x+width-20,y,28,28,c);DD8(x+width-20,y,16,16,BLACK);DC8(x+width-20,y,10,c);DD8(x+width,y,14,14,c);
        };

        // ── Top ornament line ──
        DrawOrnateLine8(80, 88, 1760, gothicGold);

        // ── Title ──
        const char* title8 = "CHARACTER SELECT";
        int titW8 = MeasureTextCustomX(ui.mainFont, title8, 68);
        DrawTextCustom(ui.mainFont, title8, 1920/2-titW8/2+3, 106, 68, Fade(BLACK,0.9f));
        DrawTextCustom(ui.mainFont, title8, 1920/2-titW8/2,   102, 68, gothicGold);

        // Game Mode badge below the title
        {
            const char* modeLabel = game.isBotVsBot ? "BOT  VS  BOT"
                                  : (game.isVsBot   ? "VS  BOT"
                                                    : "VS  PLAYER");
            Color modeTextCol = game.isBotVsBot ? Color{180, 100, 255, 255}
                              : (game.isVsBot    ? Color{220,  60,  30, 255}
                                                 : Color{ 40, 185, 120, 255});
            // Small pill border
            int mLblW = MeasureTextCustomX(ui.mainFont, modeLabel, 32);
            float pillX = 1920/2 - mLblW/2 - 18;
            float pillY = 180.0f;
            DrawRectangleRounded({pillX, pillY, (float)mLblW+36, 40}, 0.5f, 6,
                Fade(BLACK, 0.55f));
            DrawRectangleRoundedLines({pillX, pillY, (float)mLblW+36, 40}, 0.5f, 6,
                Fade(modeTextCol, 0.65f + 0.25f*gp8));
            // Glow text
            DrawTextCustom(ui.mainFont, modeLabel, (int)(pillX+18)+1, (int)pillY+6+1, 32, Fade(BLACK,0.8f));
            DrawTextCustom(ui.mainFont, modeLabel, (int)(pillX+18),   (int)pillY+6,   32, modeTextCol);
        }

        // 5 hero card slots, evenly spaced horizontally
        const float cardW   = 320.0f;
        const float cardH   = 510.0f;
        const float cardGap = 24.0f;
        const float totalCW = 5*cardW + 4*cardGap;
        const float cardStartX = (1920.0f - totalCW) / 2.0f;
        const float cardY   = 310.0f;

        // Determine which card is hovered (based on selectionPhase)
        int curSel = (ui.selectionPhase <= 1) ? ui.p1HeroSelection : ui.p2HeroSelection;
        bool isP1turn = (ui.selectionPhase == 0 || ui.selectionPhase == 1);

        // Per-hero color theme (index 0=fire, 1=archer, 2=assassin, 3=metal_blade, 4=water_mage)
        Color heroTheme[5] = {
            {220,  70,  20, 255},  // fire_knight    — red-orange
            { 40, 170, 110, 255},  // green_archer   — teal-green
            {180, 160,  60, 255},  // earth_assassin - gold (former metal_blade)
            {180, 185, 195, 255},  // metal_blade    - steel silver/grey
            { 30, 160, 220, 255},  // water_mage     — sky blue
        };

        // Hero bottom-pixel anchors — all 5 heroes use 126px
        struct HeroBot { float bottomPx; } bots[5] = {
            {126.0f}, {126.0f}, {126.0f}, {126.0f}, {126.0f},
        };

        for (int i = 0; i < 5; i++) {
            int assetID = heroMap[i]; // translate to asset index
            float cx = cardStartX + i*(cardW+cardGap);
            bool isSelected = (curSel == i);
            bool p1owns = (ui.selectionPhase >= 1 && ui.p1HeroSelection == i);
            bool p2owns = (ui.selectionPhase >= 3 && ui.p2HeroSelection == i);

            // Selected card lifts up
            float cardOffY = isSelected ? -16.0f : 0.0f;
            float cy = cardY + cardOffY;

            Color hTheme = heroTheme[i];

            // ── Card background: per-hero gradient ──
            DrawRectangleRec({cx, cy, cardW, cardH}, Fade(BLACK, 0.80f));
            // Hero color seeps in from top
            DrawRectangleGradientV((int)cx, (int)cy, (int)cardW, (int)(cardH*0.55f),
                Fade(hTheme, isSelected ? 0.32f : 0.14f), Fade(hTheme, 0.0f));
            // Dark vignette at bottom
            DrawRectangleGradientV((int)cx, (int)(cy+cardH*0.45f), (int)cardW, (int)(cardH*0.55f),
                Fade(BLACK, 0.0f), Fade(BLACK, 0.82f));

            // Ambient glow at center-bottom when selected
            float footY = cy + cardH * 0.88f;
            if (isSelected) {
                for (int g = 5; g >= 1; g--) {
                    float gr = g * 28.0f;
                    float ga = 0.10f * gp8 * (6-g) / 5.0f;
                    DrawCircle((int)(cx+cardW/2), (int)footY, gr, Fade(hTheme, ga));
                }
            } else {
                // subtle ambient even when unselected
                DrawCircle((int)(cx+cardW/2), (int)footY, 40.0f, Fade(hTheme, 0.06f));
            }

            // Selected: player color top overlay + outer glow pulse
            if (isSelected) {
                Color selCol = isP1turn ? p1Color : p2Color;
                DrawRectangleGradientV((int)cx, (int)cy, (int)cardW, (int)(cardH*0.3f),
                    Fade(selCol, 0.12f), Fade(selCol, 0.0f));
                // Shimmer sweep
                float shimX = cx + fmodf(t8 * 160.0f, cardW + 80.0f) - 40.0f;
                DrawRectangleGradientH((int)shimX, (int)cy, 60, (int)cardH,
                    Fade(WHITE, 0.0f), Fade(WHITE, 0.07f * gp8b));
                DrawRectangleGradientH((int)(shimX+60), (int)cy, 60, (int)cardH,
                    Fade(WHITE, 0.07f * gp8b), Fade(WHITE, 0.0f));
                // Outer glow rings
                for (int g = 4; g >= 1; g--) {
                    float go = g * 5.0f;
                    DrawRectangleLinesEx({cx-go, cy-go, cardW+go*2, cardH+go*2},
                        1.5f, Fade(selCol, 0.16f * gp8 * (5-g) / 4.0f));
                }
            }

            // ── Ornate diamond corners ──
            float crnSz = 10.0f;
            Color crnCol = isSelected ? Fade(hTheme, 0.90f) : Fade(hTheme, 0.45f);
            // top-left
            DD8(cx,       cy,       crnSz, crnSz, crnCol);
            DD8(cx+crnSz, cy,       crnSz, crnSz, crnCol);
            DD8(cx,       cy+crnSz, crnSz, crnSz, crnCol);
            // top-right
            DD8(cx+cardW,       cy,       crnSz, crnSz, crnCol);
            DD8(cx+cardW-crnSz, cy,       crnSz, crnSz, crnCol);
            DD8(cx+cardW,       cy+crnSz, crnSz, crnSz, crnCol);
            // bottom-left
            DD8(cx,       cy+cardH,       crnSz, crnSz, crnCol);
            DD8(cx+crnSz, cy+cardH,       crnSz, crnSz, crnCol);
            DD8(cx,       cy+cardH-crnSz, crnSz, crnSz, crnCol);
            // bottom-right
            DD8(cx+cardW,       cy+cardH,       crnSz, crnSz, crnCol);
            DD8(cx+cardW-crnSz, cy+cardH,       crnSz, crnSz, crnCol);
            DD8(cx+cardW,       cy+cardH-crnSz, crnSz, crnSz, crnCol);

            // ── Hero sprite ──
            const CharAnim& anim = ui.heroIdle[assetID];
            float botPx   = bots[i].bottomPx;
            float heroScaleMult = 1.3f; // all 3 remaining heroes use same scale
            float scale8  = (cardW * 3.0f) / (float)anim.frameWidth * heroScaleMult;
            float heroW   = anim.frameWidth  * scale8;
            float heroH   = anim.frameHeight * scale8;
            float footOff = botPx * scale8;
            float hy      = footY - footOff;
            float hx      = cx + cardW/2 - heroW/2;

            Rectangle src8 = {(float)anim.currentFrame*anim.frameWidth, 0,
                (float)anim.frameWidth, (float)anim.frameHeight};
            Rectangle dst8 = {hx, hy, heroW, heroH};

            // Dim non-selected heroes
            Color heroTint = isSelected ? WHITE : Fade(WHITE, 0.50f);
            DrawTexturePro(anim.spriteSheet, src8, dst8, {0,0}, 0.0f, heroTint);

            // P1/P2 lock badge
            if (p1owns) {
                DrawRectangleRounded({cx+8, cy+8, 56, 28}, 0.4f, 4, Fade(p1Color, 0.85f));
                DrawTextCustom(ui.mainFont, "P1", (int)cx+18, (int)cy+12, 22, BLACK);
            }
            if (p2owns) {
                DrawRectangleRounded({cx+8, cy+(p1owns?42:8), 56, 28}, 0.4f, 4, Fade(p2Color, 0.85f));
                DrawTextCustom(ui.mainFont, "P2", (int)cx+18, (int)cy+(p1owns?46:12), 22, BLACK);
            }

            // ── Card border ──
            Color borderCol;
            float borderW8;
            if (isSelected) {
                borderCol = isP1turn ? p1Color : p2Color;
                borderW8  = 3.0f;
            } else if (p1owns || p2owns) {
                borderCol = p1owns ? Fade(p1Color,0.60f) : Fade(p2Color,0.60f);
                borderW8  = 2.0f;
            } else {
                borderCol = Fade(hTheme, 0.40f);
                borderW8  = 1.5f;
            }
            DrawRectangleLinesEx({cx, cy, cardW, cardH}, borderW8, borderCol);
            // Inner thin accent line
            DrawRectangleLinesEx({cx+4, cy+4, cardW-8, cardH-8}, 0.8f, Fade(hTheme, isSelected ? 0.35f : 0.15f));

            // ── Hero name below card ──
            const char* hname = heroNames[i];
            int hnW = MeasureTextCustomX(ui.mainFont, hname, 30);
            float nameY = cy + cardH + 10.0f;
            DrawTextCustom(ui.mainFont, hname, (int)(cx+cardW/2-hnW/2)+2, (int)nameY+2, 30, Fade(BLACK,0.9f));
            Color nameCol = isSelected ? (isP1turn ? p1Color : p2Color) : Fade(hTheme, 0.75f);
            DrawTextCustom(ui.mainFont, hname, (int)(cx+cardW/2-hnW/2), (int)nameY, 30, nameCol);

            // Ornate line + diamond under name when selected
            if (isSelected) {
                float lx = cx+cardW/2-60;
                DrawLineEx({lx, nameY+36}, {lx+120, nameY+36}, 1.5f, Fade(nameCol, 0.6f));
                DD8(lx+60, nameY+36, 10, 10, nameCol);
            }
        }

        // ── Draw char select particles ──
        {
            Color heroThemeParts[5] = {
                {255, 120,  30, 255},  // fire_knight    — orange-red
                { 60, 220, 130, 255},  // green_archer   — bright teal
                {220, 200,  60, 255},  // earth_assassin - gold shimmer (former metal_blade)
                {210, 220, 230, 255},  // metal_blade    - bright silver
                { 40, 200, 255, 255},  // water_mage     — aqua
            };
            for (int k = 0; k < UIState::MAX_CHAR_PARTICLES; k++) {
                const UIState::CharParticle& p = ui.charParticles[k];
                if (p.life <= 0.0f || p.alpha <= 0.0f) continue;
                Color col = Fade(heroThemeParts[p.heroID], p.alpha);
                if (p.type == 0) {
                    // Rising ember — small glowing dot
                    DrawCircle((int)p.x, (int)p.y, p.size, col);
                    // Glow halo
                    DrawCircle((int)p.x, (int)p.y, p.size * 2.2f, Fade(col, 0.25f));
                } else {
                    // Rune diamond — rotated diamond shape
                    float s = p.size;
                    // Draw diamond using 2 triangles (no rotation, just diamond shape)
                    DrawTriangle(
                        {p.x,     p.y - s},
                        {p.x + s, p.y    },
                        {p.x,     p.y + s},
                        col);
                    DrawTriangle(
                        {p.x,     p.y - s},
                        {p.x,     p.y + s},
                        {p.x - s, p.y    },
                        col);
                    // Inner cross
                    DrawLineEx({p.x - s*0.5f, p.y}, {p.x + s*0.5f, p.y}, 1.0f, Fade(WHITE, p.alpha * 0.6f));
                    DrawLineEx({p.x, p.y - s*0.5f}, {p.x, p.y + s*0.5f}, 1.0f, Fade(WHITE, p.alpha * 0.6f));
                }
            }
        }

        // ── Turn indicator bar ──
        if (ui.selectionPhase == 0 || ui.selectionPhase == 2) {
            Color turnCol = (ui.selectionPhase == 0) ? p1Color : p2Color;
            const char* pLabel = (ui.selectionPhase == 0) ? "PLAYER 1" : "PLAYER 2";
            const char* turnTxt = "  —  Choose your champion";
            int plW  = MeasureTextCustomX(ui.mainFont, pLabel, 38);
            int ttW  = MeasureTextCustomX(ui.mainFont, turnTxt, 32);
            int totW = plW + ttW;
            int barY = 880;
            DrawTextCustom(ui.mainFont, pLabel,   1920/2-totW/2,      barY, 38, turnCol);
            DrawTextCustom(ui.mainFont, turnTxt,  1920/2-totW/2+plW,  barY+4, 32, Fade(warmWhite,0.65f));
            // Pulse underline
            DrawLineEx({(float)(1920/2-totW/2), (float)(barY+44)},
                       {(float)(1920/2+totW/2), (float)(barY+44)},
                       1.5f, Fade(turnCol, 0.45f*gp8));
        }

        // ── P1/P2 chosen display — bottom corners ──
        if (ui.selectionPhase >= 1) {
            // P1 bottom left
            const char* p1n = game.player1.name[0] ? game.player1.name : "PLAYER 1";
            DrawCircleV({90, 960}, 7.0f, p1Color);
            DrawCircleV({90, 960}, 3.0f, WHITE);
            DrawTextCustom(ui.mainFont, p1n, 108, 942, 32, Fade(warmWhite, 0.90f));
            const char* p1hero = heroNames[ui.p1HeroSelection];
            DrawTextCustom(ui.mainFont, p1hero, 108, 980, 22, Fade(p1Color, 0.80f));
        }
        if (ui.selectionPhase >= 3) {
            // P2 bottom right
            const char* p2n = game.player2.name[0] ? game.player2.name : "PLAYER 2";
            const char* p2hero = heroNames[ui.p2HeroSelection];
            int p2nW = MeasureTextCustomX(ui.mainFont, p2n, 32);
            int p2hW = MeasureTextCustomX(ui.mainFont, p2hero, 22);
            DrawCircleV({1830, 960}, 7.0f, p2Color);
            DrawCircleV({1830, 960}, 3.0f, WHITE);
            DrawTextCustom(ui.mainFont, p2n,    (int)(1830-p2nW-18), 942, 32, Fade(warmWhite,0.90f));
            DrawTextCustom(ui.mainFont, p2hero, (int)(1830-p2hW-18), 980, 22, Fade(p2Color,0.80f));
        }

        // ── Bottom ornate line ──
        DrawOrnateLine8(80, 1000, 1760, Fade(gothicGold, 0.60f));

        // ── Guide hint ──
        const char* guide8 = "[A] / [D]  navigate      [ENTER]  select      [ESC]  back";
        int gW8 = MeasureTextCustomX(ui.mainFont, guide8, 28);
        DrawTextCustom(ui.mainFont, guide8, 1920/2-gW8/2, 1036, 28, Fade(silverColor, 0.80f));

        // ── Overlay: Game mode selection ──
        if (ui.selectionPhase == -1) {
            DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.65f));

            const char* modeTxt = game.isBotVsBot ? "BOT VS BOT" : (game.isVsBot ? "VS  BOT" : "VS  PLAYER");
            Color p2Color = { 40, 170, 110, 255 }; // from previous use
            Color modeCol = game.isBotVsBot ? PURPLE : (game.isVsBot ? redAccent : p2Color);

            int boxW=920, boxH=270;
            Rectangle box={(float)(1920/2-boxW/2),(float)(1080/2-boxH/2),(float)boxW,(float)boxH};
            DrawRectangleRec(box, Fade(BLACK, 0.90f));
            DrawRectangleLinesEx(box, 2.5f, Fade(gothicGold, 0.85f));
            DrawRectangleLinesEx({box.x-5,box.y-5,box.width+10,box.height+10}, 1.0f,
                Fade(gothicGold, 0.25f*gp8));

            const char* label8 = "SELECT GAME MODE";
            int labW = MeasureTextCustomX(ui.mainFont, label8, 28);
            DrawTextCustom(ui.mainFont, label8, (int)(box.x+boxW/2-labW/2), (int)box.y+28, 28,
                Fade(dimColor, 0.90f));
            DrawLineEx({box.x+40, box.y+68}, {box.x+boxW-40, box.y+68}, 1.5f, Fade(gothicGold,0.40f));

            const char* arrow = "<  ";
            const char* arrowR = "  >";
            int modeW2 = MeasureTextCustomX(ui.mainFont, modeTxt, 58);
            int arrowLW = MeasureTextCustomX(ui.mainFont, arrow, 58);
            int arrowRW = MeasureTextCustomX(ui.mainFont, arrowR, 58);
            int totalMW = arrowLW + modeW2 + arrowRW;
            int modeX = (int)(box.x+boxW/2-totalMW/2);
            int modeY = (int)(box.y+102);
            DrawTextCustom(ui.mainFont, arrow,   modeX,               modeY, 58, Fade(gothicGold, 0.70f));
            DrawTextCustom(ui.mainFont, modeTxt, modeX+arrowLW,       modeY, 58, modeCol);
            DrawTextCustom(ui.mainFont, arrowR,  modeX+arrowLW+modeW2,modeY, 58, Fade(gothicGold, 0.70f));

            const char* hint8 = "[A] / [D]  switch      [ENTER]  confirm      [ESC]  back";
            int hintW8 = MeasureTextCustomX(ui.mainFont, hint8, 24);
            DrawTextCustom(ui.mainFont, hint8, (int)(box.x+boxW/2-hintW8/2), (int)box.y+204, 24,
                Fade(silverColor, 0.55f));
        }
        // ── Overlay: Name input ──
        else if (ui.selectionPhase == 1 || ui.selectionPhase == 3) {
            DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.65f));

            bool isP1 = (ui.selectionPhase == 1);
            Color pCol = isP1 ? p1Color : p2Color;
            int heroID = isP1 ? ui.p1HeroSelection : ui.p2HeroSelection;
            Color hTheme = heroTheme[heroID];

            const char* prompt8 = isP1 ? "PLAYER 1  —  Enter your name" : "PLAYER 2  —  Enter your name";
            const char* input8  = isP1 ? ui.p1NameInput : ui.p2NameInput;

            int boxW=900, boxH=260;
            Rectangle box={(float)(1920/2-boxW/2),(float)(1080/2-boxH/2),(float)boxW,(float)boxH};
            DrawRectangleRec(box, Fade(BLACK, 0.90f));

            // Outer border in the hero color
            DrawRectangleLinesEx(box, 2.5f, Fade(hTheme, 0.90f));
            // Faint blinking outer glow
            DrawRectangleLinesEx({box.x-5,box.y-5,box.width+10,box.height+10}, 1.0f,
                Fade(hTheme, 0.30f * gp8));

            // Top accent bar in the hero color
            DrawRectangleGradientH((int)box.x, (int)box.y, (int)boxW, 5,
                Fade(hTheme, 0.0f), Fade(hTheme, 0.95f));

            int prmW = MeasureTextCustomX(ui.mainFont, prompt8, 34);
            DrawTextCustom(ui.mainFont, prompt8, (int)(box.x+boxW/2-prmW/2), (int)box.y+24, 34, pCol);
            DrawLineEx({box.x+40, box.y+72}, {box.x+boxW-40, box.y+72}, 1.0f, Fade(hTheme, 0.40f));

            DrawTextCustom(ui.mainFont, input8, (int)box.x+52, (int)box.y+100, 62, warmWhite);
            if (((int)(GetTime()*2)%2)==0) {
                int curX = (int)box.x+52+MeasureTextCustomX(ui.mainFont, input8, 62);
                DrawTextCustom(ui.mainFont, "|", curX, (int)box.y+100, 62, Fade(hTheme, 0.95f));
            }

            // Selected hero reminder
            const char* selHeroName = heroNames[heroID];
            int shrW = MeasureTextCustomX(ui.mainFont, selHeroName, 26);
            DrawTextCustom(ui.mainFont, selHeroName, (int)(box.x+boxW/2-shrW/2), (int)box.y+180, 26,
                Fade(hTheme, 0.80f));

            const char* hint8b = "[ENTER] confirm    [ESC] back";
            int hw = MeasureTextCustomX(ui.mainFont, hint8b, 26);
            DrawTextCustom(ui.mainFont, hint8b, (int)(box.x+boxW/2-hw/2), (int)box.y+216, 26,
                Fade(silverColor, 0.70f));
        }
    }
}