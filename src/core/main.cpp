#include "raylib.h"
#include "raymath.h"
#include "playstate.hpp"
#include "constants.hpp"
#include "debug/chartEditor.hpp"
#include "ui/editorUI.hpp"
#include "core/paths.hpp" 
#include <algorithm>
#include <iostream>

// Window Dark Mode
#ifdef _WIN32
    #ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
        #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
    #endif
    typedef void* HWND;
    typedef int HRESULT;
    typedef int BOOL;
    extern "C" {
        __stdcall HRESULT DwmSetWindowAttribute(HWND hwnd, unsigned long dwAttribute, const void* pvAttribute, unsigned long cbAttribute);
        HWND GetWindowHandle(void);
    }
#endif

enum GameState { PLAYING, CHART_EDITOR };
GameState currentState = PLAYING;

// Global Variables
const int gameWidth = 1280;
const int gameHeight = 720;
static float globalVolume = 0.5f;
std::string fullTitle = Constants::TITLE + " - v" + Constants::VERSION;

int main() {
    // Init Window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(gameWidth, gameHeight, fullTitle.c_str());
    // InitWindow(gameWidth, gameHeight, constants::TITLE.c_str());
    InitAudioDevice();
    EditorUI::Init();

    // Active Dark Mode
    #ifdef _WIN32
        HWND hwnd = GetWindowHandle();
        int useDarkMode = 1; 
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
    #endif

    SetTargetFPS(60); // Set Max FPS

    // Init PlayState
    PlayState* playState = new PlayState();
    ChartEditor* editor = nullptr;

    // Game Loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Global Volume
        if (IsKeyDown(KEY_EQUAL)) globalVolume += 0.01f;
        if (IsKeyDown(KEY_MINUS)) globalVolume -= 0.01f;
        globalVolume = std::clamp(globalVolume, 0.0f, 1.0f);
        SetMasterVolume(globalVolume);
        
        // Update
        switch (currentState) {
            case PLAYING:
                playState->Update();
                
                // Tecla 7 para ir ao Editor
                if (IsKeyPressed(KEY_SEVEN)) {
                    currentState = CHART_EDITOR;
                }

                // Tecla R para Restart
                if (IsKeyPressed(KEY_R)) {
                    delete playState;
                    playState = new PlayState();
                }
                break;

            case CHART_EDITOR:
                if (editor == nullptr) {
                    editor = new ChartEditor();
                    // Opcional: Pausar o inst do playstate ao editar
                    PauseMusicStream(playState->inst);
                }
                editor->Update(dt);

                // ESC para voltar ao jogo
                if (IsKeyPressed(KEY_ESCAPE)) {
                    delete editor;
                    editor = nullptr;
                    currentState = PLAYING;
                    ResumeMusicStream(playState->inst);
                }
                break;
        }

        // Draw
        BeginDrawing();
            // ClearBackground({ 10, 10, 10, 255 });
            // Draw PlayState NOW!!
            if (currentState == PLAYING) {
                playState->Draw();
            } 
            else if (currentState == CHART_EDITOR && editor != nullptr) {
                editor->Draw();
            }
            // HUD Global
            DrawTextEx(
                EditorUI::mainFont,
                TextFormat("VOL: %i%%", (int)(globalVolume * 100)),
                { 10.0f, 10.0f },
                16.0f,
                0.0f,
                WHITE
            );
        EndDrawing();
    }

    // CleanUp
    delete playState;
    if (editor) delete editor;
    EditorUI::Unload();
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}