#ifndef PLAYSTATE_HPP
#define PLAYSTATE_HPP

#include "raylib.h"
#include <vector>
#include <string>
#include "characters/character.hpp"
#include "rhythm/conductor.hpp"
#include "stages/stage.hpp"
#include "ui/strumline.hpp"
#include "ui/health.hpp"

struct ChartNote {
    float strumTime;
    int noteData;
    float sustainLength;
    bool mustHit;
    bool active;
    Rectangle source;
};

struct Section {
    bool mustHitSection;
};

class PlayState {
public:

    PlayState();
    ~PlayState();

    void Update(); // Update
    void Draw(); // Draw all in screen
    void DrawHUD(); // Draw HUD
    void LoadSong(std::string songName);
    void BeatHit(int beat); // Bop
    void HandleGlobalInput();
    void UpdateMusic(float dt);
    void UpdateInput();
    void UpdateNotes();
    void UpdateHUD();
    void OnBeat(int beat);
    void DrawNotes();
    void HandleHit(ChartNote& note);
    void HandleMiss(int direction);
    void Countdown(float dt);
    void Ending(float dt);
    bool CanToggleScroll();

    // Stage
    Stage* stage;

    // Characters
    Character* player; // Player
    Character* opponent; // Opponent
    Character* gf; // Music Box

    // StrumLines
    StrumLine* playerStrums;
    StrumLine* enemyStrums;

    // HealthBar
    Texture2D healthBarBG;
    Texture2D iconPlayer;
    Texture2D iconEnemy;
    Font scoreFont;
    Health* healthBar;

    Texture2D noteAssets;
    Texture2D countdownReady, countdownSet, countdownGo;

    // Camera
    Camera2D camGame; // For Characters, Stage.
    Camera2D camHUD; // For Score text, HealthBar.
    Camera2D camUpper; // For StrumLines, Notes.
    Vector2 camGameTarget;

    // Rhythm & Song Data
    Conductor* conductor;
    std::vector<ChartNote> songNotes;
    std::vector<Section> songSections;
    
    float scrollSpeed = 2.4f;
    float health = 1.0f;
    int combo = 0;
    int score = 0;
    int lastBeat = -1;
    int currentTurn = 0; // 0 = BF, 1 = Dad
    bool paused = false;
    bool hitThisFrame = false;
    bool isGF = false;
    std::string currentSong;


    // float camSpeed = 0.08f;
    float defaultZoom = 1.05f;


    // Intro & Ending Logic
    bool skipIntro = false;
    bool startingSong = true;
    bool isEnding = false;
    float startTimer = 0.0f;
    int countdownLevel = 3;
    float fadeAlpha = 0.0f;

    bool isDownscroll = false;
    float scrollLerp = 0.0f;
    float scrollTarget = 0.0f;

    // Audio
    Music inst;
    Music voices;
    bool musicLoaded = false;
    Sound introThree, introTwo, introOne, introGo;

private:
    void UpdateCamera(float dt);
    int GetKeyFromDirection(int dir);
    int GetAnimFromDirection(int dir);
    int GetMissAnimFromDirection(int dir);
};

#endif