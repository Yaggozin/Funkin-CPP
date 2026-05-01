#include "strumline.hpp"
#include "core/paths.hpp"

static const float swagWidth = 112.0f;

StrumLine::StrumLine(float xPos, bool playerSide) {
    xOffset = xPos;
    yOffset = 50.0f;
    isPlayer = playerSide;
    
    texture = LoadTexture(Paths::image("ui/NOTE_assets").c_str());

    for (int i = 0; i < 4; i++) {
        arrows[i].direction = i;
        arrows[i].scale = 0.7f;
        arrows[i].pressTimer = 0.0f;
        arrows[i].confirmed = false;

        switch(i) {
            case 0: arrows[i].source = { 310, 235, 153, 157 }; break; // LEFT
            case 1: arrows[i].source = { 0, 235, 157, 153 };   break; // DOWN
            case 2: arrows[i].source = { 784, 232, 157, 153 }; break; // UP
            case 3: arrows[i].source = { 157, 235, 153, 157 }; break; // RIGHT
        }

        arrows[i].position = { xOffset + (i * swagWidth), yOffset };
    }
}

StrumLine::~StrumLine() {
    UnloadTexture(texture);
}

void StrumLine::Update(float dt) {
    int keys[4] = { KEY_LEFT, KEY_DOWN, KEY_UP, KEY_RIGHT };

    for (int i = 0; i < 4; i++) {
        arrows[i].position.x = xOffset + (i * swagWidth);
        arrows[i].position.y = yOffset;
        if (arrows[i].pressTimer > 0) {
            arrows[i].pressTimer -= dt;
            
            if (arrows[i].pressTimer <= 0) {
                switch(i) {
                    case 0: arrows[i].source = { 310, 235, 153, 157 }; break;
                    case 1: arrows[i].source = { 0, 235, 157, 153 };   break;
                    case 2: arrows[i].source = { 784, 232, 157, 153 }; break;
                    case 3: arrows[i].source = { 157, 235, 153, 157 }; break;
                }
            }
        }

        if (isPlayer) {
            if (IsKeyDown(keys[i])) {
                arrows[i].pressTimer = 0.1f; 
                switch(i) {
                    case 0: arrows[i].source = { 463, 389, 146, 149 }; break;
                    case 1: arrows[i].source = { 0, 388, 149, 146 };   break;
                    case 2: arrows[i].source = { 1850, 308, 153, 150 }; break;
                    case 3: arrows[i].source = { 784, 385, 148, 151 };  break;
                }
            }
        }
    }
}

void StrumLine::PlayAnim(int direction, float timer) {
    if (direction < 0 || direction > 3) return;
    arrows[direction].pressTimer = timer;
    switch(direction) {
        case 0: arrows[direction].source = { 463, 389, 146, 149 }; break; // LEFT CONFIRM
        case 1: arrows[direction].source = { 0, 388, 149, 146 };   break; // DOWN CONFIRM
        case 2: arrows[direction].source = { 1850, 308, 153, 150 }; break; // UP CONFIRM
        case 3: arrows[direction].source = { 784, 385, 148, 151 };  break; // RIGHT CONFIRM
    }
}

void StrumLine::Draw() {
    for (int i = 0; i < 4; i++) {
        Rectangle dest = { 
            arrows[i].position.x, 
            arrows[i].position.y, 
            arrows[i].source.width * arrows[i].scale, 
            arrows[i].source.height * arrows[i].scale 
        };
        
        Vector2 origin = { 0, 0 }; 
        DrawTexturePro(texture, arrows[i].source, dest, origin, 0.0f, WHITE);
    }
}