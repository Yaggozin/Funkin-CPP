#include "paths.hpp"

std::string Paths::image(std::string key) {
    return "resources/images/" + key + ".png";
}

std::string Paths::music(std::string key) {
    return "resources/music/" + key + ".ogg";
}

std::string Paths::sound(std::string key) {
    return "resources/sounds/" + key + ".ogg";
}

std::string Paths::json(std::string key) {
    return "resources/data/" + key + ".json";
}

std::string Paths::font(std::string key) {
    return "resources/fonts/" + key + ".ttf";
}

/*std::string Paths::character(std::string key) {
    return "resources/data/characters/" + key + ".json";
}

std::string Paths::stage(std::string key) {
    return "resources/images/stages/" + key + ".png";
}

std::string Paths::song(std::string songName, std::string file) {
    return "resources/songs/" + songName + "/" + file + ".ogg";
}*/