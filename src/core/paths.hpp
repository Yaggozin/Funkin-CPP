#ifndef PATHS_HPP
#define PATHS_HPP

#include <string>

class Paths {
public:
    static std::string image(std::string key);
    static std::string music(std::string key);
    static std::string sound(std::string key);
    static std::string json(std::string key);
    static std::string font(std::string key);
    /*
    static std::string character(std::string key);
    static std::string stage(std::string key);
    static std::string song(std::string songName, std::string file);*/
};

#endif