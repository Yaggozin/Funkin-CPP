#ifndef CONDUCTOR_HPP
#define CONDUCTOR_HPP

#include "raylib.h"

class Conductor {
public:
    float bpm;
    float crochet;       // Duração de um Beat em ms
    float stepCrochet;   // Duração de um Step (1/4 de beat) em ms
    float songPosition;  // Tempo atual da música em ms
    float lastSongPos;
    
    int curBeat;
    int curStep;

    Conductor(float bpm = 100.0f);
    
    // Atualiza a posição da música e calcula beat/step
    void Update(float musicTimeSeconds);
    
    // Atalho para mudar o BPM e recalcular os crochets
    void MapBPM(float newBpm);
};

#endif