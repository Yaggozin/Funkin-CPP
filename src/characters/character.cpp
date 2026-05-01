#include "character.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
using namespace json11;

AnimID GetIDFromName(std::string name) {
    if (name == "idle") return ANIM_IDLE;
    if (name == "singLEFT") return ANIM_LEFT;
    if (name == "singRIGHT") return ANIM_RIGHT;
    if (name == "singUP") return ANIM_UP;
    if (name == "singDOWN") return ANIM_DOWN;
    if (name == "singLEFTmiss") return ANIM_LEFT_MISS;
    if (name == "singRIGHTmiss") return ANIM_RIGHT_MISS;
    if (name == "singUPmiss") return ANIM_UP_MISS;
    if (name == "singDOWNmiss") return ANIM_DOWN_MISS;
    if (name == "hey") return ANIM_HEY;
    return ANIM_IDLE;
}

Character::Character(std::string charName, float x, float y) {
    position = { x, y };
    std::string jsonPath = "resources/data/characters/" + charName + ".json";
    LoadFromJSON(jsonPath);
}

Character::~Character() {
    UnloadTexture(spriteSheet);
}

void Character::LoadFromJSON(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "ERRO: Ficheiro JSON não encontrado: " << path << std::endl;
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    std::string err;      
    json11::Json data = json11::Json::parse(content, err);
    if (!err.empty()) {
        std::cout << "Erro ao ler JSON: " << err << std::endl;
        return;
    }

    std::string asset = data["assetPath"].string_value();
    std::string finalImgPath = "resources/images/characters/" + asset + ".png";
    std::string finalXmlPath = "resources/images/characters/" + asset + ".xml";

    this->flipX = data["flipX"].bool_value();
    
    if (data["offsets"].is_array()) {
        this->globalOffset.x = (float)data["offsets"][0].number_value();
        this->globalOffset.y = (float)data["offsets"][1].number_value();
    }

    this->spriteSheet = LoadTexture(finalImgPath.c_str());

    for (auto &anim : data["animations"].array_items()) {
        std::string animName = anim["name"].string_value();
        std::string prefix = anim["prefix"].string_value();
        this->LoadXML(finalXmlPath.c_str(), prefix.c_str(), GetIDFromName(animName), animName);
    }

    if (!anims.empty()) PlayAnimByID(ANIM_IDLE);
}

void Character::LoadXML(const char* xmlPath, const char* xmlName, AnimID id, std::string realName) {
    std::ifstream file(xmlPath); 
    
    if (!file.is_open()) {
        std::cout << "ERRO: XML não encontrado em: " << xmlPath << std::endl;
        return;
    }

    Animation newAnim;
    newAnim.id = id;
    newAnim.name = realName;

    std::string line;
    while (std::getline(file, line)) {
        std::string searchName = "name=\"" + std::string(xmlName); 
        size_t namePos = line.find(searchName);

        if (line.find("<SubTexture") != std::string::npos && namePos != std::string::npos) {
            
            std::string remainder = line.substr(namePos + searchName.length());

            if (remainder.find("MISS") == 0 || remainder.find(" MISS") == 0) {
                continue; 
            }

            Rectangle r;
            float fx = 0, fy = 0;
            
            auto getVal = [&](std::string key) {
                size_t pos = line.find(key + "=\"");
                if (pos == std::string::npos) return 0.0f;
                return std::stof(line.substr(pos + key.length() + 2));
            };

            r.x = getVal("x");
            r.y = getVal("y");
            r.width = getVal("width");
            r.height = getVal("height");
            fx = getVal("frameX");
            fy = getVal("frameY");

            newAnim.frames.push_back(r);
            newAnim.frameOffsets.push_back({fx, fy});
        }
    }
    anims.push_back(newAnim);
    file.close();
}

void Character::Update() {
    if (anims.empty()) return;
    float dt = GetFrameTime();

    if (holdTimer > 0) {
        holdTimer -= dt;
    } 
    
    if (holdTimer <= 0) {
        if (anims[curAnim].name != "idle" && 
            anims[curAnim].name != "danceLeft" && 
            anims[curAnim].name != "danceRight") {
            PlayAnim("idle");
        }
    }

    if (!isGF) {
        if (holdTimer <= 0) {
            PlayAnim("idle");
        }
    }

    Animation& a = anims[curAnim];
    if (a.frames.size() > 1) {
        frameTimer += dt;
        if (frameTimer >= 0.041f) { // 24 FPS
            frameTimer = 0;
            
            if (a.name.find("sing") != std::string::npos) {
                if (curFrame < (int)a.frames.size() - 1) {
                    curFrame++;
                }
            } else {
                curFrame++;
                if (curFrame >= (int)a.frames.size()) {
                    curFrame = 0;
                }
            }
        }
    }
}

void Character::PlayAnimByID(AnimID id, bool forced) {
    if (anims.empty()) return;
    for (int i = 0; i < (int)anims.size(); i++) {
        if (anims[i].id == id) {
            if (forced || curAnim != i) {
                curAnim = i;
                curFrame = 0;
                frameTimer = 0;
            }
            break;
        }
    }
}

void Character::PlayAnim(std::string animName, bool forced) {
    if (anims.empty()) return;

    for (int i = 0; i < (int)anims.size(); i++) {
        if (anims[i].name == animName) {
            if (forced || curAnim != i) {
                curAnim = i;
                curFrame = 0;
                frameTimer = 0;
                
                if (animName.find("sing") != std::string::npos) {
                    holdTimer = 0.4f; 
                }
            }
            break;
        }
    }
}

void Character::Draw() {
    if (anims.empty()) {
        DrawTextureEx(spriteSheet, position, 0.0f, 0.2f, WHITE);
        return;
    }

    Animation& a = anims[curAnim];
    Rectangle source = a.frames[curFrame];
    Vector2 offset = a.frameOffsets[curFrame];

    Vector2 drawPos = { position.x - offset.x, position.y - offset.y };
    DrawTextureRec(spriteSheet, source, drawPos, WHITE);
}