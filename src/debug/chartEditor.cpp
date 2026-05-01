#include "chartEditor.hpp"
#include "ui/editorUI.hpp"
#include "core/constants.hpp"
#include "json11.hpp"
#include "raymath.h"
#include <iostream>

bool justResumed = false;
float visualProgress = 0.0f;

float ChartEditor::GetStepFromTime(float time) {
    return time / stepCrochet;
}

float ChartEditor::GetYFromTime(float time) {
    return GetStepFromTime(time) * Constants::GRID_SIZE;
}

bool windowMenuOpen = false;

ChartEditor::ChartEditor() {
    // Grid
    gridCam.target = { 0, 0 };
    gridCam.offset = { (float)GetScreenWidth() / 2.0f - (Constants::GRID_SIZE * 4), 100.0f };
    gridCam.rotation = 0.0f;
    gridCam.zoom = 1.0f;

    uiCam = { {0,0}, {0,0}, 0.0f, 1.0f };

    LoadJSON("bopeebo");
}

ChartEditor::~ChartEditor() {}

void ChartEditor::LoadJSON(std::string songName) {
    std::string path = "resources/songs/" + songName + "/" + songName + ".json";
    char* fileText = LoadFileText(path.c_str());

    if (fileText == nullptr) {
        std::cout << "Falha ao carregar: " << path << std::endl;
        return;
    }

    std::string err;
    auto json = json11::Json::parse(fileText, err);
    UnloadFileText(fileText);

    if (!err.empty()) {
        std::cout << "JSON parse error: " << err << std::endl;
        return;
    }

    auto song = json["song"];
    bpm = (float)song["bpm"].number_value();
    stepCrochet = ((60.0f / bpm) * 1000.0f) / 4.0f;
    
    tempNotes.clear();
    tempSections.clear();

    for (const auto &section : song["notes"].array_items()) {
        Section sec;
        sec.mustHitSection = section["mustHitSection"].bool_value();
        tempSections.push_back(sec);

        for (const auto &nData : section["sectionNotes"].array_items()) {
            ChartNote n;
            n.strumTime = (float)nData[0].number_value();
            int rawData = nData[1].int_value();
            n.mustHit = (rawData > 3) ? !sec.mustHitSection : sec.mustHitSection;
            n.noteData = rawData % 4;
            tempNotes.push_back(n);
        }
    }

    // Load Audios
    std::string instPath = "resources/songs/" + songName + "/Inst.ogg";
    std::string voicesPath = "resources/songs/" + songName + "/Voices.ogg";
    
    inst = LoadMusicStream(instPath.c_str());
    voices = LoadMusicStream(voicesPath.c_str());
    musicLoaded = true;
    // Audio load check
    if (inst.stream.buffer == nullptr) {
        std::cout << "Failed to load Inst\n";
    }
    if (voices.stream.buffer == nullptr) {
        std::cout << "Failed to load Voices\n";
    }
    
    StopMusicStream(inst);
    StopMusicStream(voices);
    PlayMusicStream(inst);
    PlayMusicStream(voices);
    PauseMusicStream(inst);
    PauseMusicStream(voices);
    songPosition = 0;
    gridCam.target.y = 0;
}

void ChartEditor::Update(float dt) {
    gridCam.offset.x = (GetScreenWidth() / 2.0f) - (Constants::GRID_SIZE * 4);

    if (musicLoaded) {
        UpdateMusicStream(inst);
        UpdateMusicStream(voices);
    }

    if (IsKeyPressed(KEY_SPACE)) {
        paused = !paused;
        if (paused) {
            PauseMusicStream(inst);
            PauseMusicStream(voices);
        } else {
            float targetTime = songPosition / 1000.0f;
            SeekMusicStream(inst, targetTime);
            SeekMusicStream(voices, targetTime);
            ResumeMusicStream(inst);
            ResumeMusicStream(voices);

            justResumed = true;
        }
    }

    if (!paused && musicLoaded) {

        if (justResumed) {
            // Skip 1 frame to let audio sync
            justResumed = false;
        } else {
            float musicTime = GetMusicTimePlayed(inst) * 1000.0f;
            if (musicTime > 0) {
                songPosition = musicTime;
            }
        }

    } else {
        // Scroll when paused
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {

            float scrollAmount = stepCrochet; // base = 1 step

            // Fast scroll (CTRL)
            if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
                scrollAmount *= 4; // 4x faster
            }

            // Section snap (SHIFT)
            if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
                scrollAmount = stepCrochet * 16; // jump 1 section
            }

            songPosition -= wheel * scrollAmount;

            if (songPosition < 0) songPosition = 0;
        }
    }

    gridCam.target.y = (songPosition / stepCrochet) * Constants::GRID_SIZE;
}

void ChartEditor::Draw() {
    ClearBackground({ 90, 51, 160, 255 });
    BeginMode2D(gridCam);
        DrawGrid();
        DrawNotes();
        
        // A LINHA INDICADORA
        float lineY = GetYFromTime(songPosition);
        DrawRectangle(0, lineY, Constants::GRID_SIZE * 8, 2, RED);
    EndMode2D();

    DrawUI();
}

void ChartEditor::DrawGrid() {
    int totalSteps = tempSections.size() * 16;

    // Only draw visible area
    int start = (int)(gridCam.target.y / Constants::GRID_SIZE) - 20;
    int end = start + 40;

    if (start < 0) start = 0;
    if (end > totalSteps) end = totalSteps;

    for (int i = start; i < end; i++) {
        
        // Section start line (every 16 steps)
        if (i % 16 == 0 && i != 0) {
            DrawRectangle(-2, i * Constants::GRID_SIZE - 2, (Constants::GRID_SIZE * 8) + 4, 4, BLACK);
        }

        // Beat line (every 4 steps)
        if (i % 4 == 0 && i % 16 != 0) {
            DrawRectangle(0, i * Constants::GRID_SIZE - 1,
                        Constants::GRID_SIZE * 8, 1, DARKGRAY);
        }

        for (int j = 0; j < 8; j++) {
            // Checker Pattern
            Color cellColor = ((i + j) % 2 == 0) ? Constants::GRID_COLOR_1 : Constants::GRID_COLOR_2;
            /*if (i % 16 == 0) {
                cellColor = { 210, 210, 210, 255 }; 
            }*/
            DrawRectangle(j * Constants::GRID_SIZE, i * Constants::GRID_SIZE, 
                          Constants::GRID_SIZE, Constants::GRID_SIZE, cellColor);
            DrawRectangleLines(j * Constants::GRID_SIZE, i * Constants::GRID_SIZE, 
                               Constants::GRID_SIZE, Constants::GRID_SIZE, Constants::GRID_BORDER);
        }
    }

    // Mid split (Player/Opponent)
    DrawRectangle(4 * Constants::GRID_SIZE - 2, 0, 4, totalSteps * Constants::GRID_SIZE, BLACK);
    
    // Side borders
    DrawRectangle(-2, 0, 2, totalSteps * Constants::GRID_SIZE, BLACK); // Left
    DrawRectangle(8 * Constants::GRID_SIZE, 0, 2, totalSteps * Constants::GRID_SIZE, BLACK); // Right
}

void ChartEditor::DrawNotes() {
    for (const auto& note : tempNotes) {
        float visualY = GetYFromTime(note.strumTime);
        int visualX = note.noteData * Constants::GRID_SIZE;
        if (note.mustHit) visualX += (4 * Constants::GRID_SIZE);

        Color nColor = WHITE;
        switch(note.noteData) {
            case 0: nColor = Constants::EDITOR_NOTE_LEFT; break;
            case 1: nColor = Constants::EDITOR_NOTE_DOWN; break;
            case 2: nColor = Constants::EDITOR_NOTE_UP; break;
            case 3: nColor = Constants::EDITOR_NOTE_RIGHT; break;
        }

        DrawRectangle(visualX + 2, visualY + 2, Constants::GRID_SIZE - 4, Constants::GRID_SIZE - 4, nColor);
    }
}

void ChartEditor::DrawUI() {
BeginMode2D(uiCam);

        /*
        *   Upbar
        */

        DrawRectangle(0, 0, GetScreenWidth(), 40, { 0, 0, 0, 220 });

        float menuX = 10;
        float menuY = 5;
        float spacing = 60;

        // Menu labels
        const char* menus[] = { "File", "Edit", "View", "Audio", "Test", "Window", "Help" };

        for (int i = 0; i < 7; i++) {
            Rectangle item = { menuX + (i * spacing), menuY, 50, 20 };

            bool clicked = EditorUI::Button(item, menus[i], 14, Color{40,40,40,255}, WHITE);

            // Only care about WINDOW menu
            if (i == 5 && clicked) {
                windowMenuOpen = !windowMenuOpen;
            }
        }

        if (windowMenuOpen) {

            Rectangle dropdown = {
                10 + (5 * 60), // posição do "Window"
                30,
                180,
                100
            };

            DrawRectangleRec(dropdown, Color{35,35,35,255});
            DrawRectangleLinesEx(dropdown, 1, BLACK);

            float itemY = dropdown.y + 5;

            // ===== WINDOW LIST =====

            // Exemplo com 1 janela (tu pode adicionar mais depois)
            EditorWindow* windows[] = { &songWin };
            int windowCount = 1;

            for (int i = 0; i < windowCount; i++) {
                EditorWindow* w = windows[i];

                Rectangle item = {
                    dropdown.x + 5,
                    itemY,
                    dropdown.width - 10,
                    20
                };

                // Toggle
                if (EditorUI::Button(item, w->title.c_str(), 14, Color{50,50,50,255}, WHITE)) {
                    w->visible = !w->visible;
                }

                // Checkmark
                if (w->visible) {
                    EditorUI::DrawText("✔", { item.x + item.width - 20, item.y + 2 }, 14, GREEN);
                }

                itemY += 25;
            }
        }
        if (windowMenuOpen && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();

            Rectangle dropdown = {
                10 + (5 * 60),
                30,
                180,
                100
            };

            if (!CheckCollisionPointRec(m, dropdown)) {
                windowMenuOpen = false;
            }
        }

        /*DrawRectangle(0, 0, GetScreenWidth(), 40, { 0, 0, 0, 220 });
        EditorUI::DrawText(
            TextFormat("EDITOR - %s | BPM: %.1f | TIME: %.2fs", paused ? "PAUSED" : "PLAYING", bpm, songPosition/1000.0f), 
            { 20, 10 }, 20, WHITE
        );*/

        /*
        *   TimeLine
        */

        float songLength = GetMusicTimeLength(inst) * 1000.0f;
        float targetProgress = (songLength > 0) ? (songPosition / songLength) : 0;

        // Smooth (lerp)
        visualProgress += (targetProgress - visualProgress) * 0.2f;

        // Timeline bar
        float timelineHeight = 14;
        Rectangle timeline = { 
            20, 
            (float)GetScreenHeight() - timelineHeight - 20.0f, 
            (float)GetScreenWidth() - 40.0f,
            timelineHeight 
        };

        // Background
        DrawRectangleRec(timeline, Color{30, 30, 30, 255});

        // Border
        DrawRectangleLinesEx(timeline, 1, BLACK);

        // Progress
        // float progress = (songLength > 0) ? (songPosition / songLength) : 0;

        Rectangle fill = {
            timeline.x,
            timeline.y,
            timeline.width * visualProgress,
            timeline.height
        };
        DrawRectangleRec(fill, RED);

        // Handle
        float handleX = timeline.x + (timeline.width * visualProgress);
        DrawCircle(handleX, timeline.y + timeline.height / 2, 6, WHITE);

        // Time
        EditorUI::DrawText(
            TextFormat("%.2fs / %.2fs", songPosition/1000.0f, songLength/1000.0f),
            { timeline.x, timeline.y - 20 },
            16,
            WHITE
        );

        // Timeline click
        Vector2 mouse = GetMousePosition();

        bool hoveringTimeline = CheckCollisionPointRec(mouse, timeline);

        if (hoveringTimeline && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {

            float percent = (mouse.x - timeline.x) / timeline.width;

            if (percent < 0) percent = 0;
            if (percent > 1) percent = 1;

            songPosition = percent * songLength;

            float targetTime = songPosition / 1000.0f;
            SeekMusicStream(inst, targetTime);
            SeekMusicStream(voices, targetTime);

            justResumed = true;
        }

        /*
        *   Song Metadata Window
        */

        // Window
        EditorUI::DrawWindow(songWin);

        // Base content area
        float contentX = songWin.rect.x + 15;
        float contentY = songWin.rect.y + 65;
        float contentW = songWin.rect.width - 30;

        // ===== SECTION: INFO =====
        EditorUI::DrawSection({ contentX, contentY, contentW, 80 }, "Info");

        EditorUI::DrawText(
            TextFormat("BPM: %.1f", bpm),
            { contentX + 10, contentY + 30 },
            16,
            LIGHTGRAY
        );

        EditorUI::DrawText(
            TextFormat("Step: %d", (int)(songPosition / stepCrochet)),
            { contentX + 10, contentY + 55 },
            16,
            LIGHTGRAY
        );

        // ===== SECTION: CONTROLS =====
        float controlsY = contentY + 100;

        EditorUI::DrawSection({ contentX, controlsY, contentW, 100 }, "Controls");

        // Buttons row
        float btnW = (contentW - 20) / 3;

        if (EditorUI::Button({ contentX + 5, controlsY + 40, btnW, 30 }, "Play", 16)) {
            paused = false;
        }

        if (EditorUI::Button({ contentX + 10 + btnW, controlsY + 40, btnW, 30 }, "Pause", 16)) {
            paused = true;
        }

        if (EditorUI::Button({ contentX + 15 + (btnW * 2), controlsY + 40, btnW, 30 }, "Reload", 16)) {
            LoadJSON("bopeebo");
        }

        // ===== SECTION: SLIDER TEST =====
        float sliderY = controlsY + 120;

        EditorUI::DrawSection({ contentX, sliderY, contentW, 70 }, "Instrumental");

        // fake value só pra teste
        float fakeValue = fmod(songPosition / 1000.0f, 1.0f);

        EditorUI::DrawSlider(
            { contentX + 10, sliderY + 35, contentW - 20, 10 },
            fakeValue
        );
    EndMode2D();
}