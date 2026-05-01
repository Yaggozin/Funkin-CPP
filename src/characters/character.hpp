#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "raylib.h"
#include <vector>
#include <string>
// #include <nlohmann/json.hpp>
// using json = nlohmann::json;
#include "json11.hpp"

enum AnimID {
    ANIM_LEFT = 0,   // Agora bate com noteData 0
    ANIM_DOWN = 1,   // Agora bate com noteData 1
    ANIM_UP = 2,     // Agora bate com noteData 2
    ANIM_RIGHT = 3,  // Agora bate com noteData 3
    ANIM_IDLE,       // O Idle fica depois
    ANIM_UP_MISS, 
    ANIM_DOWN_MISS, 
    ANIM_LEFT_MISS, 
    ANIM_RIGHT_MISS,
    ANIM_HEY, 
    ANIM_DANCE_LEFT, 
    ANIM_DANCE_RIGHT
};

struct Animation {
    std::string name;
    AnimID id;
    std::vector<Rectangle> frames;
    std::vector<Vector2> frameOffsets;
};

AnimID GetIDFromName(std::string name);

class Character {
public:
    Vector2 position;
    Texture2D spriteSheet;
    bool flipX = false;
    Vector2 globalOffset = {0, 0};
    Vector2 cameraOffset = {0, 0};
    
    std::vector<Animation> anims;
    int curAnim = 0;
    int curFrame = 0;
    float frameTimer = 0.0f;
    float holdTimer = 0.0f;
    bool isGF = false;

    Character(std::string charName, float x, float y);
    ~Character();

    void LoadFromJSON(std::string path);
    void LoadXML(const char* xmlPath, const char* xmlName, AnimID id, std::string realName);
    void Update();
    void PlayAnim(std::string animName, bool forced = false);

    void PlayAnimByID(AnimID id, bool forced = false);
    void Draw();
};

#endif