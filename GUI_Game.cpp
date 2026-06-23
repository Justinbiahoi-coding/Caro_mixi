#include "GUI_Game.h"
#include "Library.h"

// In-game pause / settings overlay (gear button at the top-right of the board)
static void DrawGearIcon(float cx, float cy, float radius, Color color);
static void DrawPauseOverlay(const GameState& game, const UIState& ui);
static void UpdatePauseOverlay(GameState& game, UIState& ui, Vector2 mouse);

// Check whether the move at (row,col) blocks the opponent (oppPlayer).
// Returns true if:
//   - It blocks one end of 3 opponent marks in a row (the other end already blocked by edge/our mark)
//   - It blocks both ends of 4 opponent marks in a row (board edge counts as a block)
static bool CheckBlockVariant(GameState& game, int row, int col, int oppPlayer) {
    const int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};
    for (int d = 0; d < 4; d++) {
        int dx = dirs[d][0], dy = dirs[d][1];
        int count = 0, blocks = 0;
        GetLineStatus(game, row, col, dx, dy, oppPlayer, count, blocks);
        // count = number of consecutive opponent marks (excluding the move just made)
        // blocks = number of blocked ends (edge + non-opponent marks)
        // Our new mark sits at one end, so it contributes one block
        // GetLineStatus counts a block at an empty end; placing there turns that end into a block
        // GetLineStatus does not know about the new (row,col), so we account for it here:
        // After MakeMove, board[row][col] is already our mark, so GetLineStatus sees
        // our mark blocking one side and the edge/other mark blocking the other.
        // Conditions: 3 in a row + 1 blocked end (blocks >= 1, count == 3)
        //             4 in a row + both ends blocked (blocks == 2, count == 4)
        if (count == 3) return true;  // block one end of 3, regardless of the other end
        if (count == 4 && blocks == 2) return true;  // block both ends of 4 (edge counts as block)
    }
    return false;
}

// Draw a gothic gear/cog icon centered at (cx,cy). Teeth are rotated rectangles
// (always render regardless of vertex winding), with a solid body and a dark hole.
static void DrawGearIcon(float cx, float cy, float radius, Color color) {
    const int teeth = 8;
    float bodyR    = radius * 0.66f;
    float toothW   = radius * 0.42f;
    float toothLen = radius * 0.46f;
    float dInner   = bodyR * 0.80f; // teeth start slightly inside the body
    for (int i = 0; i < teeth; i++) {
        float deg = (float)i * (360.0f / teeth);
        Rectangle r = { cx, cy, toothW, toothLen };
        Vector2 origin = { toothW * 0.5f, dInner + toothLen };
        DrawRectanglePro(r, origin, deg, color);
    }
    DrawCircle((int)cx, (int)cy, bodyR, color);
    DrawCircle((int)cx, (int)cy, radius * 0.30f, (Color){ 14, 10, 7, 255 });
}

// Handle input for the in-game pause overlay (mouse driven). Geometry MUST stay
// in sync with DrawPauseOverlay below.
static void UpdatePauseOverlay(GameState& game, UIState& ui, Vector2 mouse) {
    (void)game;
    const int panelW = 620, panelH = 520;
    const int panelX = (1920 - panelW) / 2;
    const int panelY = (1080 - panelH) / 2;

    const int barWidth = 360, barHeight = 22;
    const int barX = panelX + (panelW - barWidth) / 2 - 30;
    const int musicBarY = panelY + 150;
    const int sfxBarY   = panelY + 240;

    if (VolumeSliderUpdate(ui.musicVolume, ui.draggingVolume, mouse, barX, musicBarY, barWidth, barHeight))
        SetMusicVolume(ui.bgMusic, ui.musicVolume);
    if (VolumeSliderUpdate(ui.sfxVolume, ui.draggingSFX, mouse, barX, sfxBarY, barWidth, barHeight))
        ApplySFXVolume(ui);

    bool dragging = ui.draggingVolume || ui.draggingSFX;

    Rectangle btnToggle = { (float)(panelX + (panelW - 360) / 2), (float)(panelY + 310), 360, 50 };
    Rectangle btnResume = { (float)(panelX + 40), (float)(panelY + panelH - 90), 250, 56 };
    Rectangle btnMenu   = { (float)(panelX + panelW - 290), (float)(panelY + panelH - 90), 250, 56 };

    if (!dragging && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, btnToggle)) {
            ui.musicEnabled = !ui.musicEnabled;
            if (ui.musicEnabled) PlayMusicStream(ui.bgMusic);
            else PauseMusicStream(ui.bgMusic);
        } else if (CheckCollisionPointRec(mouse, btnResume)) {
            ui.inGamePaused = false;
        } else if (CheckCollisionPointRec(mouse, btnMenu)) {
            ui.inGamePaused = false;
            ui.currentScreen = 0;
        }
    }
}

// Draw the dim backdrop + settings panel for the in-game pause overlay.
static void DrawPauseOverlay(const GameState& game, const UIState& ui) {
    (void)game;
    Vector2 mouse = GetMousePosition();
    Color gold  = { 255, 180, 0, 255 };
    Color stone = { 14, 10, 7, 255 };

    DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.62f));

    const int panelW = 620, panelH = 520;
    const int panelX = (1920 - panelW) / 2;
    const int panelY = (1080 - panelH) / 2;
    Rectangle panel = { (float)panelX, (float)panelY, (float)panelW, (float)panelH };

    DrawRectangleRounded(panel, 0.04f, 8, Fade(stone, 0.96f));
    DrawRectangleRoundedLines(panel, 0.04f, 8, Fade(gold, 0.85f));
    DrawRectangleRoundedLines({ panel.x + 5, panel.y + 5, panel.width - 10, panel.height - 10 },
                              0.04f, 8, Fade(gold, 0.25f));

    const char* title = "PAUSED";
    int tw = MeasureTextCustomX(ui.mainFont, title, 64);
    DrawTextCustom(ui.mainFont, title, panelX + (panelW - tw) / 2 + 3, panelY + 33, 64, Fade(BLACK, 0.8f));
    DrawTextCustom(ui.mainFont, title, panelX + (panelW - tw) / 2,     panelY + 30, 64, gold);

    const int barWidth = 360, barHeight = 22;
    const int barX = panelX + (panelW - barWidth) / 2 - 30;
    const int musicBarY = panelY + 150;
    const int sfxBarY   = panelY + 240;

    DrawTextCustom(ui.mainFont, "Music Volume", barX, musicBarY - 38, 30, LIGHTGRAY);
    DrawTextCustom(ui.mainFont, "SFX Volume",   barX, sfxBarY - 38, 30, LIGHTGRAY);
    VolumeSliderDraw(ui.mainFont, barX, musicBarY, barWidth, barHeight, ui.musicVolume, ui.draggingVolume, gold);
    VolumeSliderDraw(ui.mainFont, barX, sfxBarY,   barWidth, barHeight, ui.sfxVolume,   ui.draggingSFX,   gold);

    auto drawBtn = [&](Rectangle b, const char* label, bool hov) {
        DrawRectangleRounded(b, 0.3f, 6, hov ? Color{ 120, 70, 10, 240 } : Color{ 10, 10, 10, 205 });
        DrawRectangleRoundedLines(b, 0.3f, 6, Fade(gold, hov ? 0.9f : 0.4f));
        int lw = MeasureTextCustomX(ui.mainFont, label, 30);
        DrawTextCustom(ui.mainFont, label, (int)(b.x + (b.width - lw) / 2),
                       (int)(b.y + (b.height - 30) / 2), 30, hov ? WHITE : Fade(WHITE, 0.7f));
    };

    Rectangle btnToggle = { (float)(panelX + (panelW - 360) / 2), (float)(panelY + 310), 360, 50 };
    Rectangle btnResume = { (float)(panelX + 40), (float)(panelY + panelH - 90), 250, 56 };
    Rectangle btnMenu   = { (float)(panelX + panelW - 290), (float)(panelY + panelH - 90), 250, 56 };

    char tg[64];
    snprintf(tg, sizeof(tg), "Music: %s", ui.musicEnabled ? "ON" : "OFF");
    drawBtn(btnToggle, tg, CheckCollisionPointRec(mouse, btnToggle));
    drawBtn(btnResume, "Resume",    CheckCollisionPointRec(mouse, btnResume));
    drawBtn(btnMenu,   "Main Menu", CheckCollisionPointRec(mouse, btnMenu));

    const char* hint = "[ESC] Resume   -   click the gear to toggle";
    int hw = MeasureTextCustomX(ui.mainFont, hint, 20);
    DrawTextCustom(ui.mainFont, hint, panelX + (panelW - hw) / 2, panelY + panelH - 26, 20, Fade(WHITE, 0.4f));
}

void UpdateGUIGame(GameState& game, UIState& ui) {
    float dt = GetFrameTime();
    Vector2 mouse = GetMousePosition();

    int p1Asset = HERO_MAP[ui.p1HeroSelection];
    int p2Asset = HERO_MAP[ui.p2HeroSelection];

    // --- IN-GAME PAUSE / SETTINGS OVERLAY ---------------------------------
    // The gear button is only available during active play (not while a win
    // animation or the end-game panel is on screen).
    bool canPause = (game.matchStatus == 0 && !ui.pendingWin &&
                     !ui.isP1Dying && !ui.isP2Dying);
    if (!canPause) {
        ui.inGamePaused = false; // never linger in pause once the round resolves
    } else {
        Vector2 gearCenter = { 1850.0f, 56.0f };
        bool gearClicked = CheckCollisionPointCircle(mouse, gearCenter, 34.0f) &&
                           IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        if (gearClicked || IsKeyPressed(KEY_ESCAPE)) {
            ui.inGamePaused = !ui.inGamePaused;
        }
    }
    if (ui.inGamePaused) {
        UpdatePauseOverlay(game, ui, mouse);
        return; // freeze the match while the overlay is open
    }

    // Helper: get active attack anim by variant
    auto getAtk = [&](int asset, int variant) -> CharAnim& {
        if (variant == 1) return ui.heroAttack2[asset];
        if (variant == 2) return ui.heroAttack3[asset];
        return ui.heroAttack[asset];
    };

    auto triggerDeath = [&]() {
        if (!ui.pendingWin) return;
        // pendingWinStatus: 1 = P1 wins (P2 loses), 2 = P2 wins (P1 loses), 3 = Draw
        if (ui.pendingWinStatus == 1) {
            if (!ui.isP2Dying) {
                ui.isP2Dying = true;
                ui.heroDeath[p2Asset].currentFrame = 0;
                ui.heroDeath[p2Asset].frameTimer = 0.0f;
            }
        } else if (ui.pendingWinStatus == 2) {
            if (!ui.isP1Dying) {
                ui.isP1Dying = true;
                ui.heroDeath[p1Asset].currentFrame = 0;
                ui.heroDeath[p1Asset].frameTimer = 0.0f;
            }
        } else if (ui.pendingWinStatus == 3) {
            if (!ui.isP1Dying && !ui.isP2Dying) {
                ui.isP1Dying = true;
                ui.heroDeath[p1Asset].currentFrame = 0;
                ui.heroDeath[p1Asset].frameTimer = 0.0f;
                
                ui.isP2Dying = true;
                ui.heroDeath[p2Asset].currentFrame = 0;
                ui.heroDeath[p2Asset].frameTimer = 0.0f;
            }
        } else {
            game.matchStatus = ui.pendingWinStatus;
            ui.pendingWin = false;
            ui.pendingWinStatus = 0;
        }
    };

    // Update P1 attack
    if (ui.isP1Attacking) {
        CharAnim& atkAnim = getAtk(p1Asset, ui.p1AttackVariant);
        atkAnim.frameTimer += dt;
        if (atkAnim.frameTimer >= atkAnim.frameDuration) {
            atkAnim.frameTimer = 0.0f;
            atkAnim.currentFrame++;
            if (atkAnim.currentFrame >= atkAnim.frameCount) {
                ui.isP1Attacking = false;
                atkAnim.currentFrame = 0;
                if (ui.pendingWin) triggerDeath();
            }
        }
    } else if (ui.isP1Dying) {
        CharAnim& deathAnim = ui.heroDeath[p1Asset];
        deathAnim.frameTimer += dt;
        if (deathAnim.frameTimer >= deathAnim.frameDuration) {
            deathAnim.frameTimer = 0.0f;
            deathAnim.currentFrame++;
            if (deathAnim.currentFrame >= deathAnim.frameCount) {
                deathAnim.currentFrame = deathAnim.frameCount - 1;
                if (ui.pendingWinStatus != 0) {
                    game.matchStatus = ui.pendingWinStatus;
                    ui.pendingWinStatus = 0;
                }
                ui.pendingWin = false;
                ui.isP1Dying = false;
                ui.isP1Dead = true;
            }
        }
    } else if (!ui.isP1Dead) {
        CharAnim& idleAnim = ui.heroIdle[p1Asset];
        idleAnim.frameTimer += dt;
        if (idleAnim.frameTimer >= idleAnim.frameDuration) {
            idleAnim.frameTimer = 0.0f;
            idleAnim.currentFrame = (idleAnim.currentFrame + 1) % idleAnim.frameCount;
        }
    }

    // Update P2 attack
    if (ui.isP2Attacking) {
        CharAnim& atkAnim = getAtk(p2Asset, ui.p2AttackVariant);
        atkAnim.frameTimer += dt;
        if (atkAnim.frameTimer >= atkAnim.frameDuration) {
            atkAnim.frameTimer = 0.0f;
            atkAnim.currentFrame++;
            if (atkAnim.currentFrame >= atkAnim.frameCount) {
                ui.isP2Attacking = false;
                atkAnim.currentFrame = 0;
                if (ui.pendingWin) triggerDeath();
            }
        }
    } else if (ui.isP2Dying) {
        CharAnim& deathAnim = ui.heroDeath[p2Asset];
        deathAnim.frameTimer += dt;
        if (deathAnim.frameTimer >= deathAnim.frameDuration) {
            deathAnim.frameTimer = 0.0f;
            deathAnim.currentFrame++;
            if (deathAnim.currentFrame >= deathAnim.frameCount) {
                deathAnim.currentFrame = deathAnim.frameCount - 1;
                if (ui.pendingWinStatus != 0) {
                    game.matchStatus = ui.pendingWinStatus;
                    ui.pendingWinStatus = 0;
                }
                ui.pendingWin = false;
                ui.isP2Dying = false;
                ui.isP2Dead = true;
            }
        }
    } else if (!ui.isP2Dead) {
        CharAnim& idleAnim = ui.heroIdle[p2Asset];
        idleAnim.frameTimer += dt;
        if (idleAnim.frameTimer >= idleAnim.frameDuration) {
            idleAnim.frameTimer = 0.0f;
            idleAnim.currentFrame = (idleAnim.currentFrame + 1) % idleAnim.frameCount;
        }
    }

    // --- 2. HANDLE MOVE INPUT ---
        if (game.matchStatus == 0 && !ui.pendingWin && !ui.isP1Dying && !ui.isP2Dying) {
            bool moveMade = false;
            bool wasP1 = game.isPlayer1Turn;
            int moveRow = -1, moveCol = -1;
    
            if ((game.isVsBot && !game.isPlayer1Turn) || game.isBotVsBot) {
                game.botThinkTimer += dt;
                if (game.botThinkTimer >= 1.5f) {
                    game.botThinkTimer = 0.0f;
                    int currentBotPlayer = game.isPlayer1Turn ? 1 : 2;
                    BotMove(game, currentBotPlayer);
                    moveMade = true;
                    moveRow = game.lastMoveRow;
                    moveCol = game.lastMoveCol;
                }
            } else {
                if (game.inputType == 0) { // MOUSE MODE
                float gridWidth = BOARD_SIZE * ui.cellSize;
                float gridHeight = BOARD_SIZE * ui.cellSize;
    
                if (mouse.x >= ui.cellStartX && mouse.x < ui.cellStartX + gridWidth &&
                    mouse.y >= ui.cellStartY && mouse.y < ui.cellStartY + gridHeight) {
                    
                    int col = (mouse.x - ui.cellStartX) / ui.cellSize;
                    int row = (mouse.y - ui.cellStartY) / ui.cellSize;
    
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        moveMade = MakeMove(game, row, col);
                        if (moveMade) { moveRow = row; moveCol = col; }
                    }
                }
            }
            else if (game.inputType == 1) { // KEYBOARD MODE
                if (IsKeyPressed(KEY_W) && game.cursorRow > 0) game.cursorRow--;
                if (IsKeyPressed(KEY_S) && game.cursorRow < BOARD_SIZE - 1) game.cursorRow++;
                if (IsKeyPressed(KEY_A) && game.cursorCol > 0) game.cursorCol--;
                if (IsKeyPressed(KEY_D) && game.cursorCol < BOARD_SIZE - 1) game.cursorCol++;

                if (IsKeyPressed(KEY_ENTER)) {
                    moveMade = MakeMove(game, game.cursorRow, game.cursorCol);
                    if (moveMade) { moveRow = game.cursorRow; moveCol = game.cursorCol; }
                }
            }
            } // Ket thuc khoi else (khong phai luot bot)

            // --- 3. TRIGGER ATTACK ANIMATION ON A SUCCESSFUL MOVE ---
            if (moveMade) {
                int lastRow = -1, lastCol = -1;
                // Find the cell just placed (value wasP1 ? 1 : 2)
                // We already know row/col from input, saved before MakeMove
                // see moveRow/moveCol set above
                int variant = 0; // default attack_s1
                int myPlayer = wasP1 ? 1 : 2;
                int oppPlayer = wasP1 ? 2 : 1;

                // Check win: matchStatus != 0 means this move won
                if (game.matchStatus != 0) {
                    variant = 2; // attack_s3
                    ui.pendingWinStatus = game.matchStatus;
                    ui.pendingWin = true;
                    game.matchStatus = 0; // keep it, reveal after the animation

                    if (ui.pendingWinStatus == 3) {
                        // Draw -> both play attack_s3
                        ui.isP1Attacking = true;
                        ui.p1AttackVariant = 2;
                        getAtk(p1Asset, 2).currentFrame = 0;
                        getAtk(p1Asset, 2).frameTimer = 0.0f;

                        ui.isP2Attacking = true;
                        ui.p2AttackVariant = 2;
                        getAtk(p2Asset, 2).currentFrame = 0;
                        getAtk(p2Asset, 2).frameTimer = 0.0f;
                        
                        PlaySound(ui.heroAttackSound[p1Asset][2]); // both play the win (s3) sound
                        PlaySound(ui.heroAttackSound[p2Asset][2]);
                    }
                } else {
                    // Check whether this move blocks the opponent (using the placed row/col)
                    if (CheckBlockVariant(game, moveRow, moveCol, oppPlayer)) {
                        variant = 1; // attack_s2
                    }
                }

                if (ui.pendingWinStatus != 3) {
                    if (wasP1) {
                        ui.isP1Attacking = true;
                        ui.p1AttackVariant = variant;
                        getAtk(p1Asset, variant).currentFrame = 0;
                        getAtk(p1Asset, variant).frameTimer = 0.0f;
                        PlaySound(ui.heroAttackSound[p1Asset][variant]); // P1: s1/s2/s3 by variant
                    } else {
                        ui.isP2Attacking = true;
                        ui.p2AttackVariant = variant;
                        getAtk(p2Asset, variant).currentFrame = 0;
                        getAtk(p2Asset, variant).frameTimer = 0.0f;
                        PlaySound(ui.heroAttackSound[p2Asset][variant]); // P2: s1/s2/s3 by variant
                    }
                }

                // Spawn a cell effect at the placed cell
                if (ui.cellEffectCount < UIState::MAX_CELL_EFFECTS) {
                    int asset = wasP1 ? p1Asset : p2Asset;
                    UIState::CellEffect& eff = ui.cellEffects[ui.cellEffectCount++];
                    eff.row = moveRow;
                    eff.col = moveCol;
                    eff.player = wasP1 ? 1 : 2;
                    eff.heroAsset = asset;
                    eff.timer = 0.0f;
                    eff.frameDur = 0.07f;
                    eff.currentFrame = 0;
                    eff.done = false;
                }
            }
        }
    else { // END GAME STATE (Win/Lose/Draw)
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) ui.endGameSelection = 0;
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) ui.endGameSelection = 1;

        float btnW = 220.0f, btnH = 52.0f, btnGap = 20.0f;
        float btnY = ui.boardFrameRec.y + ui.boardFrameRec.height + 14.0f;
        Rectangle btnPlayAgain = { 960.0f - btnW - btnGap*0.5f, btnY, btnW, btnH };
        Rectangle btnMenu      = { 960.0f + btnGap*0.5f,        btnY, btnW, btnH };
        if (CheckCollisionPointRec(mouse, btnPlayAgain)) ui.endGameSelection = 0;
        if (CheckCollisionPointRec(mouse, btnMenu)) ui.endGameSelection = 1;

        bool confirm = false;
        if (IsKeyPressed(KEY_ENTER)) confirm = true;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && 
           (CheckCollisionPointRec(mouse, btnPlayAgain) || CheckCollisionPointRec(mouse, btnMenu))) {
            confirm = true;
        }

        if (confirm) {
            if (ui.endGameSelection == 0) {
                ResetRound(game);
                ResetHeroAnimState(ui);
            }
            else ui.currentScreen = 0;
        }
    }

    // Update win screen timer
    if (game.matchStatus != 0) ui.winScreenTimer += dt;

    // Update cell effects, clamp dt so a lag spike doesn't skip the whole animation
    float effDt = (dt > 0.1f) ? 0.1f : dt;
    for (int k = 0; k < ui.cellEffectCount; k++) {
        UIState::CellEffect& eff = ui.cellEffects[k];
        if (eff.done) continue;
        eff.timer += effDt;
        int totalFrames = ui.heroEffectFrames[eff.heroAsset];
        int frame = (int)(eff.timer / eff.frameDur);
        if (frame >= totalFrames) {
            eff.currentFrame = totalFrames - 1;
            eff.done = true;
        } else {
            eff.currentFrame = frame;
        }
    }

    // Cell effects are reset on a new round (ResetRound is called on confirm)
    // SHORTCUTS  (ESC now toggles the in-game pause overlay, handled above)
    if (IsKeyPressed(KEY_M)) ui.currentScreen = 0;
    if (IsKeyPressed(KEY_L)) {
        ui.currentScreen = 6; 
        ui.nameInput[0] = '\0'; 
        ui.letterCount = 0;
        ui.saveSelection = 0;
    }
}

static void DrawCharAnim(const CharAnim& c, float x, float y, float drawW, float drawH, bool flipH, bool isActive) {
    Rectangle srcRec = { (float)(c.currentFrame * c.frameWidth), 0, flipH ? -(float)c.frameWidth : (float)c.frameWidth, (float)c.frameHeight };
    Rectangle dstRec = { x, y, drawW, drawH };
    Color tint = isActive ? WHITE : Fade(GRAY, 0.5f);
    DrawTexturePro(c.spriteSheet, srcRec, dstRec, {0,0}, 0.0f, tint);
}

void DrawGUIGame(const GameState& game, const UIState& ui) {
    const int p1Asset = HERO_MAP[ui.p1HeroSelection];
    const int p2Asset = HERO_MAP[ui.p2HeroSelection];

    // BACKGROUND & BOARD
    DrawTexturePro(ui.bgGame, { 0, 0, (float)ui.bgGame.width, (float)ui.bgGame.height }, { 0, 0, 1920.0f, 1080.0f }, { 0, 0 }, 0.0f, WHITE);

    // === SIDE BACKGROUND PANELS behind heroes, full height ===
    float boardL = ui.boardFrameRec.x;
    float boardR = ui.boardFrameRec.x + ui.boardFrameRec.width;

    // Left panel: dark edge fading to transparent near the board
    DrawRectangleGradientH(0, 0, (int)(boardL + 40), 1080,
        {5, 4, 2, 230}, {5, 4, 2, 0});

    // Right panel: full screen
    DrawRectangleGradientH((int)(boardR - 40), 0, (int)(1920 - boardR + 40), 1080,
        {5, 4, 2, 0}, {5, 4, 2, 230});

    // Top panel: dark at top fading down to transparent
    DrawRectangleGradientV(0, 0, 1920, 160,
        {5, 4, 2, 200}, {5, 4, 2, 0});

    // Bottom panel: dark at bottom fading up
    DrawRectangleGradientV(0, 920, 1920, 160,
        {5, 4, 2, 0}, {5, 4, 2, 200});

    // Vignette: only darken the outer screen edges, keep heroes naturally lit
    // Left: dark outer edge (alpha 100), fades to 0 by 1/3 of the panel
    DrawRectangleGradientH(0, 0, (int)(boardL * 0.5f), 1080, {0,0,0,100}, {0,0,0,0});
    // Right
    DrawRectangleGradientH((int)(boardR + (1920-boardR)*0.5f), 0, (int)((1920-boardR)*0.5f), 1080, {0,0,0,0}, {0,0,0,100});
    // Top: only darken the very top, fade quickly
    DrawRectangleGradientV(0, 0, 1920, 80, {0,0,0,90}, {0,0,0,0});
    // Bottom
    float boardBot = ui.boardFrameRec.y + ui.boardFrameRec.height;
    DrawRectangleGradientV(0, (int)(boardBot + 40), 1920, 60, {0,0,0,0}, {0,0,0,80});


    // BOARD FRAME, code-drawn: dark semi-transparent stone + thin gold border
    {
        Rectangle fr = ui.boardFrameRec;
        float bt = (float)GetTime();
        float bpulse = 0.5f + 0.5f * sinf(bt * 2.0f);
        Color boardGold = { 255, 180, 0, 255 }; // Gothic Gold

        // Dark stone surface, kept translucent so the battlefield shows faintly through
        DrawRectangleRounded(fr, 0.02f, 8, Fade(Color{ 14, 10, 7, 255 }, 0.82f));
        // Soft inner shade to give depth toward the edges
        DrawRectangleGradientV((int)fr.x, (int)fr.y, (int)fr.width, (int)(fr.height * 0.5f),
            Fade(BLACK, 0.25f), Fade(BLACK, 0.0f));

        // Thin double gold border
        DrawRectangleRoundedLines(fr, 0.02f, 8, Fade(boardGold, 0.85f));
        DrawRectangleRoundedLines({ fr.x + 5, fr.y + 5, fr.width - 10, fr.height - 10 }, 0.02f, 8, Fade(boardGold, 0.22f));

        // Small gold corner diamonds, subtle gothic accent (pulses gently)
        auto BoardCorner = [&](float cx, float cy) {
            float s = 11.0f;
            Color c = Fade(boardGold, 0.55f + 0.25f * bpulse);
            DrawTriangle({ cx - s, cy }, { cx, cy + s }, { cx + s, cy }, c);
            DrawTriangle({ cx - s, cy }, { cx + s, cy }, { cx, cy - s }, c);
        };
        BoardCorner(fr.x, fr.y);
        BoardCorner(fr.x + fr.width, fr.y);
        BoardCorner(fr.x, fr.y + fr.height);
        BoardCorner(fr.x + fr.width, fr.y + fr.height);
    }

    Vector2 mousePos = GetMousePosition();
    
    // DRAW BOARD CELLS AND PIECES
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            float x = ui.cellStartX + j * ui.cellSize;
            float y = ui.cellStartY + i * ui.cellSize;
            Rectangle cellRect = { x, y, ui.cellSize, ui.cellSize };

            // Is this cell hovered (mouse) or under the keyboard cursor?
            bool hovered = false;
            if (game.inputType == 0 && CheckCollisionPointRec(mousePos, cellRect) && game.board[i][j].c == 0) {
                hovered = true;
            } else if (game.inputType == 1 && i == game.cursorRow && j == game.cursorCol) {
                hovered = true;
            }

            // Cell with a thin gold border; brighten the fill on hover/cursor
            Color cellBorder = { 255, 180, 0, 60 };
            if (hovered) {
                DrawRectangleRec(cellRect, Fade(Color{ 255, 200, 80, 255 }, 0.22f));
                cellBorder = Color{ 255, 200, 80, 200 };
            }
            DrawRectangleLinesEx(cellRect, 1.0f, cellBorder);

            if (game.board[i][j].c == 1 || game.board[i][j].c == 2) {
                int owner = game.board[i][j].c;
                int asset = (owner == 1) ? p1Asset : p2Asset;

                // Find the cell effect for this cell
                const UIState::CellEffect* eff = nullptr;
                for (int k = 0; k < ui.cellEffectCount; k++) {
                    if (ui.cellEffects[k].row == i && ui.cellEffects[k].col == j) {
                        eff = &ui.cellEffects[k];
                        break;
                    }
                }

                float pad = ui.cellSize * 0.10f;
                float drawSize = ui.cellSize - pad * 2.0f;
                float dx = x + pad, dy = y + pad;

                if (eff && !eff->done) {
                    // Draw the current effect frame (double size, centered on the cell)
                    const Texture2D& sheet = ui.heroEffect[eff->heroAsset];
                    int fh = sheet.height;
                    int fw = fh; // square frame
                    Rectangle src = { (float)(eff->currentFrame * fw), 0, (float)fw, (float)fh };
                    float effSize = drawSize * 2.0f;
                    float effX = x + ui.cellSize * 0.5f - effSize * 0.5f;
                    float effY = y + ui.cellSize * 0.5f - effSize * 0.5f;
                    Rectangle dst = { effX, effY, effSize, effSize };
                    DrawTexturePro(sheet, src, dst, {0,0}, 0.0f, WHITE);
                } else {
                    // Effect finished -> draw the static icon
                    const Texture2D& icon = ui.heroIcon[asset];
                    Rectangle src = { 0, 0, (float)icon.width, (float)icon.height };
                    Rectangle dst = { dx, dy, drawSize, drawSize };
                    DrawTexturePro(icon, src, dst, {0,0}, 0.0f, WHITE);
                }
            }
        }
    }
    
    if (game.inputType == 1) {
        float cx = ui.cellStartX + game.cursorCol * ui.cellSize;
        float cy = ui.cellStartY + game.cursorRow * ui.cellSize;
        DrawRectangleLinesEx({cx, cy, ui.cellSize, ui.cellSize}, 3.0f, DARKGREEN);
    }

    // MATCH INFO BADGES, only the ROUND badge is kept
    char roundText[30];
    snprintf(roundText, sizeof(roundText), "ROUND %d", game.roundCount);
    DrawBadgeText(ui.mainFont, ui.roundBadge, roundText, 15, 261.1f, 74.6f, 39, WHITE, 0.0f);

    // === HORIZONTAL BAR BELOW THE BOARD ===
    {
        float bx = ui.boardFrameRec.x;
        float bw = ui.boardFrameRec.width;
        float by = ui.boardFrameRec.y + ui.boardFrameRec.height + 10.0f;
        float bh = 88.0f;

        float hudT = (float)GetTime();
        float hudPulse = 0.5f + 0.5f * sinf(hudT * 2.5f);

        // HUD background gradient from edges to center
        DrawRectangleGradientH((int)bx, (int)by, (int)(bw*0.5f), (int)bh,
            {8, 6, 2, 245}, {20, 16, 5, 220});
        DrawRectangleGradientH((int)(bx + bw*0.5f), (int)by, (int)(bw*0.5f), (int)bh,
            {20, 16, 5, 220}, {8, 6, 2, 245});
        unsigned char borderA = (unsigned char)(170 + (int)(70 * hudPulse));
        DrawRectangleLinesEx({bx, by, bw, bh}, 2.0f, {160, 120, 40, borderA});
        DrawRectangleLinesEx({bx+3, by+3, bw-6, bh-6}, 1.0f, {100, 75, 25, 60});

        // Layout 3 zone
        float zL = bw * 0.22f;
        float zR = bw * 0.22f;
        float zM = bw - zL - zR;
        float zLx = bx;
        float zMx = bx + zL;
        float zRx = bx + zL + zM;

        // Dividers
        DrawLineEx({zMx, by+8}, {zMx, by+bh-8}, 1.5f, {160, 120, 40, 90});
        DrawLineEx({zRx, by+8}, {zRx, by+bh-8}, 1.5f, {160, 120, 40, 90});

        // LEFT zone: [L] Save, vertically centered in bh=88
        {
            Color keyCol   = {220, 185, 80, 240};
            Color labelCol = {185, 150, 65, 210};
            // badge+label block is 34px tall, center: by + (bh-34)/2
            float badY = by + (bh - 34.0f) * 0.5f;
            float badX = zLx + 18.0f;
            DrawRectangleRounded({badX, badY, 34, 34}, 0.25f, 4, {50, 38, 10, 200});
            DrawRectangleRoundedLines({badX, badY, 34, 34}, 0.25f, 4, Fade(keyCol, 0.80f));
            int lW = MeasureTextCustomX(ui.mainFont, "L", 24);
            int lH = MeasureTextCustomY(ui.mainFont, "L", 24);
            DrawTextCustom(ui.mainFont, "L",    (int)(badX + 17 - lW * 0.5f), (int)(badY + (34 - lH) * 0.5f), 24, keyCol);
            int svW = MeasureTextCustomX(ui.mainFont, "Save", 26);
            int svH = MeasureTextCustomY(ui.mainFont, "Save", 26);
            DrawTextCustom(ui.mainFont, "Save", (int)(badX + 42), (int)(badY + (34 - svH) * 0.5f), 26, labelCol);
        }

        // RIGHT zone: [ESC] Menu, vertically centered in bh=88
        {
            Color keyCol   = {210, 130, 60, 240};
            Color labelCol = {170, 110, 55, 210};
            float badY = by + (bh - 34.0f) * 0.5f;
            float badX = zRx + 18.0f;
            DrawRectangleRounded({badX, badY, 56, 34}, 0.25f, 4, {45, 22, 8, 200});
            DrawRectangleRoundedLines({badX, badY, 56, 34}, 0.25f, 4, Fade(keyCol, 0.75f));
            int escW = MeasureTextCustomX(ui.mainFont, "ESC", 22);
            int escH = MeasureTextCustomY(ui.mainFont, "ESC", 22);
            DrawTextCustom(ui.mainFont, "ESC",  (int)(badX + 28 - escW * 0.5f), (int)(badY + (34 - escH) * 0.5f), 22, keyCol);
            int mnW = MeasureTextCustomX(ui.mainFont, "Menu", 26);
            int mnH = MeasureTextCustomY(ui.mainFont, "Menu", 26);
            DrawTextCustom(ui.mainFont, "Menu", (int)(badX + 64), (int)(badY + (34 - mnH) * 0.5f), 26, labelCol);
        }

        // Zone GIỮA: TURN INFO
        {
            const char* turnName = game.isPlayer1Turn ? game.player1.name : game.player2.name;
            float glowA = 0.14f + 0.10f * hudPulse;
            unsigned char ga = (unsigned char)(glowA * 255);
            Color glowC = game.isPlayer1Turn
                ? Color{255, 165, 40, ga}
                : Color{90,  155, 255, ga};
            DrawRectangleGradientH((int)zMx, (int)by, (int)(zM*0.5f), (int)bh,
                Fade(glowC, 0.0f), glowC);
            DrawRectangleGradientH((int)(zMx+zM*0.5f), (int)by, (int)(zM*0.5f), (int)bh,
                glowC, Fade(glowC, 0.0f));

            // --- Fixed vertical layout for bh=88 ---
            // Line 1: "TURN" label    -> y = by + 8
            // Line 2: big player name -> y = by + 26
            // Line 3: small move count -> y = by + 62
            float midX = zMx + zM * 0.5f;

            // Small "TURN" label, centered horizontally, on top
            const char* lbl = "TURN";
            int lblW = MeasureTextCustomX(ui.mainFont, lbl, 16);
            int lblH = MeasureTextCustomY(ui.mainFont, lbl, 16);
            DrawTextCustom(ui.mainFont, lbl,
                (int)(midX - lblW * 0.5f), (int)(by + 8), 16, {160, 130, 65, 155});

            // Big player name, marquee if it overflows the middle zone
            Color nameCol = game.isPlayer1Turn
                ? Color{255, 210, 80, 255}
                : Color{130, 195, 255, 255};
            int tnFontSz  = 32;
            int tnH       = MeasureTextCustomY(ui.mainFont, turnName, tnFontSz);
            float nameY   = by + 28.0f;  // per the fixed vertical layout
            int tnW       = MeasureTextCustomX(ui.mainFont, turnName, tnFontSz);
            float nameClipPad = 12.0f;
            float nameClipW   = zM - nameClipPad * 2.0f;
            float nameClipX   = zMx + nameClipPad;
            if (tnW <= (int)nameClipW) {
                float nx = midX - tnW * 0.5f;
                DrawTextCustom(ui.mainFont, turnName, (int)(nx+2), (int)(nameY+2), tnFontSz, Fade(BLACK, 0.65f));
                DrawTextCustom(ui.mainFont, turnName, (int)(nx-1), (int)(nameY-1), tnFontSz, Fade(nameCol, 0.28f * hudPulse));
                DrawTextCustom(ui.mainFont, turnName, (int)nx,     (int)nameY,     tnFontSz, nameCol);
            } else {
                float sp2   = 70.0f;
                float prd2  = (tnW + sp2) / 50.0f;
                float t2    = fmodf((float)GetTime(), prd2);
                float pauseT2 = 1.0f;
                float off2 = (t2 < pauseT2) ? 0.0f : (t2 - pauseT2) * 50.0f;
                if (off2 > tnW + sp2) off2 = fmodf(off2, tnW + sp2);
                BeginScissorMode((int)nameClipX, (int)by, (int)nameClipW, (int)bh);
                DrawTextCustom(ui.mainFont, turnName, (int)(nameClipX - off2 + 2), (int)(nameY+2), tnFontSz, Fade(BLACK, 0.65f));
                DrawTextCustom(ui.mainFont, turnName, (int)(nameClipX - off2),     (int)nameY,     tnFontSz, nameCol);
                DrawTextCustom(ui.mainFont, turnName, (int)(nameClipX - off2 + tnW + (int)sp2 + 2), (int)(nameY+2), tnFontSz, Fade(BLACK, 0.65f));
                DrawTextCustom(ui.mainFont, turnName, (int)(nameClipX - off2 + tnW + (int)sp2),     (int)nameY,     tnFontSz, nameCol);
                EndScissorMode();
            }

            // Move count, centered horizontally, at the bottom
            char moveStr[16];
            snprintf(moveStr, sizeof(moveStr), "Move %d", game.moveCount);
            int mvW = MeasureTextCustomX(ui.mainFont, moveStr, 18);
            DrawTextCustom(ui.mainFont, moveStr,
                (int)(midX - mvW * 0.5f), (int)(by + 64), 18, {155, 125, 55, 160});
        }
    }

    float glowT = (float)GetTime();
    float glowAlpha = 0.55f + 0.45f * sinf(glowT * 4.0f);

    // Theme color per hero (asset index 1-5)
    const Color heroThemeColors[6] = {
        {150, 150, 150, 255},  // 0: black_knight (hidden)
        {220,  70,  20, 255},  // 1: fire_knight
        { 40, 170, 110, 255},  // 2: green_archer
        {180, 160,  60, 255},  // 3: earth_assassin - gold (former metal_blade)
        {180, 185, 195, 255},  // 4: metal_blade    - steel silver/grey
        { 30, 160, 220, 255},  // 5: water_mage
    };
    Color p1ThemeCol = heroThemeColors[HERO_MAP[ui.p1HeroSelection]];
    Color p2ThemeCol = heroThemeColors[HERO_MAP[ui.p2HeroSelection]];

    // DRAW PLAYER 1 & 2 NAMES, badge next to each hero
    int badgePWidth = 460, badgePHeight = 95, nameFontSize = 40;

    // Helper: draw a name in a badge, with marquee scrolling if too wide
    // clipPadH = horizontal padding (px each side), scroll speed = 55px/s
    auto DrawBadgeName = [&](const char* name, float bx, float by, float bw, float bh2, int fSize, Color col) {
        int tW = MeasureTextCustomX(ui.mainFont, name, fSize);
        int tH = MeasureTextCustomY(ui.mainFont, name, fSize);
        float clipPad = 18.0f;
        float clipW   = bw - clipPad * 2.0f;   // visible text area
        float clipX   = bx + clipPad;
        // Use fSize * 0.65f instead of tH (MeasureTextCustomY returns line-height larger than visual)
        float textY   = by + (bh2 - fSize * 0.65f) * 0.5f;  // precise vertical centering
        if (tW <= (int)clipW) {
            // Name fits -> just center horizontally
            float textX = bx + (bw - tW) * 0.5f;
            DrawTextCustom(ui.mainFont, name, (int)textX, (int)textY, fSize, col);
        } else {
            // Name too long -> marquee scroll
            float scrollSpeed = 55.0f;           // px/second
            float gap         = 40.0f;           // gap between the two copies
            float period      = (tW + gap) / scrollSpeed;
            float t           = fmodf((float)GetTime(), period);
            float offset      = t * scrollSpeed; // 0 -> (tW+gap) per cycle
            // Pause 1.2s before scrolling starts
            float pauseT = 1.2f;
            if (t < pauseT) offset = 0.0f;
            else            offset = (t - pauseT) * scrollSpeed;
            if (offset > tW + gap) offset = fmodf(offset, tW + gap);

            BeginScissorMode((int)clipX, (int)by, (int)clipW, (int)bh2);
            // Copy 1
            DrawTextCustom(ui.mainFont, name, (int)(clipX - offset), (int)textY, fSize, col);
            // Copy 2 (appears as copy 1 nears the end)
            DrawTextCustom(ui.mainFont, name, (int)(clipX - offset + tW + gap), (int)textY, fSize, col);
            EndScissorMode();
        }
    };

    float p1X = 230.0f - badgePWidth * 0.5f, p1Y = 324.0f;
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height},
        {p1X, p1Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    DrawBadgeName(game.player1.name, p1X, p1Y, (float)badgePWidth, (float)badgePHeight, nameFontSize, WHITE);
    float p2X = 1690.0f - badgePWidth * 0.5f, p2Y = 324.0f;
    DrawTexturePro(ui.playerBadge, {0, 0, (float)ui.playerBadge.width, (float)ui.playerBadge.height},
        {p2X, p2Y, (float)badgePWidth, (float)badgePHeight}, {0, 0}, 0.0f, WHITE);
    DrawBadgeName(game.player2.name, p2X, p2Y, (float)badgePWidth, (float)badgePHeight, nameFontSize, WHITE);
    // --- DRAW HEROES ---
    int p1A = HERO_MAP[ui.p1HeroSelection];
    int p2A = HERO_MAP[ui.p2HeroSelection];

    auto getAtkC = [&](int asset, int variant) -> const CharAnim& {
        if (variant == 1) return ui.heroAttack2[asset];
        if (variant == 2) return ui.heroAttack3[asset];
        return ui.heroAttack[asset];
    };

    // Scale 1.5x. The sprite sheet is large (1200x700) but the real character only occupies
    // the middle, so heroDrawOffset tells where the real character sits within the sprite.
    // Formula: anchor = where we want the real character to stand
    //   drawX = anchorX - (spriteW/2 + offsetX)   [negative offsetX = character shifted right in sprite]
    //   drawY = anchorY - spriteH - offsetY        [negative offsetY = character shifted down in sprite]
    float charScale = 1.5f;

    const CharAnim& p1Anim = ui.isP1Attacking ? getAtkC(p1A, ui.p1AttackVariant)
                           : (ui.isP1Dying || ui.isP1Dead) ? ui.heroDeath[p1A]
                           :                    ui.heroIdle[p1A];
    bool p1Active = true; // always lit, the spotlight indicates whose turn it is
    float p1W = ui.heroDrawSize[p1A].x * charScale;
    float p1H = ui.heroDrawSize[p1A].y * charScale;
    float pedestalH = 44.0f; // pedestal height, the character stands on top of it

    float p1AnchorX = 230.0f,  p1AnchorY = 1010.0f - pedestalH;
    float p1DrawX = p1AnchorX - p1W * 0.5f;
    float p1DrawY = p1AnchorY - p1H;

    const CharAnim& p2Anim = ui.isP2Attacking ? getAtkC(p2A, ui.p2AttackVariant)
                           : (ui.isP2Dying || ui.isP2Dead) ? ui.heroDeath[p2A]
                           :                    ui.heroIdle[p2A];
    bool p2Active = true;
    float p2W = ui.heroDrawSize[p2A].x * charScale;
    float p2H = ui.heroDrawSize[p2A].y * charScale;
    float p2AnchorX = 1690.0f, p2AnchorY = 1010.0f - pedestalH;
    float p2DrawX = p2AnchorX - p2W * 0.5f;
    float p2DrawY = p2AnchorY - p2H;

    // === SPOTLIGHT under the character, stage style ===
    auto drawSpotlight = [&](float cx, float fy, bool active, Color c) {
        float pulse = 0.75f + 0.25f * sinf(glowT * 3.5f);

        // Trapezoid pedestal, +30% size
        float topRx = 124.0f;  // 95 * 1.3
        float topRy = 21.0f;   // 16 * 1.3
        float botRx = 169.0f;  // 130 * 1.3
        float botRy = 29.0f;   // 22 * 1.3
        float pH    = 44.0f;   // a bit taller for balance
        float topY  = fy - pH;
        float pRx = topRx, pRy = topRy;

        // Pedestal side color: stone-like, lightly tinted with the theme color
        unsigned char sr = (unsigned char)((int)c.r * 30 / 100 + 55);
        unsigned char sg = (unsigned char)((int)c.g * 30 / 100 + 55);
        unsigned char sb = (unsigned char)((int)c.b * 30 / 100 + 55);
        // Pedestal top color: brighter than the sides, still opaque
        unsigned char tr2 = (unsigned char)((int)c.r * 35 / 100 + 80);
        unsigned char tg2 = (unsigned char)((int)c.g * 35 / 100 + 80);
        unsigned char tb2 = (unsigned char)((int)c.b * 35 / 100 + 80);

        int segs = 80;

        // === PEDESTAL SIDE (trapezoid) ===
        for (int i = 0; i < segs; i++) {
            float a0 = (float)i       * 2.0f * PI / segs;
            float a1 = (float)(i + 1) * 2.0f * PI / segs;

            // Shade: the front (sinf > 0) is brighter
            float shade0 = 0.55f + 0.45f * sinf(a0);
            float shade1 = 0.55f + 0.45f * sinf(a1);
            float shadeAvg = (shade0 + shade1) * 0.5f;

            float bx0 = cx + cosf(a0) * botRx, by0 = fy    + sinf(a0) * botRy;
            float bx1 = cx + cosf(a1) * botRx, by1 = fy    + sinf(a1) * botRy;
            float tx0 = cx + cosf(a0) * topRx, ty0 = topY  + sinf(a0) * topRy;
            float tx1 = cx + cosf(a1) * topRx, ty1 = topY  + sinf(a1) * topRy;

            unsigned char cr2 = (unsigned char)(sr * (0.6f + 0.4f * shadeAvg));
            unsigned char cg2 = (unsigned char)(sg * (0.6f + 0.4f * shadeAvg));
            unsigned char cb2 = (unsigned char)(sb * (0.6f + 0.4f * shadeAvg));

            DrawTriangle({bx0,by0},{tx0,ty0},{bx1,by1}, {cr2,cg2,cb2,255});
            DrawTriangle({tx0,ty0},{tx1,ty1},{bx1,by1}, {cr2,cg2,cb2,255});
        }

        // === PEDESTAL TOP, solid ===
        DrawEllipse((int)cx, (int)topY, (int)topRx, (int)topRy, {tr2,tg2,tb2,255});
        // Center highlight, a soft bright spot
        DrawEllipse((int)cx, (int)topY, (int)(topRx*0.55f), (int)(topRy*0.55f),
            {(unsigned char)((int)tr2+25<255?(int)tr2+25:255),
             (unsigned char)((int)tg2+25<255?(int)tg2+25:255),
             (unsigned char)((int)tb2+25<255?(int)tb2+25:255), 255});

        // === TOP EDGE, theme-colored glow ===
        for (int i = 0; i < segs; i++) {
            float a0 = (float)i       * 2.0f * PI / segs;
            float a1 = (float)(i + 1) * 2.0f * PI / segs;
            float ex0 = cx + cosf(a0) * topRx, ey0 = topY + sinf(a0) * topRy;
            float ex1 = cx + cosf(a1) * topRx, ey1 = topY + sinf(a1) * topRy;
            DrawLineEx({ex0,ey0},{ex1,ey1}, 5.0f, {c.r,c.g,c.b,(unsigned char)(70*pulse)});
            DrawLineEx({ex0,ey0},{ex1,ey1}, 2.0f,
                {(unsigned char)((c.r+255)/2),(unsigned char)((c.g+255)/2),(unsigned char)((c.b+255)/2),
                 (unsigned char)(220*pulse)});
        }

        // === PEDESTAL BOTTOM EDGE ===
        for (int i = 0; i < segs; i++) {
            float a0 = (float)i       * 2.0f * PI / segs;
            float a1 = (float)(i + 1) * 2.0f * PI / segs;
            float ex0 = cx + cosf(a0) * botRx, ey0 = fy + sinf(a0) * botRy;
            float ex1 = cx + cosf(a1) * botRx, ey1 = fy + sinf(a1) * botRy;
            DrawLineEx({ex0,ey0},{ex1,ey1}, 2.0f, {sr,sg,sb,200});
        }

        // === DECORATIVE GROOVE across the side ===
        float midH = 0.45f;
        float rRx  = topRx + (botRx - topRx) * midH;
        float rRy  = topRy + (botRy - topRy) * midH;
        float rY   = topY  + pH * midH;
        for (int i = 0; i < segs; i++) {
            float a0 = (float)i       * 2.0f * PI / segs;
            float a1 = (float)(i + 1) * 2.0f * PI / segs;
            float lx0 = cx + cosf(a0) * rRx, ly0 = rY + sinf(a0) * rRy;
            float lx1 = cx + cosf(a1) * rRx, ly1 = rY + sinf(a1) * rRy;
            DrawLineEx({lx0,ly0},{lx1,ly1}, 3.0f, {15,15,15,200});
            DrawLineEx({lx0,ly0},{lx1,ly1}, 1.0f,
                {(unsigned char)((c.r+180)/2),(unsigned char)((c.g+180)/2),(unsigned char)((c.b+180)/2),
                 (unsigned char)(90*pulse)});
        }

        // === RISING FLAMES, only when active, particles like the menu ===
        if (active) {
            // 40 virtual particles, each with its own phase seed
            // 90 total: 40 from the top, 50 from the pedestal sides
            int pCount = 90;
            float speed = 0.45f;
            for (int k = 0; k < pCount; k++) {
                float phaseOffset = (float)k / pCount;
                float t = fmodf(glowT * speed + phaseOffset, 1.0f);

                float spawnX, spawnY;
                if (k < 40) {
                    // Group 1: spawn on the pedestal top
                    float spawnAngle = phaseOffset * 2.0f * PI * 3.7f;
                    float spawnR = topRx * (0.25f + 0.70f * (0.5f + 0.5f * sinf(spawnAngle * 2.1f)));
                    spawnX = cx + cosf(spawnAngle) * spawnR;
                    spawnY = topY + sinf(spawnAngle) * topRy * (spawnR / topRx);
                } else {
                    // Group 2: spawn on the pedestal sides, spread around botRx/midRx
                    float spawnAngle = phaseOffset * 2.0f * PI * 5.3f;
                    // Pseudo-random position along the side (between top and bottom)
                    float sideT = 0.3f + 0.7f * (0.5f + 0.5f * sinf(phaseOffset * 19.1f));
                    float sRx = topRx + (botRx - topRx) * sideT;
                    float sRy = topRy + (botRy - topRy) * sideT;
                    float sY  = topY  + pH * sideT;
                    spawnX = cx + cosf(spawnAngle) * sRx;
                    spawnY = sY + sinf(spawnAngle) * sRy;
                }

                // Rise as high as the hero
                float riseMax = 420.0f + 80.0f * sinf(phaseOffset * 13.7f);
                float sway = sinf(glowT * 1.8f + phaseOffset * 17.3f) * 10.0f * (1.0f - t * 0.7f);
                float px = spawnX + sway;
                float py = spawnY - t * riseMax;

                // Alpha
                float life = (t < 0.1f) ? (t / 0.1f) : (1.0f - (t - 0.1f) / 0.9f);
                life = life * life;
                unsigned char fa2 = (unsigned char)(life * 210);
                if (fa2 < 5) continue;

                float sz = (1.0f - t * 0.9f) * (4.0f + 2.5f * sinf(phaseOffset * 11.1f));

                // Color: outer = orange/red mixed with theme, mid = theme, core = bright white
                // outer glow
                DrawCircle((int)px, (int)py, sz * 2.4f,
                    {c.r, (unsigned char)(c.g/3), (unsigned char)(c.b/4), (unsigned char)(fa2 * 0.35f)});
                // mid: pure theme color
                DrawCircle((int)px, (int)py, sz * 1.5f,
                    {c.r, c.g, c.b, fa2});
                // core: white mixed with theme
                DrawCircle((int)px, (int)py, sz * 0.7f,
                    {(unsigned char)((c.r + 255) / 2),
                     (unsigned char)((c.g + 255) / 2),
                     (unsigned char)((c.b + 255) / 2),
                     (unsigned char)(fa2 < 255 ? fa2 : 255)});
            }
        }

        // === GLOW under the pedestal ===
        DrawEllipse((int)cx,(int)fy,(int)(botRx+55),(int)(botRy+9),{c.r,c.g,c.b,(unsigned char)(15*pulse)});
        DrawEllipse((int)cx,(int)fy,(int)(botRx+22),(int)(botRy+4),{c.r,c.g,c.b,(unsigned char)(38*pulse)});

        if (!active) {
            DrawEllipse((int)cx,(int)topY,(int)topRx,(int)topRy,{0,0,0,130});
        }
    };
    // fy = the real ground line (anchor + pedestalH = 1010)
    drawSpotlight(p1AnchorX, p1AnchorY + pedestalH - 5.0f, game.isPlayer1Turn,  p1ThemeCol);
    drawSpotlight(p2AnchorX, p2AnchorY + pedestalH - 5.0f, !game.isPlayer1Turn, p2ThemeCol);

    // DRAW GLOW ELLIPSE behind the winning hero, must come BEFORE DrawCharAnim
    if (game.matchStatus != 0 && game.matchStatus != 3) {
        float _t2  = (float)GetTime();
        float _p   = 0.5f + 0.5f * sinf(_t2 * 3.0f);
        float _p2  = 0.5f + 0.5f * sinf(_t2 * 1.8f + 0.7f);
        int   _win = game.matchStatus;
        float _wx  = (_win == 1) ? 230.0f : 1690.0f;
        Color _wc  = heroThemeColors[(_win==1) ? HERO_MAP[ui.p1HeroSelection]
                                                : HERO_MAP[ui.p2HeroSelection]];
        float glowA = 0.20f + 0.10f * _p2;
        for (int r = 300; r >= 60; r -= 20) {
            float frac = (float)(r - 60) / 240.0f;
            float a = glowA * (1.0f - frac * frac);
            DrawEllipse((int)_wx, 820, r, (int)(r * 0.55f), Fade(_wc, a));
        }
        DrawEllipse((int)_wx, 820, 90, 30, Fade(WHITE, 0.10f + 0.06f*_p));
    }

    DrawCharAnim(p1Anim, p1DrawX, p1DrawY, p1W, p1H, false, p1Active);
    DrawCharAnim(p2Anim, p2DrawX, p2DrawY, p2W, p2H, true, p2Active);

    // DRAW END GAME SCREEN
    if (game.matchStatus != 0) {
        float t      = (float)GetTime();
        float pulse  = 0.5f + 0.5f * sinf(t * 3.0f);
        float pulse2 = 0.5f + 0.5f * sinf(t * 1.8f + 0.7f);
        float wt     = ui.winScreenTimer; // time since the win screen appeared

        bool isDraw    = (game.matchStatus == 3);
        int  winPlayer = game.matchStatus;
        int  loserPlayer = (winPlayer == 1) ? 2 : 1;
        float winnerAnchorX = (winPlayer == 1) ? 230.0f  : 1690.0f;
        float loserAnchorX  = (winPlayer == 1) ? 1690.0f : 230.0f;
        float winnerBadgeX  = winnerAnchorX;
        float winnerBadgeY  = 374.0f;

        Color winTheme = isDraw
            ? Color{180,180,180,255}
            : heroThemeColors[(winPlayer==1)
                ? HERO_MAP[ui.p1HeroSelection]
                : HERO_MAP[ui.p2HeroSelection]];

        float bfx = ui.boardFrameRec.x;
        float bfy = ui.boardFrameRec.y;
        float bfw = ui.boardFrameRec.width;
        float bfh = ui.boardFrameRec.height;

        // 1. CORNER VIGNETTE, slightly darken edges without covering the board
        int vgStr = 160; // vignette strength
        DrawRectangleGradientH(0, 0, 320, 1080, {0,0,0,(unsigned char)vgStr}, {0,0,0,0});
        DrawRectangleGradientH(1600, 0, 320, 1080, {0,0,0,0}, {0,0,0,(unsigned char)vgStr});
        DrawRectangleGradientV(0, 0, 1920, 220, {0,0,0,(unsigned char)vgStr}, {0,0,0,0});
        DrawRectangleGradientV(0, 860, 1920, 220, {0,0,0,0}, {0,0,0,(unsigned char)vgStr});

        // 3. LOSING HERO fades out (lower alpha)
        // Already handled by the hero draw above with low alpha when matchStatus != 0
        // Add one more dark overlay layer over the losing hero area
        if (!isDraw) {
            float loserX = loserAnchorX;
            // Semi-transparent dark ellipse over the losing hero area
            for (int r = 250; r >= 50; r -= 25) {
                float frac = (float)(r - 50) / 200.0f;
                float a = 0.35f * (1.0f - frac * frac);
                DrawEllipse((int)loserX, 650, r, (int)(r * 1.4f), Fade(BLACK, a));
            }
        }

        // 4. CONFETTI falling from the top
        if (!isDraw) {
            // 80 confetti pieces, alternating theme + white + gold
            Color confColors[4] = {
                winTheme,
                WHITE,
                {255, 215, 0, 255},
                {winTheme.r, (unsigned char)(winTheme.g/2+127), winTheme.b, 255}
            };
            for (int k = 0; k < 80; k++) {
                float phase   = (float)k / 80.0f;
                float speed   = 0.28f + 0.18f * sinf(phase * 7.3f);
                float cycleT  = fmodf(wt * speed + phase, 1.0f);
                float cx = 80.0f + phase * 1760.0f + sinf(phase*13.1f + t*1.2f) * 60.0f;
                float cy = -20.0f + cycleT * 1120.0f;
                float sz = 4.0f + 3.0f * sinf(phase * 9.7f);
                float alpha = (cycleT > 0.85f) ? (1.0f - cycleT) / 0.15f : 0.85f;
                Color cc = confColors[k % 4];
                // Small rotating piece (a tilted little rectangle)
                float angle = t * (60.0f + phase * 120.0f) + phase * 360.0f;
                DrawRectanglePro({cx, cy, sz*2.0f, sz*0.8f}, {sz, sz*0.4f}, angle,
                    Fade(cc, alpha * 0.9f));
            }
        }

        // 5. PARTICLES around the winning hero's feet, more and more varied
        if (!isDraw) {
            for (int k = 0; k < 80; k++) {
                float phase  = (float)k / 80.0f;
                float speed  = 0.45f + 0.20f * sinf(phase * 11.3f);
                float cycleT = fmodf(t * speed + phase, 1.0f);
                float spread = (k % 3 == 0) ? 130.0f : (k % 3 == 1) ? 80.0f : 50.0f;
                float px = winnerAnchorX + sinf(phase*17.3f + cycleT*6.28f) * spread;
                float py = 1010.0f - cycleT * (420.0f + 180.0f * sinf(phase*11.7f));
                float alpha = (cycleT < 0.15f) ? cycleT/0.15f
                            : (cycleT > 0.72f) ? (1.0f-cycleT)/0.28f : 1.0f;
                alpha *= 0.85f;
                float sz = 1.8f + 3.0f * sinf(phase * 9.1f);
                // Alternate theme color and white
                Color pc = (k % 4 == 0) ? WHITE : winTheme;
                DrawCircleV({px, py}, sz,        Fade(pc, alpha));
                DrawCircleV({px, py}, sz * 2.5f, Fade(pc, alpha * 0.15f));
            }
        }

        // 6. HIGHLIGHT THE 5 WINNING CELLS, a blinking bright line
        if (!isDraw) {
            float x0 = ui.cellStartX + game.winLine[0][1]*ui.cellSize + ui.cellSize*0.5f;
            float y0 = ui.cellStartY + game.winLine[0][0]*ui.cellSize + ui.cellSize*0.5f;
            float x4 = ui.cellStartX + game.winLine[4][1]*ui.cellSize + ui.cellSize*0.5f;
            float y4 = ui.cellStartY + game.winLine[4][0]*ui.cellSize + ui.cellSize*0.5f;
            // Wide outer glow
            DrawLineEx({x0,y0},{x4,y4}, 14.0f, Fade(winTheme, 0.18f + 0.10f*pulse));
            // Middle glow
            DrawLineEx({x0,y0},{x4,y4}, 7.0f,  Fade(winTheme, 0.45f + 0.20f*pulse));
            // Main line
            DrawLineEx({x0,y0},{x4,y4}, 3.0f,  Fade(winTheme, 1.0f));
            // Blinking white core
            DrawLineEx({x0,y0},{x4,y4}, 1.5f,  Fade(WHITE, 0.60f + 0.40f*pulse));
        }

        // 7. "WINS!" TEXT with scale-in animation
        if (!isDraw) {
            const char* winText = "WINS!";
            // Scale ease-out: 0.3 -> 1.0 over the first 0.4s
            float scl = wt < 0.4f ? (0.3f + 0.7f * (wt / 0.4f)) : 1.0f;
            // Easing cubic out
            float t01 = wt < 0.4f ? (wt / 0.4f) : 1.0f;
            scl = 1.0f - (1.0f - t01) * (1.0f - t01) * (1.0f - t01);
            scl = 0.3f + scl * 0.7f;

            int baseFontSize = 62;
            int fontSize = (int)(baseFontSize * scl);
            if (fontSize < 8) fontSize = 8;
            int wTw = MeasureTextCustomX(ui.mainFont, winText, fontSize);
            float textX = winnerBadgeX - wTw * 0.5f;
            float textY = winnerBadgeY - 78.0f;

            // Shadow
            DrawTextCustom(ui.mainFont, winText, (int)(textX+3), (int)(textY+3), fontSize, Fade(BLACK, 0.80f));
            // Large glow
            DrawTextCustom(ui.mainFont, winText, (int)(textX-2), (int)(textY-2), fontSize, Fade(winTheme, 0.25f * pulse));
            DrawTextCustom(ui.mainFont, winText, (int)(textX+2), (int)(textY+2), fontSize, Fade(winTheme, 0.15f * pulse));
            // Main text, brightness pulse
            Color tc = {winTheme.r, winTheme.g, winTheme.b, (unsigned char)(210 + 45*pulse)};
            DrawTextCustom(ui.mainFont, winText, (int)textX, (int)textY, fontSize, tc);
            // Shine sweeping across the text (moving white band)
            float shineX = winnerBadgeX - 200.0f + fmodf(wt * 280.0f, 600.0f);
            DrawRectanglePro({shineX, textY-5, 40, (float)fontSize+10}, {20,0}, 15.0f,
                Fade(WHITE, 0.18f));
        } else {
            const char* drawText = "DRAW!";
            float scl = wt < 0.4f ? (0.3f + 0.7f * (wt / 0.4f)) : 1.0f;
            float t01 = wt < 0.4f ? (wt / 0.4f) : 1.0f;
            scl = 1.0f - (1.0f - t01) * (1.0f - t01) * (1.0f - t01);
            scl = 0.3f + scl * 0.7f;

            int baseFontSize = 100;
            int fontSize = (int)(baseFontSize * scl);
            if (fontSize < 8) fontSize = 8;

            int dw = MeasureTextCustomX(ui.mainFont, drawText, fontSize);
            int dh = MeasureTextCustomY(ui.mainFont, drawText, fontSize);
            
            float textX = bfx + bfw * 0.5f - dw * 0.5f;
            float textY = bfy + bfh * 0.5f - dh * 0.5f;
            
            // Slightly darken the board so the text stands out
            DrawRectangle((int)bfx, (int)bfy, (int)bfw, (int)bfh, Fade(BLACK, 0.45f * t01));

            DrawTextCustom(ui.mainFont, drawText, (int)(textX+4), (int)(textY+4), fontSize, Fade(BLACK, 0.8f));
            Color tc = {180, 180, 180, (unsigned char)(205 + 50 * pulse)};
            DrawTextCustom(ui.mainFont, drawText, (int)textX, (int)textY, fontSize, tc);
        }

        // ── 8. BUTTON ──
        float btnW = 240.0f, btnH = 58.0f, btnGap = 24.0f;
        float btnY = bfy + bfh + 14.0f;
        Rectangle btnPlayAgain = { 960.0f - btnW - btnGap*0.5f, btnY, btnW, btnH };
        Rectangle btnMenu      = { 960.0f + btnGap*0.5f,        btnY, btnW, btnH };

        bool hovPlay = (ui.endGameSelection == 0);
        bool hovMenu = (ui.endGameSelection == 1);

        // ── Play Again button ──
        {
            Color bgPlay = hovPlay ? Color{30,110,50,240}  : Color{10,10,10,200};
            Color bdPlay = hovPlay ? Color{60,200,85,255}  : Fade(winTheme, 0.45f);
            // Outer glow khi hover
            if (hovPlay) {
                DrawRectangleRoundedLines({btnPlayAgain.x-3, btnPlayAgain.y-3, btnPlayAgain.width+6, btnPlayAgain.height+6},
                    0.30f, 8, Fade({60,200,85,255}, 0.25f * pulse));
            }
            DrawRectangleRounded(btnPlayAgain, 0.28f, 8, bgPlay);
            // Top shine
            DrawRectangleGradientV((int)btnPlayAgain.x+4, (int)btnPlayAgain.y+2,
                (int)btnPlayAgain.width-8, (int)(btnPlayAgain.height*0.45f),
                Fade(WHITE, hovPlay ? 0.09f : 0.04f), Fade(WHITE, 0.0f));
            DrawRectangleRoundedLines(btnPlayAgain, 0.28f, 8, bdPlay);

            // Key hint badge
            DrawRectangleRounded({btnPlayAgain.x+8, btnPlayAgain.y+8, 28, 22}, 0.35f, 4,
                Fade(BLACK, 0.50f));
            DrawRectangleRoundedLines({btnPlayAgain.x+8, btnPlayAgain.y+8, 28, 22}, 0.35f, 4,
                Fade({80,200,100,255}, 0.60f));
            DrawTextCustom(ui.mainFont, "A", (int)(btnPlayAgain.x+16), (int)(btnPlayAgain.y+11), 16,
                Fade({130,230,140,255}, 0.85f));

            const char* lblPlay = "Play Again";
            int wpL = MeasureTextCustomX(ui.mainFont, lblPlay, 28);
            DrawTextCustom(ui.mainFont, lblPlay,
                (int)(btnPlayAgain.x + (btnW-wpL)*0.5f + 6),
                (int)(btnPlayAgain.y + (btnH-28)*0.5f), 28,
                hovPlay ? WHITE : Fade(WHITE, 0.55f));
        }

        // ── Main Menu button ──
        {
            Color bgMenu2 = hovMenu ? Color{120,70,10,240} : Color{10,10,10,200};
            Color bdMenu2 = hovMenu ? Color{210,140,30,255}: Fade(winTheme, 0.45f);
            if (hovMenu) {
                DrawRectangleRoundedLines({btnMenu.x-3, btnMenu.y-3, btnMenu.width+6, btnMenu.height+6},
                    0.30f, 8, Fade({210,140,30,255}, 0.25f * pulse));
            }
            DrawRectangleRounded(btnMenu, 0.28f, 8, bgMenu2);
            DrawRectangleGradientV((int)btnMenu.x+4, (int)btnMenu.y+2,
                (int)btnMenu.width-8, (int)(btnMenu.height*0.45f),
                Fade(WHITE, hovMenu ? 0.09f : 0.04f), Fade(WHITE, 0.0f));
            DrawRectangleRoundedLines(btnMenu, 0.28f, 8, bdMenu2);

            // Key hint badge
            DrawRectangleRounded({btnMenu.x+8, btnMenu.y+8, 28, 22}, 0.35f, 4,
                Fade(BLACK, 0.50f));
            DrawRectangleRoundedLines({btnMenu.x+8, btnMenu.y+8, 28, 22}, 0.35f, 4,
                Fade({210,160,50,255}, 0.60f));
            DrawTextCustom(ui.mainFont, "D", (int)(btnMenu.x+16), (int)(btnMenu.y+11), 16,
                Fade({230,190,90,255}, 0.85f));

            const char* lblMenu = "Main Menu";
            int wmL = MeasureTextCustomX(ui.mainFont, lblMenu, 28);
            DrawTextCustom(ui.mainFont, lblMenu,
                (int)(btnMenu.x + (btnW-wmL)*0.5f + 6),
                (int)(btnMenu.y + (btnH-28)*0.5f), 28,
                hovMenu ? WHITE : Fade(WHITE, 0.55f));
        }

        // ── Footer: keyboard navigation hint ──
        {
            float footY = btnY + btnH + 8.0f;
            const char* footHint = "[A] Play Again     [D] Main Menu     [ENTER] Confirm";
            int fhW = MeasureTextCustomX(ui.mainFont, footHint, 20);
            // Subtle pill background
            DrawRectangleRounded({960.0f - fhW*0.5f - 16, footY, (float)fhW+32, 28}, 0.5f, 6,
                {8, 6, 2, 140});
            DrawRectangleRoundedLines({960.0f - fhW*0.5f - 16, footY, (float)fhW+32, 28}, 0.5f, 6,
                Fade(winTheme, 0.20f));
            DrawTextCustom(ui.mainFont, footHint,
                (int)(960.0f - fhW*0.5f), (int)(footY + 4), 20,
                Fade({180, 155, 80, 255}, 0.50f));
        }
    }

    // GEAR BUTTON (top-right) — only while the match is actively in progress
    if (game.matchStatus == 0 && !ui.pendingWin && !ui.isP1Dying && !ui.isP2Dying) {
        Vector2 gearCenter = { 1850.0f, 56.0f };
        bool hov = ui.inGamePaused || CheckCollisionPointCircle(GetMousePosition(), gearCenter, 34.0f);
        Color gold = { 255, 180, 0, 255 };
        DrawCircle((int)gearCenter.x, (int)gearCenter.y, 30.0f, Fade(BLACK, 0.45f));
        DrawCircleLines((int)gearCenter.x, (int)gearCenter.y, 30.0f, Fade(gold, hov ? 0.9f : 0.5f));
        DrawGearIcon(gearCenter.x, gearCenter.y, hov ? 19.5f : 18.0f, Fade(gold, hov ? 1.0f : 0.8f));
    }

    // PAUSE OVERLAY — drawn on top of everything when open
    if (ui.inGamePaused) DrawPauseOverlay(game, ui);
}