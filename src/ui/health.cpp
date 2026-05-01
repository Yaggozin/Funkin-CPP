#include "health.hpp"
#include "raymath.h"
#include <cstdio>
#include <cstring>
#include <cmath>

Health::Health(float x, float y, Texture2D bg, Texture2D iconP, Texture2D iconE, Font font) {
    bgTexture = bg;
    iconPlayer = iconP;
    iconEnemy = iconE;
    scoreFont = font;
    barRect = { x, y, (float)bgTexture.width, (float)bgTexture.height };
    
    playerColor = { 102, 255, 0, 255 }; 
    enemyColor = { 255, 0, 0, 255 };   
    
    health = 1.0f;
    visualHealth = 1.0f;
    iconScale = 1.0f;
    iconLerpSpeed = 0.15f; // Valor padrão de lerp dos ícones
    songScore = 0;
    visualScore = 0.0f;
}

Health::~Health() {
    // As texturas geralmente são gerenciadas pelo state principal, 
    // mas se precisar descarregar aqui:
    // UnloadTexture(bgTexture);
}

std::string Health::FormatScore(int score) {
    char temp[32];
    sprintf(temp, "%d", score);
    
    int len = strlen(temp);
    int commas = (len - 1) / 3;
    int outLen = len + commas;
    
    char buffer[32];
    buffer[outLen] = '\0';
    int count = 0;
    for (int i = len - 1, j = outLen - 1; i >= 0; i--, j--) {
        buffer[j] = temp[i];
        count++;
        if (count == 3 && i != 0) {
            buffer[--j] = ',';
            count = 0;
        }
    }
    return std::string(buffer);
}

void Health::Update(float healthValue, int currentScore) {
    health = healthValue;
    songScore = currentScore;
    
    // Lógica de update que estava no DrawUI do C, agora separada
    visualHealth = Lerp(visualHealth, health, 0.15f);
    iconScale = Lerp(iconScale, 1.0f, iconLerpSpeed);
    visualScore = Lerp(visualScore, (float)songScore, 0.2f);
    
    if (fabsf(visualScore - (float)songScore) < 0.1f) {
        visualScore = (float)songScore;
    }
}

void Health::Draw() {
    // 1. Fundo da barra
    DrawTexture(bgTexture, (int)barRect.x, (int)barRect.y, WHITE);

    float innerWidth = barRect.width - 8;
    float innerHeight = barRect.height - 8;
    float innerX = barRect.x + 4;
    float innerY = barRect.y + 4;
    
    float healthPercent = visualHealth / 2.0f; 
    float playerWidth = innerWidth * healthPercent;

    // 2. Retângulos de vida
    DrawRectangle(innerX, innerY, innerWidth, innerHeight, enemyColor);
    DrawRectangle(innerX + (innerWidth - playerWidth), innerY, playerWidth, innerHeight, playerColor);

    // 3. Texto do Score (com outline/contorno)
    std::string formatted = FormatScore((int)visualScore);
    const char* scoreText = TextFormat("Score: %s", formatted.c_str());
    
    float fontSize = 14.0f;
    float spacing = -1.0f; 
    Vector2 textSize = MeasureTextEx(scoreFont, scoreText, fontSize, spacing);
    
    float scoreX = barRect.x + (barRect.width - 120) - (textSize.x / 2);
    float scoreY = barRect.y + barRect.height + 5;

    // Outline
    DrawTextEx(scoreFont, scoreText, { scoreX + 1, scoreY + 1 }, fontSize, spacing, BLACK);
    DrawTextEx(scoreFont, scoreText, { scoreX - 1, scoreY - 1 }, fontSize, spacing, BLACK);
    DrawTextEx(scoreFont, scoreText, { scoreX + 1, scoreY - 1 }, fontSize, spacing, BLACK);
    DrawTextEx(scoreFont, scoreText, { scoreX - 1, scoreY + 1 }, fontSize, spacing, BLACK);
    // Main Text
    DrawTextEx(scoreFont, scoreText, { scoreX, scoreY }, fontSize, spacing, WHITE);

    // 4. Ícones
    float iconSpacing = 27.0f;
    float iconAnchorX = innerX + (innerWidth - playerWidth);
    float baseSize = 150.0f;
    float currentSize = baseSize * iconScale;
    float iconY = barRect.y + (barRect.height / 2) - (currentSize / 2);
    
    // Opponent Icon (Frame 1 se vida > 80%)
    int dadFrame = (visualHealth > 1.6f) ? 1 : 0;
    Rectangle dadSource = { dadFrame * 150.0f, 0, 150.0f, 150.0f };
    Rectangle dadDest = { iconAnchorX - currentSize + iconSpacing, iconY, currentSize, currentSize };
    DrawTexturePro(iconEnemy, dadSource, dadDest, {0, 0}, 0.0f, WHITE);

    // Player Icon (Frame 1 se vida < 20% | Invertido no X)
    int bfFrame = (visualHealth < 0.4f) ? 1 : 0;
    Rectangle bfSource = { bfFrame * 150.0f, 0, -150.0f, 150.0f }; 
    Rectangle bfDest = { iconAnchorX - iconSpacing, iconY, currentSize, currentSize };
    DrawTexturePro(iconPlayer, bfSource, bfDest, {0, 0}, 0.0f, WHITE);
}