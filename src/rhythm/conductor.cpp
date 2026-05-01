#include "conductor.hpp"

Conductor::Conductor(float bpm) {
    songPosition = 0;
    lastSongPos = 0;
    curBeat = 0;
    curStep = 0;
    MapBPM(bpm);
}

void Conductor::MapBPM(float newBpm) {
    bpm = newBpm;
    crochet = (60.0f / bpm) * 1000.0f;
    stepCrochet = crochet / 4.0f;
}

void Conductor::Update(float musicTimeSeconds) {
    // Raylib dá o tempo em segundos, FNF usa milissegundos
    songPosition = musicTimeSeconds * 1000.0f;
    
    curStep = (int)(songPosition / stepCrochet);
    curBeat = (int)(songPosition / crochet);
}