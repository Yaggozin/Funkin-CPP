#ifndef STRUMLINE_HPP
#define STRUMLINE_HPP

#include "raylib.h"
#include <vector>

struct StrumArrow {
    Vector2 position;
    Rectangle source;
    int direction;
    float scale;
    float pressTimer;
    bool confirmed; // Para quando a nota é acertada (glow)
};

class StrumLine {
public:
    StrumArrow arrows[4];
    Texture2D texture;
    float xOffset;
    float yOffset;
    bool isPlayer;

    StrumLine(float xPos, bool playerSide);
    ~StrumLine();

    void Update(float dt);
    void Draw();
    
    // Função para dar o "bop" ou brilho na seta
    void PlayAnim(int direction, float timer = 0.1f);
};

#endif