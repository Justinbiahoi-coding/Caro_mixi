#ifndef GUI_H
#define GUI_H
#define _CRT_SECURE_NO_WARNINGS
#include "Library.h"

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
    Texture2D pieceX;
    Texture2D pieceO;
    Texture2D playerBadge;
    Texture2D roundBadge; 
    Texture2D titleBadge;
    float cellSize;       
    float cellStartX;     
    float cellStartY;     
    Rectangle boardFrameRec;

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

    Font mainFont;

    CharAnim charP1; 
    CharAnim charP2; 

    Music bgMusic;
    float musicVolume = 0.8f;
    bool musicEnabled;
    bool draggingVolume = false;
};

void InitGUI(UIState& ui);
void UnloadGUI(UIState& ui);
void UpdateGUI(GameState& game, UIState& ui);
void DrawGUI(const GameState& game, const UIState& ui);


// --------------------------------------   
// helper
static float fontSpacing = 0.0f;

inline void DrawTextCustom(Font font, const char *text, int posX, int posY, int fontSize, Color color) {
    // Spacing (khoảng cách chữ) để mặc định là 1.0f
    DrawTextEx(font, text, {(float)posX, (float)posY}, (float)fontSize, fontSpacing, color);
}

inline int MeasureTextCustomX(Font font, const char *text, int fontSize) {
    return (int)MeasureTextEx(font, text, (float)fontSize, fontSpacing).x;
}
inline int MeasureTextCustomY(Font font, const char *text, int fontSize) {
    return (int)MeasureTextEx(font, text, (float)fontSize, fontSpacing).y;
}

inline void DrawBadgeText(Font font, Texture2D badge, const char *text, int y, float badgeWidth, float badgeHeight, int textFontSize, Color textColor, float offsetY = 0.0f) {
    // 1. Tính toán vị trí Badge để căn giữa ngang
    float badgeX = (1920.0f - badgeWidth) / 2.0f; 

    // 2. Tính toán độ rộng/cao của text
    float textWidth = (float)MeasureTextCustomX(font, text, textFontSize);
    float textHeight = (float)MeasureTextCustomY(font, text, textFontSize);

    // 3. Tính toán vị trí Text để căn giữa (Đã bỏ công thức cũ, thay bằng offsetY an toàn hơn)
    float textX = badgeX + (badgeWidth - textWidth) / 2.0f;
    float textY = y + (badgeHeight - textHeight) / 2.0f + offsetY; 

    // 4. Vẽ Badge 
    DrawTexturePro(badge, {0, 0, (float)badge.width, (float)badge.height}, 
                   {badgeX, (float)y, badgeWidth, badgeHeight}, {0, 0}, 0.0f, WHITE);

    // 5. Vẽ Text lên trên
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

#endif // GUI_H