#ifndef GUI_H
#define GUI_H
#define _CRT_SECURE_NO_WARNINGS
#include <raylib.h>
#include "DataStruct.h"

struct CharAnim {
    Texture2D spriteSheet;
    int frameCount;      // total number of frames in the sprite sheet
    int frameWidth;      // width 1 frame = sheet.width / frameCount
    int frameHeight;     // = sheet.height 
    int currentFrame;
    float frameTimer;
    float frameDuration; // each second 1 frame
};

struct UIState {
    int currentScreen; 
    int menuSelection;
    int settingSelection;
    int loadSelection;
    int saveSelection;
    int p1HeroSelection; 
    int p2HeroSelection; 
    int selectionPhase; 
    int endGameSelection; 

    char nameInput[30];
    int letterCount;
    bool shouldExit;

    char p1NameInput[16]; 
    char p2NameInput[16];
    int p1LetterCount;
    int p2LetterCount;
    
    int activeInputField; 
    
    Texture2D boardFrame;
    Texture2D cell;
    Texture2D playerBadge;
    Texture2D roundBadge;


    float cellSize;       
    float cellStartX;     
    float cellStartY;     
    Rectangle boardFrameRec;
    Texture2D cursorNormal; 
    Texture2D cursorClick;  
    Texture2D bgMenu;
    Texture2D btnNewGame;
    Texture2D btnLoadGame;
    Texture2D bgGame;
    Texture2D btnSettings;
    Texture2D btnHelp;
    Texture2D btnCredits;
    Texture2D btnExit;
    Texture2D bgSettings;
    Texture2D bgSaveLoad;
    Texture2D bgLoadGame;
    Texture2D bgSelect;
    Texture2D helpBg;
    float helpScrollTimer; // time since Help opened, used for panel scale-in
    bool isHelpVietnamese; // Add language toggle

    Font mainFont;

    CharAnim heroIdle[8];        // Idle animations (index 0=black_knight hidden, 1-7 active)
    CharAnim heroAttack[8];      // attack_s1
    CharAnim heroAttack2[8];     // attack_s2
    CharAnim heroAttack3[8];     // attack_s3
    CharAnim heroDeath[8];       // death animation
    Texture2D heroIcon[8];       // attack_icon, used as the board piece
    Vector2 heroDrawSize[8];
    Vector2 heroDrawOffset[8];
    // Legacy support
    CharAnim& charP1 = heroIdle[0];
    CharAnim& charP2 = heroIdle[1];
    CharAnim& atkP1 = heroAttack[0];
    CharAnim& atkP2 = heroAttack[1];
    bool isP1Attacking;
    bool isP2Attacking;
    int p1AttackVariant; // 0/1/2 = s1/s2/s3
    int p2AttackVariant;
    bool pendingWin;        // true while waiting for attack_s3 to finish
    int  pendingWinStatus;  // the real matchStatus (1 or 2)
    bool isP1Dying;         // P1 death animation playing
    bool isP2Dying;         // P2 death animation playing
    bool isP1Dead;          // P1 dead, hold last frame
    bool isP2Dead;          // P2 dead, hold last frame
    float winScreenTimer;   // time since win screen appeared (for scale-in)

    // Attack effect on a board cell
    Texture2D heroEffect[8];   // attack_effect sprite sheet
    int heroEffectFrames[8];   // frame count per hero
    Sound heroAttackSound[8][3];  // [hero][variant] 0=normal(s1) 1=block(s2) 2=win(s3)

    struct CellEffect {
        int row, col;
        int player;        // 1 or 2
        int heroAsset;     // index into heroEffect[]
        float timer;       // total elapsed time
        float frameDur;    // seconds per frame
        int currentFrame;
        bool done;         // effect finished, draw icon only
    };
    static const int MAX_CELL_EFFECTS = 225; // 15x15
    CellEffect cellEffects[MAX_CELL_EFFECTS];
    int cellEffectCount;

    Music bgMusic;
    float musicVolume = 0.8f;
    bool musicEnabled;
    bool draggingVolume = false;
    float sfxVolume = 0.8f;     // volume for hero attack sound effects (0.0 - 1.0)
    bool draggingSFX = false;   // true while the SFX volume slider is being dragged
    bool inGamePaused = false;  // true when the in-game gear/pause settings overlay is open
    int  pauseSelection = 0;    // focused control in the pause overlay: 0=Music 1=SFX 2=Toggle 3=Resume 4=Menu

    // Menu particles
    struct Ember {
        float x, y;
        float vx, vy;
        float alpha;
        float size;
        float life; // 0.0 → 1.0
    };
    static const int MAX_EMBERS = 160;
    Ember embers[MAX_EMBERS];

    // Character select particles
    struct CharParticle {
        float x, y;
        float vx, vy;
        float alpha;
        float size;
        float life;      // 0.0 → 1.0
        int   heroID;    // which hero spawned it (0–3)
        int   type;      // 0 = rising ember, 1 = rune diamond
        float angle;     // for rune rotation
        float angleV;    // rotation speed
    };
    static const int MAX_CHAR_PARTICLES = 200;
    CharParticle charParticles[MAX_CHAR_PARTICLES];

    // Smooth menu scroll
    float menuScrollY = 0.0f; // current highlight position (interpolated pixels)
};

// Maps selection index (0-4) → asset index (1-5), black_knight hidden at index 0
static const int HERO_MAP[7] = {1, 2, 3, 4, 5, 6, 7};

void InitGUI(UIState& ui);
void UnloadGUI(UIState& ui);
void UpdateGUI(GameState& game, UIState& ui);
void DrawGUI(const GameState& game, const UIState& ui);


// --------------------------------------   
// helper
static float fontSpacing = 0.0f;

inline void DrawTextCustom(Font font, const char *text, int posX, int posY, int fontSize, Color color) {
    DrawTextEx(font, text, {(float)posX, (float)posY}, (float)fontSize, fontSpacing, color);
}

inline int MeasureTextCustomX(Font font, const char *text, int fontSize) {
    return (int)MeasureTextEx(font, text, (float)fontSize, fontSpacing).x;
}
inline int MeasureTextCustomY(Font font, const char *text, int fontSize) {
    return (int)MeasureTextEx(font, text, (float)fontSize, fontSpacing).y;
}

inline void DrawBadgeText(Font font, Texture2D badge, const char *text, int y, float badgeWidth, float badgeHeight, int textFontSize, Color textColor, float offsetY = 0.0f) {
    // Center the badge horizontally
    float badgeX = (1920.0f - badgeWidth) / 2.0f;

    // MeasureTextCustomY returns the full line-height (with ascender/descender),
    // so use fontSize * 0.65f to estimate the real glyph height for better vertical centering.
    float textWidth   = (float)MeasureTextCustomX(font, text, textFontSize);
    float visualTextH = textFontSize * 0.65f;   // approximate visual glyph height

    // Center text inside the badge
    float textX = badgeX + (badgeWidth  - textWidth)  * 0.5f;
    float textY = y      + (badgeHeight - visualTextH) * 0.5f + offsetY;

    // Draw badge, then text on top
    DrawTexturePro(badge, {0, 0, (float)badge.width, (float)badge.height},
                   {badgeX, (float)y, badgeWidth, badgeHeight}, {0, 0}, 0.0f, WHITE);
    DrawTextCustom(font, text, (int)textX, (int)textY, textFontSize, textColor);
}

inline CharAnim LoadCharAnim(const char* path, int frameCount, float frameDuration) {
    CharAnim c = {};
    c.spriteSheet  = LoadTexture(path);
    c.frameCount   = frameCount;
    c.frameWidth   = c.spriteSheet.width / frameCount;
    c.frameHeight  = c.spriteSheet.height;
    c.frameDuration = frameDuration;
    return c;
}

inline void UnloadCharAnim(CharAnim& c) {
    UnloadTexture(c.spriteSheet);
}

// Apply the current SFX volume to every loaded hero attack sound.
// heroAttackSound[0] (black_knight) is never loaded, so start at index 1.
// Call this once after sounds are loaded, and whenever ui.sfxVolume changes.
inline void ApplySFXVolume(UIState& ui) {
    for (int i = 1; i < 8; i++) {
        for (int v = 0; v < 3; v++) {
            SetSoundVolume(ui.heroAttackSound[i][v], ui.sfxVolume);
        }
    }
}

// Reusable draggable volume slider (used by both the Settings screen and the
// in-game pause overlay). Updates `value` in [0,1] while the user drags it and
// keeps `dragging` in sync with the mouse button. Returns true while dragging,
// so the caller can apply side effects (SetMusicVolume / ApplySFXVolume).
inline bool VolumeSliderUpdate(float& value, bool& dragging, Vector2 mouse,
                               int barX, int barY, int barWidth, int barHeight) {
    Rectangle hit = { (float)barX - 20, (float)barY - 20,
                      (float)barWidth + 40, (float)barHeight + 40 };
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, hit))
        dragging = true;
    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        dragging = false;
    if (dragging) {
        float t = (mouse.x - (float)barX) / (float)barWidth;
        value = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t);
    }
    return dragging;
}

// Draw the matching slider track + fill + knob + percentage label.
inline void VolumeSliderDraw(Font font, int barX, int barY, int barWidth, int barHeight,
                             float value, bool active, Color activeColor) {
    DrawRectangleRec({ (float)barX, (float)barY, (float)barWidth, (float)barHeight }, DARKGRAY);
    DrawRectangle(barX, barY, (int)(barWidth * value), barHeight, MAROON);
    Rectangle knob = { barX + value * barWidth - 8.0f, (float)barY - 12.0f, 16.0f, 48.0f };
    DrawRectangleRec(knob, active ? activeColor : (Color){ 200, 150, 40, 255 });
    DrawTextEx(font, TextFormat("%i%%", (int)(value * 100)),
               { (float)(barX + barWidth + 20), (float)barY - 5 }, 30.0f, fontSpacing, WHITE);
}

// Reset both heroes' animation state (frame + timer) when a new game starts.
// Call this on EVERY switch to currentScreen = 1 to avoid stuck death/attack frames.
inline void ResetHeroAnimState(UIState& ui) {
    ui.pendingWin       = false;
    ui.pendingWinStatus = 0;
    ui.isP1Attacking    = false;
    ui.isP2Attacking    = false;
    ui.p1AttackVariant  = 0;
    ui.p2AttackVariant  = 0;
    ui.isP1Dying        = false;
    ui.isP2Dying        = false;
    ui.isP1Dead         = false;
    ui.isP2Dead         = false;
    ui.winScreenTimer   = 0.0f;
    ui.cellEffectCount  = 0;
    ui.inGamePaused     = false;

    int assets[2] = { HERO_MAP[ui.p1HeroSelection], HERO_MAP[ui.p2HeroSelection] };
    for (int i = 0; i < 2; i++) {
        int a = assets[i];
        ui.heroIdle[a].currentFrame    = 0;  ui.heroIdle[a].frameTimer    = 0.0f;
        ui.heroDeath[a].currentFrame   = 0;  ui.heroDeath[a].frameTimer   = 0.0f;
        ui.heroAttack[a].currentFrame  = 0;  ui.heroAttack[a].frameTimer  = 0.0f;
        ui.heroAttack2[a].currentFrame = 0;  ui.heroAttack2[a].frameTimer = 0.0f;
        ui.heroAttack3[a].currentFrame = 0;  ui.heroAttack3[a].frameTimer = 0.0f;
    }
}

#endif // GUI_H