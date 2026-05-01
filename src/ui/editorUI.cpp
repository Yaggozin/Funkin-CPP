#include "ui/editorUI.hpp"
#include "core/paths.hpp"
#include <iostream>

Font EditorUI::mainFont;

// UI state system (future use)
/*enum UIState {
    UI_IDLE,
    UI_HOVER,
    UI_PRESSED
};*/

void EditorUI::Init() {
    // Carrega a fonte usando seu sistema de paths
    mainFont = LoadFontEx(Paths::font("arial").c_str(), 32, 0, 250);

    if (mainFont.texture.id <= 0) {
        std::cout << "Failed to load font\n";
    }
    SetTextureFilter(mainFont.texture, TEXTURE_FILTER_BILINEAR);
}

void EditorUI::Unload() {
    UnloadFont(mainFont);
}

void EditorUI::DrawText(const char* text, Vector2 pos, float size, Color color, float spacing) {
    DrawTextEx(mainFont, text, pos, size, spacing, color);
}

bool EditorUI::Button(Rectangle rect, const char* label, float fontSize, Color bg, Color textCol) {
    Vector2 mousePoint = GetMousePosition();
    bool hovering = CheckCollisionPointRec(mousePoint, rect);
    bool clicked = hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool pressed = hovering && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    Color currentBg = bg;

    if (pressed) {
        currentBg = ColorBrightness(bg, -0.2f);
    } else if (hovering) {
        currentBg = ColorBrightness(bg, 0.2f);
    }
    
    DrawRectangleRec(rect, currentBg);

    // Soft top highlight
    DrawRectangle(rect.x, rect.y, rect.width, 2, Color{ 255, 255, 255, 20 });

    // Soft bottom shadow
    DrawRectangle(rect.x, rect.y + rect.height - 2, rect.width, 2, Color{ 0, 0, 0, 60 });

    float spacing = 1.0f;
    Vector2 textSize = MeasureTextEx(mainFont, label, fontSize, spacing);
    Vector2 textPos = { 
        rect.x + (rect.width / 2) - (textSize.x / 2), 
        rect.y + (rect.height / 2) - (textSize.y / 2) 
    };

    DrawTextEx(mainFont, label, textPos, fontSize, spacing, textCol);
    return clicked;
}

void EditorUI::DrawPanel(Rectangle rect, Color color) {
    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);
}

// Window container
void EditorUI::DrawWindow(EditorWindow &win) {
    if (!win.visible) return;

    Vector2 mousePos = GetMousePosition();
    float titleBarHeight = 50.0f;
    Rectangle titleBar = { win.rect.x, win.rect.y, win.rect.width, titleBarHeight };

    // Drag start
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, titleBar)) {
        win.dragging = true;
        win.dragOffset = { mousePos.x - win.rect.x, mousePos.y - win.rect.y };
    }

    // Drag update
    if (win.dragging) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            win.rect.x = mousePos.x - win.dragOffset.x;
            win.rect.y = mousePos.y - win.dragOffset.y;
        } else {
            win.dragging = false;
        }
        if (win.rect.x < 0) win.rect.x = 0;
        if (win.rect.y < 0) win.rect.y = 0;
        if (win.rect.x + win.rect.width > GetScreenWidth())
            win.rect.x = GetScreenWidth() - win.rect.width;
        if (win.rect.y + win.rect.height > GetScreenHeight())
            win.rect.y = GetScreenHeight() - win.rect.height;
    }

    // Main background
    DrawRectangleRec(win.rect, Color{ 45, 45, 45, 255 }); 

    // Title Bar
    Color titleColor = win.dragging ? Color{ 100, 100, 110, 255 } : Color{ 82, 83, 85, 255 };
    //Color titleColor = win.dragging ? Color{ 82, 83, 85, 255 } : Color{ 82, 83, 85, 255 };
    DrawRectangleRec(titleBar, titleColor);

    // Border
    DrawRectangleLinesEx(win.rect, 2, Color{ 38, 36, 50, 255 }); 

    // Title
    DrawTextEx(mainFont, win.title.c_str(), 
    { win.rect.x + 15, win.rect.y + 16 }, 
    18.0f, 
    0, 
    Color{ 142, 143, 145, 255 }
    );
}

void EditorUI::DrawSection(Rectangle rect, const char* title) {
    // Background
    DrawRectangleRec(rect, Color{ 50, 52, 55, 255 });

    // Top line (highlight)
    DrawRectangle(rect.x, rect.y, rect.width, 2, Color{ 70, 70, 80, 255 });

    // Title
    DrawTextEx(mainFont, title, 
        { rect.x + 10, rect.y + 6 }, 
        14, 
        0, 
        LIGHTGRAY
    );
}
void EditorUI::DrawSlider(Rectangle rect, float value) {
    // Background
    DrawRectangleRec(rect, Color{ 30, 30, 30, 255 });

    // Fill
    DrawRectangle(rect.x, rect.y, rect.width * value, rect.height, Color{ 80, 120, 200, 255 });

    // Handle
    float x = rect.x + rect.width * value;
    DrawRectangle(x - 3, rect.y - 2, 6, rect.height + 4, WHITE);
}
void EditorUI::DrawSeparator(Rectangle rect) {
    DrawRectangle(rect.x, rect.y, rect.width, 1, Color{70, 70, 70, 255});
}
Rectangle EditorUI::Pad(Rectangle rect, float padding) {
    return {
        rect.x + padding,
        rect.y + padding,
        rect.width - padding * 2,
        rect.height - padding * 2
    };
}