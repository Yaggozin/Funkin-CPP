#ifndef EDITORUI_HPP
#define EDITORUI_HPP

#include "raylib.h"
#include <string>

struct EditorWindow {
    Rectangle rect;
    std::string title;
    bool dragging = false;
    Vector2 dragOffset = { 0, 0 };
    bool minimized = false;
     bool visible = true;
};

class EditorUI {
public:
    static Font mainFont;

    static void Init();
    static void Unload();

    static void DrawText(const char* text, Vector2 pos, float size, Color color, float spacing = 0.0f);
    
    static bool Button(Rectangle rect, const char* label, float fontSize = 18.0f, Color bg = DARKGRAY, Color textCol = WHITE);

    // Window system
    static void DrawWindow(EditorWindow &win);
    static void DrawPanel(Rectangle rect, Color color);

    // NEW UI ELEMENTS
    static void DrawSection(Rectangle rect, const char* title);
    static void DrawSlider(Rectangle rect, float value);
    static void DrawSeparator(Rectangle rect);

    // Layout helper
    static Rectangle Pad(Rectangle rect, float padding);
};

#endif