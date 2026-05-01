#ifndef HEALTH_HPP
#define HEALTH_HPP

#include "raylib.h"
#include <string>

class Health {
public:
    Rectangle barRect;
    Color playerColor;
    Color enemyColor;
    
    float health;       // 0.0 a 2.0
    float visualHealth; // LERP
    
    Texture2D bgTexture;
    Texture2D iconPlayer;
    Texture2D iconEnemy;
    
    float iconScale;
    float iconLerpSpeed;
    
    Font scoreFont;
    int songScore;
    float visualScore;

    Health(float x, float y, Texture2D bg, Texture2D iconP, Texture2D iconE, Font font);
    ~Health();

    void Update(float healthValue, int currentScore);
    void Draw();
    
private:
    std::string FormatScore(int score);
};

#endif