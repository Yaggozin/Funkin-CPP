#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "raylib.h"
#include <string>
#include <cstring>

namespace Constants {

    // Informações do Jogo
    const std::string TITLE = "Friday Night Funkin'";
    const std::string VERSION = "0.1.0";

    // Câmera
    const float DEFAULT_CAMERA_FOLLOW_RATE = 0.04f;
    const float DEFAULT_CAMERA_ZOOM = 1.05f;
    const float CAMERA_ZOOM_BEAT_ADD = 0.015f;

    // Gameplay
    const float DEFAULT_SCROLL_SPEED = 2.4f;
    const float SING_ANIM_HOLD_TIME = 0.4f;
    const float SAFE_ZONE_OFFSET = 160.0f;
    
    // HUD and Default Offsets
    const Vector2 BF_CAMERA_OFFSET = { 150, 100 };
    const Vector2 DAD_CAMERA_OFFSET = { 250, 200 };
    
    // Colors
    const Color COLOR_HEALTH_BAR_GREEN = { 49, 176, 209, 255 };
    const Color COLOR_HEALTH_BAR_RED = { 175, 102, 206, 255 };

    // Chart Editor
    const int GRID_SIZE = 40;            // Tamanho de cada quadrado no chart
    const int STRUM_COUNT = 8;           // 4 para o oponente, 4 para o player
    const Color GRID_COLOR_1 = { 255, 255, 255, 255 }; // Branco
    const Color GRID_COLOR_2 = { 230, 230, 230, 255 }; // Cinza claro
    const Color GRID_BORDER  = { 200, 200, 200, 255 }; // Bordas internas mais suaves
    
    // Notas no Editor
    const Color EDITOR_NOTE_LEFT = { 194, 75, 153, 255 };
    const Color EDITOR_NOTE_DOWN = { 0, 255, 255, 255 };
    const Color EDITOR_NOTE_UP = { 18, 250, 5, 255 };
    const Color EDITOR_NOTE_RIGHT = { 249, 57, 63, 255 };

}

#endif