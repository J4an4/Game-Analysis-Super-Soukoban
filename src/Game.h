#pragma once
#include "Globals.h"
#include "Scene.h"
#include <vector>
#include "raylib.h"

class Game
{
public:
    // Constructor and Destructor
    Game();
    ~Game();

    // Initialize the game
    AppStatus Initialise(float scale);

    // Update the game state
    AppStatus Update();

    // Render the game
    void Render();

    // Clean up resources
    void Cleanup();

    // Set the selected level
    void SetSelectedLevel(int level);

private:

    // Functions to handle game states
    AppStatus BeginPlay();
    AppStatus BeginPlayMenu();
    void FinishPlay();

    // Variables to manage time and animation
    int time;
    float a = 0;

    // Flag to indicate win or lose state
    bool wonOrLost = false;

    // Resource loading and unloading functions
    AppStatus LoadResources();
    void UnloadResources();

    // Game state and scene objects
    GameState state;
    Scene* scene;
    const Texture2D* img_menu;
    const Texture2D* img_initial;

    // Functions for scene transitions
    void Transition();

    // Transition animation variables
    bool transitionFromTopToBottom;
    bool introPlaying;
    float transitionAlpha;
   
    // Render textures and rectangles for scaling
    RenderTexture2D target;
    Rectangle src, dst;


    // Arrays of textures for different game sections
    std::vector<Texture2D> introImages;
    int currentIntroFrame;
    float introDuration;
    float timeBetweenImages;

    std::vector<Texture2D> coverImages;
    int currentCoverFrame;
    float coverDuration;
    float timeBetweenImages_cover;
    float framesCounter_cover;

    std::vector<Texture2D> menuImages;
    int currentMenuFrame;
    float menuDuration;
    float timeBetweenImages_menu;
    float framesCounter_menu;

    std::vector<Texture2D> menuImages_final;
    int currentMenuFrame_final;
    float menuDuration_final;
    float timeBetweenImages_menu_final;
    float framesCounter_menu_final;

    // General frame counter
    float framesCounter;      

    // Frame speed for animation
    float framesSpeed = 1;     

    // Selected level
    int selectedLevel;       
};
