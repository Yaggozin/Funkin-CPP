#ifndef STAGE_HPP
#define STAGE_HPP

#include "raylib.h"
#include <vector>
#include <string>

struct StageLayer {
    Texture2D texture;
    Vector2 position;
    float scrollFactor;
    float scale;
};

class Stage {
public:
    std::vector<StageLayer> layers;
    Color backgroundColor;

    Stage(std::string stageName = "default");
    ~Stage();

    void AddLayer(std::string fileName, Vector2 pos, float scroll, float scale);
    void Draw(Camera2D cam);
};

#endif