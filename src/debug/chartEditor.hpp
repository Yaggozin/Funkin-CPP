#ifndef CHARTEDITOR_HPP
#define CHARTEDITOR_HPP

#include "raylib.h"
#include "core/playstate.hpp"
#include "ui/editorUI.hpp"
#include <vector>
#include <string>

class ChartEditor {
public:
    ChartEditor();
    ~ChartEditor();

    void Update(float dt);
    void Draw();
    void LoadJSON(std::string songName);

private:
    EditorWindow songWin = { { 300, 60, 500, 585 }, "Song Metadata" }; // x initial, y initial, width, height

    // Câmeras
    Camera2D gridCam;
    Camera2D uiCam;

    // Dados do Chart
    std::vector<ChartNote> tempNotes;
    std::vector<Section> tempSections;
    // Variáveis de Controle
    float songPosition = 0.0f;
    float bpm = 100.0f;
    float stepCrochet = 150.0f;
    float scrollSpeed = 1.0f;
    bool paused = true;
    Music inst;
    Music voices;
    bool musicLoaded = false;
    float songLength = 0.0f; // em ms
    float GetStepFromTime(float time);
    float GetYFromTime(float time);
    bool visible = true;
    

    // UI Helper
    void DrawGrid();
    void DrawNotes();
    void DrawUI();
};

#endif