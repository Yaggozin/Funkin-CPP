#include "playstate.hpp"
#include "core/paths.hpp"
#include "constants.hpp"
#include "json11.hpp"
#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "raymath.h"

static std::string singAnims[] = {"singLEFT", "singDOWN", "singUP", "singRIGHT"};
static std::string missAnims[] = {"singLEFTmiss", "singDOWNmiss", "singUPmiss", "singRIGHTmiss"};

static Vector2 bfCameraOffset = { 150, 100 }; // Boyfriend Camera Offset
// static Vector2 gfCameraOffset = { 250, 200 }; // Girlfriend Camera Offset
static Vector2 dadCameraOffset = { 250, 200 }; // Dad Camera Offset

static const float camSpeed = 0.04f;

enum GameState { PLAYING, CHART_EDITOR }; 
extern GameState currentState;

PlayState::PlayState() {

    // Music
    std::string songName = "bopeebo"; 
    std::string jsonPath = Paths::json(songName);
    currentSong = songName;

    char* fileText = LoadFileText(jsonPath.c_str());
    float songBpm = 100.0f; // DEFAULT BPM

    if (fileText != nullptr) {
        std::string err;
        auto json = json11::Json::parse(fileText, err);

        if (err.empty()) {
            if (json["song"].is_object()) {
                songBpm = (float)json["song"]["bpm"].number_value();
                TraceLog(LOG_INFO, "BPM do arquivo JSON carregado: %f", songBpm);
            }
        } else {
            TraceLog(LOG_ERROR, "Erro ao ler JSON: %s", err.c_str());
        }
        
        UnloadFileText(fileText);
    }
    conductor = new Conductor(songBpm);
    LoadSong(songName);

    // Stages
    stage = new Stage("default");
    
    // Characters
    player = new Character("Boyfriend", 770, 450); // Create Player
    opponent = new Character("Daddy", 100, 100); // Create Opponent
    gf = new Character("Girlfriend", 400, 130); // Create Music Box
    gf->isGF = true;

    // StrumLines
    float screenW = (float)GetScreenWidth();
    float center = screenW / 2.0f;
    float groupWidth = 448.0f;
    enemyStrums = new StrumLine((center / 2.0f) - (groupWidth / 2.0f), false);
    playerStrums = new StrumLine(center + (center / 2.0f) - (groupWidth / 2.0f), true);

    noteAssets = LoadTexture(Paths::image("ui/notes").c_str());
    countdownReady = LoadTexture(Paths::image("ui/ready").c_str());
    countdownSet = LoadTexture(Paths::image("ui/set").c_str());
    countdownGo = LoadTexture(Paths::image("ui/go").c_str());

    introThree = LoadSound(Paths::sound("ui/setgo/intro3").c_str());
    introTwo = LoadSound(Paths::sound("ui/setgo/intro2").c_str());
    introOne = LoadSound(Paths::sound("ui/setgo/intro1").c_str());
    introGo = LoadSound(Paths::sound("ui/setgo/introGo").c_str());

    healthBarBG = LoadTexture(Paths::image("ui/healthBar").c_str());
    iconPlayer  = LoadTexture(Paths::image("ui/icons/icon-bf").c_str());
    iconEnemy   = LoadTexture(Paths::image("ui/icons/icon-dad").c_str());
    scoreFont   = LoadFont(Paths::font("vcr").c_str());

    float barX = (GetScreenWidth() / 2.0f) - 300;
    healthBar = new Health(barX, GetScreenHeight() - 100, healthBarBG, iconPlayer, iconEnemy, scoreFont);

    // Game Camera
    camGame.target = { player->position.x, player->position.y };
    camGame.offset = { GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    camGame.rotation = 0.0f;
    camGame.zoom = 1.0f;

    // Strums Camera
    camUpper.target = { 0, 0 };
    camUpper.offset = { 0, 0 };
    camUpper.rotation = 0.0f;
    camUpper.zoom = 1.0f;

    // HUD Camera
    camHUD.target = { 0, 0 };
    camHUD.offset = { 0, 0 };
    camHUD.rotation = 0.0f;
    camHUD.zoom = 1.0f;

    camUpper.target = { 0, 0 }; camUpper.offset = { 0, 0 }; camUpper.rotation = 0.0f; camUpper.zoom = 1.0f;
    camHUD.target = { 0, 0 }; camHUD.offset = { 0, 0 }; camHUD.rotation = 0.0f; camHUD.zoom = 1.0f;
    camGameTarget = { player->position.x, player->position.y};
}

PlayState::~PlayState() {
    delete stage;
    delete player; // Delete Boyfriend
    delete opponent; // Delete Daddy
    delete gf; // Delete Girlfriend
    delete playerStrums; // Delete Player Strums
    delete enemyStrums; // Delete Enemy Strums
    delete healthBar; // Delete HealthBar and Score Text
    delete conductor; // Delete Conductor

    UnloadTexture(healthBarBG);
    UnloadTexture(iconPlayer);
    UnloadTexture(iconEnemy);
    UnloadFont(scoreFont);

    UnloadTexture(noteAssets);
    UnloadTexture(countdownReady); UnloadTexture(countdownSet); UnloadTexture(countdownGo);
    UnloadSound(introThree); UnloadSound(introTwo); UnloadSound(introOne); UnloadSound(introGo);

    if (musicLoaded) {
        UnloadMusicStream(inst);
        UnloadMusicStream(voices);
    }
}

int PlayState::GetKeyFromDirection(int dir) {
    switch (dir) {
        case 0:  return KEY_LEFT;
        case 1:  return KEY_DOWN;
        case 2:  return KEY_UP;
        case 3:  return KEY_RIGHT;
        default: return 0;
    }
}
int PlayState::GetAnimFromDirection(int dir) {
    if (dir == 0) return 3;
    if (dir == 1) return 2; 
    if (dir == 2) return 1; 
    if (dir == 3) return 4; 
    return 0; // ANIM_IDLE
}
int PlayState::GetMissAnimFromDirection(int dir) {
    if (dir == 0) return 5;
    if (dir == 1) return 6;
    if (dir == 2) return 7;
    if (dir == 3) return 8;
    return 5;
}

void PlayState::LoadSong(std::string songName) {
    songNotes.clear();
    songSections.clear();

    // Load Audio
    std::string instPath = "resources/songs/" + songName + "/Inst.ogg";
    std::string voicesPath = "resources/songs/" + songName + "/Voices.ogg";
    
    inst = LoadMusicStream(instPath.c_str());
    voices = LoadMusicStream(voicesPath.c_str());
    
    inst.looping = false; 
    voices.looping = false;
    musicLoaded = true;

    // Load JSON
    std::string jsonPath = "resources/songs/" + songName + "/" + songName + ".json";
    char* rawJson = LoadFileText(jsonPath.c_str());
    
    if (rawJson == nullptr) {
        std::cout << "ERRO: Chart não encontrado em " << jsonPath << std::endl;
        return;
    }

    std::string err;
    json11::Json json = json11::Json::parse(rawJson, err);
    UnloadFileText(rawJson);

    if (!err.empty()) {
        std::cout << "ERRO de Parse no JSON: " << err << std::endl;
        return;
    }

    auto song = json["song"];
    conductor->bpm = (float)song["bpm"].number_value();
    conductor->crochet = (60.0f / conductor->bpm) * 1000.0f;
    conductor->stepCrochet = conductor->crochet / 4.0f;
    
    if (song["speed"].is_number()) {
        scrollSpeed = (float)song["speed"].number_value();
    }

    if (song["notes"].is_array()) {
        for (const auto& section : song["notes"].array_items()) {
            bool isMustHitSection = section["mustHitSection"].bool_value();
            songSections.push_back({ isMustHitSection });

            if (section["sectionNotes"].is_array()) {
                for (const auto& noteData : section["sectionNotes"].array_items()) {
                    float time = (float)noteData[0].number_value();
                    int rawData = noteData[1].int_value();
                    
                    bool side = isMustHitSection;
                    if (rawData > 3) side = !side;

                    ChartNote newNote;
                    newNote.strumTime = time;
                    newNote.mustHit = side;
                    newNote.noteData = rawData % 4;
                    newNote.active = true;

                    int d = newNote.noteData;
                    if (d == 0) newNote.source = { 157, 0, 154, 157 };      // Esquerda
                    if (d == 1) newNote.source = { 0, 0, 157, 154 };        // Baixo
                    if (d == 2) newNote.source = { 154, 157, 157, 154 };    // Cima
                    if (d == 3) newNote.source = { 0, 154, 154, 157 };      // Direita

                    songNotes.push_back(newNote);
                }
            }
        }
    }

    std::sort(songNotes.begin(), songNotes.end(), [](const ChartNote& a, const ChartNote& b) {
        return a.strumTime < b.strumTime;
    });

    std::cout << "Song Loaded: " << songName << " | Notes: " << songNotes.size() << " | Sections: " << songSections.size() << std::endl;

    PlayMusicStream(inst);
    PlayMusicStream(voices);
    startingSong = false;
}

void PlayState::UpdateCamera(float dt) {
    int curSection = (int)(conductor->songPosition / (conductor->stepCrochet * 16));

    if (curSection >= 0 && curSection < (int)songSections.size()) {
        currentTurn = songSections[curSection].mustHitSection ? 0 : 1;
    }

    Vector2 targetPos;
    if (currentTurn == 0) { // Boyfriend
        targetPos.x = player->position.x + bfCameraOffset.x;
        targetPos.y = player->position.y + bfCameraOffset.y;
    } else { // Dad / Opponent
        targetPos.x = opponent->position.x + dadCameraOffset.x;
        targetPos.y = opponent->position.y + dadCameraOffset.y;
    }

    // Lerp
    float followLerp = 1.0f - powf(1.0f - camSpeed, dt * 60.0f);
    camGame.target.x = Lerp(camGame.target.x, targetPos.x, followLerp);
    camGame.target.y = Lerp(camGame.target.y, targetPos.y, followLerp);
    camGame.zoom += (defaultZoom - camGame.zoom) * dt * 3.0f;

    /*Vector2 targetPos;
    if (currentTurn == 0) { // Boyfriend
        targetPos.x = player->position.x + bfCameraOffset.x;
        targetPos.y = player->position.y + bfCameraOffset.y;
    } else { // Dad / Opponent
        targetPos.x = opponent->position.x + dadCameraOffset.x;
        targetPos.y = opponent->position.y + dadCameraOffset.y;
    }

    // Lerp
    camGame.target.x = Lerp(camGame.target.x, targetPos.x, camSpeed);
    camGame.target.y = Lerp(camGame.target.y, targetPos.y, camSpeed);
    camGame.zoom += (defaultZoom - camGame.zoom) * dt * 3.0f;*/

}

void PlayState::Update() {

    float dt = GetFrameTime();
    float sw = (float)GetScreenWidth(); float sh = (float)GetScreenHeight();

    // Smooth tween
    float speed = 5.0f; // quanto maior, mais rápido
    scrollLerp += (scrollTarget - scrollLerp) * GetFrameTime() * speed;

    float offsetX = (sw - 1280) / 2.0f;
    if (enemyStrums) {
        enemyStrums->xOffset = 100 + offsetX;
        enemyStrums->Update(dt);
    }
    if (playerStrums) {
        playerStrums->xOffset = 730 + offsetX;
        playerStrums->Update(dt);
    }

    float upY = 50;
    float downY = GetScreenHeight() - 150;

    float strumY = Lerp(upY, downY, scrollLerp);

    for (int i = 0; i < 4; i++) {
        enemyStrums->arrows[i].position.y = strumY;
        playerStrums->arrows[i].position.y = strumY;
    }

    /*int currentBeat = 0;
    if (conductor != nullptr && conductor->crochet > 0) {
        currentBeat = (int)floor(conductor->songPosition / conductor->crochet);
    }*/
    
    camGame.offset = { sw / 2.0f, sh / 2.0f };

    healthBar->barRect.x = (sw / 2) - (healthBar->barRect.width / 2);

    float upHUD = sh - 100;
    float downHUD = 50;

    healthBar->barRect.y = Lerp(upHUD, downHUD, scrollLerp);

    if (conductor != nullptr && !songSections.empty()) {
        size_t sectionIndex = (size_t)((conductor->songPosition / conductor->stepCrochet) / 16);

        if (sectionIndex < songSections.size()) {
            // 0 = Opponent, 1 = Player
            currentTurn = songSections[sectionIndex].mustHitSection ? 1 : 0;
        }
    }

    player->Update(); opponent->Update(); gf->Update();

    healthBar->Update(health, score);
    if (health > 2.0f) health = 2.0f;
    if (health <= 0.0f) {
        health = 0.0f;
        // gameover here
    }

    // Section Focus
    //int curSection = (int)(conductor->songPosition / (conductor->stepCrochet * 16));

    health = std::clamp(health, 0.0f, 2.0f);

    if (musicLoaded && !IsMusicStreamPlaying(inst) && !startingSong) {
        isEnding = true;
    }

    if (fabs(scrollLerp - scrollTarget) < 0.001f) {
        scrollLerp = scrollTarget;
    }

    UpdateInput();     // input primeiro

    if (!paused) {
        UpdateMusic(dt);
        UpdateNotes();
        UpdateCamera(dt);

        player->Update();
        opponent->Update();
        gf->Update();
    }

    UpdateHUD(); // UI baseada nos dados
    Ending(dt); // efeitos finais

    if (startingSong) {
        Countdown(dt);
    }

}

void PlayState::BeatHit(int beat) {
    
    /*
        In this function, you make things
        happen in time with the music!
    */

    // Icon
    healthBar->iconScale = 1.2f;
    
    // Camera
    if (beat % 4 == 0) {
        camGame.zoom += 0.035f;
        // camHUD.zoom += 0.02f;
    }

    // Characters
    if (player && player->holdTimer <= 0) {
        player->PlayAnim("idle");
    }
    if (opponent && opponent->holdTimer <= 0) {
        opponent->PlayAnim("idle");
    }
    if (gf && beat % 2 == 0) {
        static bool danceDir = false;
        danceDir = !danceDir;

        if (danceDir)
            gf->PlayAnim("danceRight", true);
        else
            gf->PlayAnim("danceLeft", true);
    }
}

void PlayState::OnBeat(int beat) {

    /*
    *   Beats here
    */

}

void PlayState::DrawNotes() {

    /*
    *   
    *   
    */

    for (const auto& note : songNotes) {
        if (!note.active) continue;

        StrumLine* targetStrum = note.mustHit ? playerStrums : enemyStrums;
        
        float noteX = targetStrum->arrows[note.noteData].position.x;
        float direction = Lerp(-1.0f, 1.0f, scrollLerp);

        float noteY = targetStrum->arrows[note.noteData].position.y +
                    direction * (conductor->songPosition - note.strumTime) * scrollSpeed;

        if (noteY > -200 && noteY < GetScreenHeight() + 200) { 
            DrawTexturePro(
                noteAssets,
                note.source,
                { noteX, noteY, note.source.width * 0.7f, note.source.height * 0.7f },
                { 0, 0 }, 0, WHITE
            );
        }
    }
}

void PlayState::UpdateInput() {

    /*
    *   
    *   
    */

    // DEBUGs
    if (IsKeyPressed(KEY_SEVEN)) {
        if (musicLoaded) {
            PauseMusicStream(inst);
            PauseMusicStream(voices);
        }

        currentState = CHART_EDITOR;
        return;
    }

    /*
    *   Gameplay
    */

    if (IsKeyPressed(KEY_ENTER)) {
        paused = !paused;
    }

    // Restart song
    if (IsKeyPressed(KEY_R)) {
        LoadSong(currentSong);
    }

    /*
    *   Cool Functions
    */

    if (IsKeyPressed(KEY_TAB) && CanToggleScroll()) {
        isDownscroll = !isDownscroll;
        scrollTarget = isDownscroll ? 1.0f : 0.0f;
    }

}

void PlayState::UpdateNotes() {

    /*
    *   
    *   
    */

    // Vars
    hitThisFrame = false;
    float singTime = (5.0f * conductor->stepCrochet) / 1000.0f;

    // Note Logic
    for (auto& note : songNotes) {
        if (!note.active) continue;

        StrumLine* targetStrum = note.mustHit ? playerStrums : enemyStrums;
        float noteY = targetStrum->arrows[note.noteData].position.y + 
                      0.45f * (note.strumTime - conductor->songPosition) * scrollSpeed;

        // Opponent Hit Everytime
        if (!note.mustHit && conductor->songPosition >= note.strumTime) {
            note.active = false;
            
            opponent->PlayAnim(singAnims[note.noteData], true);
            opponent->holdTimer = singTime; 
            enemyStrums->PlayAnim(note.noteData);
            continue;
        }

        // Miss
        if (noteY < -100 && note.active) {
            note.active = false;
            if (note.mustHit) {
                HandleMiss(note.noteData);
            }
        }

        // Hit
        if (note.mustHit) {
            int key = GetKeyFromDirection(note.noteData);
            bool checkHit = std::abs(note.strumTime - conductor->songPosition) < 160.0f;

            if (IsKeyPressed(key) && checkHit) {
                HandleHit(note);
                hitThisFrame = true;
                break;
            }
        }
    }

    // if (player->holdTimer <= 0 || hitThisFrame) SetMusicVolume(voices, 1.0f);

    // Ghost Tapping
    if (!hitThisFrame) {
        for (int i = 0; i < 4; i++) {
            // arrows[i].position.x = xOffset + (i * swagWidth);
            if (IsKeyPressed(GetKeyFromDirection(i))) {
                bool noteNearby = false;
                for (const auto& note : songNotes) {
                    if (note.active && note.mustHit && note.noteData == i) {
                        if (std::abs(note.strumTime - conductor->songPosition) < 250.0f) {
                            noteNearby = true;
                            break;
                        }
                    }
                }
                if (!noteNearby) {
                    HandleMiss(i);
                }
            }
        }
    }
}

void PlayState::HandleHit(ChartNote& note) {
    float singTime = (5.0f * conductor->stepCrochet) / 1000.0f;

    note.active = false;

    health += 0.04f;
    score += 350;

    player->PlayAnim(singAnims[note.noteData], true);
    player->holdTimer = singTime;

    playerStrums->PlayAnim(note.noteData);

    SetMusicVolume(voices, 1.0f);
}

void PlayState::HandleMiss(int direction) {
    float singTime = (5.0f * conductor->stepCrochet) / 1000.0f;

    health -= 0.05f;
    score -= 350;

    player->PlayAnim(missAnims[direction], true);
    player->holdTimer = singTime;

    SetMusicVolume(voices, 0.0f);
}

void PlayState::UpdateMusic(float dt) {

    /*
    *   
    *   
    */

    if (!musicLoaded) return;

    UpdateMusicStream(inst);
    UpdateMusicStream(voices);

    conductor->songPosition = GetMusicTimePlayed(inst) * 1000;

    // Beat Hit
    int currentBeat = (int)(conductor->songPosition / conductor->crochet);
    if (currentBeat > lastBeat) {
        lastBeat = currentBeat;
        BeatHit(currentBeat);
        OnBeat(currentBeat);
    }

    /*if (musicLoaded) {
        UpdateMusicStream(inst);
        UpdateMusicStream(voices);
        conductor->songPosition = GetMusicTimePlayed(inst) * 1000;
        int currentBeat = (int)(conductor->songPosition / conductor->crochet);
        if (currentBeat > lastBeat) {
            lastBeat = currentBeat;
            BeatHit(currentBeat);
        }
    }*/

}

void PlayState::UpdateHUD() {

    /*
    *   
    *   
    */
}

void PlayState::Countdown(float dt) {

    /*
    *   
    *   
    */

    if (!startingSong || skipIntro) return;

    startTimer += dt * 1000;

    int currentCountdown = (int)(std::abs(startTimer) / conductor->crochet);

    if (currentCountdown != countdownLevel && startTimer < 0) {
        countdownLevel = currentCountdown;

        switch(countdownLevel) {
            case 3: PlaySound(introThree); break;
            case 2: PlaySound(introTwo); break;
            case 1: PlaySound(introOne); break;
            case 0: PlaySound(introGo); break;
        }
    }

    if (startTimer >= 0) {
        startingSong = false;
        PlayMusicStream(inst);
        PlayMusicStream(voices);
    }
}

void PlayState::Ending(float dt) {

    /*
    *   
    *   
    */

    if (!isEnding) return;

    fadeAlpha += dt * 1.5f;
    if (fadeAlpha >= 1.0f) {
        // no futuro currentState = MainMenu;
    }

}

void PlayState::DrawHUD() {
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    
    if (healthBar != nullptr) {
        healthBar->barRect.x = (sw / 2.0f) - (healthBar->barRect.width / 2.0f);
        float upHUD = sh - 100;
        float downHUD = 50;

        healthBar->barRect.y = Lerp(upHUD, downHUD, scrollLerp);
    }

    // Render
    BeginMode2D(camHUD);
        // DrawText(currentTurn == 0 ? "FOCUS: PLAYER" : "FOCUS: OPPONENT", 20, 30, 20, DARKBLUE);
        if (healthBar != nullptr) {
            healthBar->Draw();
        }
    EndMode2D();
}

bool PlayState::CanToggleScroll() {
    return fabs(scrollLerp - scrollTarget) < 0.01f;
}

void PlayState::Draw() {
    ClearBackground(BLACK);

    // Render Gameplay
    BeginMode2D(camGame);
        /*
        *   Characters and Stage here.
        */
        stage->Draw(camGame);
        gf->Draw();
        opponent->Draw();
        player->Draw();
    EndMode2D();

    // Render Upper
    BeginMode2D(camUpper);
        /*
        *   StrumLines and Notes here.
        */
        enemyStrums->Draw();
        playerStrums->Draw();
        DrawNotes();
    EndMode2D();

    // Render HUD
    DrawHUD();
    if (startingSong) {
        Texture2D* img = nullptr;
        if (countdownLevel == 2) img = &countdownReady;
        if (countdownLevel == 1) img = &countdownSet;
        if (countdownLevel == 0) img = &countdownGo;

        if (img) {
            float sw = (float)GetScreenWidth(); float sh = (float)GetScreenHeight();
            Vector2 pos = { sw / 2.0f, sh / 2.0f };
            DrawTexturePro(*img, 
                {0, 0, (float)img->width, (float)img->height},
                {pos.x, pos.y, (float)img->width, (float)img->height},
                {img->width / 2.0f, img->height / 2.0f}, 0, WHITE);
        }
    }

    if (fadeAlpha > 0) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, fadeAlpha));
    }
}