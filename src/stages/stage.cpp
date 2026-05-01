#include "stage.hpp"
#include <iostream>

Stage::Stage(std::string stageName) {
    backgroundColor = BLACK;

    if (stageName == "default") {
        backgroundColor = LIGHTGRAY;
    } 
    else if (stageName == "stage") {
        backgroundColor = LIGHTGRAY;
        // 1. STAGE BACK
        // AddLayer("stageback.png", {-600, -200}, 0.9f, 1.0f);
        // 2. STAGE FRONT
        AddLayer("stagefront.png", {-650, 600}, 0.9f, 1.1f);
        // 3. STAGE CURTAINS
        // AddLayer("stagecurtains.png", {-500, -300}, 1.3f, 0.9f);
    }
}

Stage::~Stage() {
    // CleanUp
    for (auto& layer : layers) {
        UnloadTexture(layer.texture);
    }
    layers.clear();
}

void Stage::AddLayer(std::string fileName, Vector2 pos, float scroll, float scale) {
    std::string path = "resources/images/stages/" + fileName;
    Texture2D tex = LoadTexture(path.c_str());
    
    if (tex.id == 0) {
        std::cout << "WARNING: No Load: " << path << std::endl;
        return;
    }

    layers.push_back({tex, pos, scroll, scale});
}

void Stage::Draw(Camera2D cam) {
    ClearBackground(backgroundColor);

    for (const auto& layer : layers) {
        // Scroll Factor (Parallax)
        Vector2 drawPos = {
            layer.position.x - (cam.target.x * (layer.scrollFactor - 1.0f)),
            layer.position.y - (cam.target.y * (layer.scrollFactor - 1.0f))
        };

        DrawTextureEx(layer.texture, drawPos, 0.0f, layer.scale, WHITE);
    }
}