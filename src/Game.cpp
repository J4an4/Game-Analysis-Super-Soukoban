#include "Game.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <stdio.h>
#include "raylib.h"
#include <vector>


#define DEBUG_LOG(text, ...) log(__FILE__, __LINE__, text, __VA_ARGS__)


Music soundMusic[10];
Sound soundEffect[10];

Game::Game()
{  
    // Initialize game state
    state = GameState::START;

    // Initialize scene and textures
    scene = nullptr;
    img_menu = nullptr;

    // Initialize time
    time = GetTime();

    // Initialize render texture and rectangles
    target = {};
    src = {};
    dst = {};

    // Initialize intro animation variables
    introPlaying = true;
    currentIntroFrame = 0;
    framesCounter = 0;
    introDuration = 35.0f;

    // Initialize cover animation variables
    currentCoverFrame = 0;
    framesCounter_cover = 0;
    coverDuration = 3.0f;

    // Initialize main menu animation variables
    currentMenuFrame = 0;
    framesCounter_menu = 0;
    menuDuration = 3.0f;

    // Initialize final menu animation variables
    currentMenuFrame_final = 0;
    framesCounter_menu_final = 0;
    menuDuration_final = 3.0f;


}
Game::~Game()
{
    // Release scene resources
    if (scene != nullptr)
    {
        scene->Release();
        delete scene;
        scene = nullptr;
    }

}

AppStatus Game::Initialise(float scale)
{
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Super Soukoban");
    ToggleFullscreen();

    // Calculate scaled dimensions
    float scaledWidth = WINDOW_WIDTH * scale;
    float scaledHeight = WINDOW_HEIGHT * scale;

    // Initialize window and render texture
    target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);

    if (target.id == 0)
    {
        LOG("Failed to create render texture");
        return AppStatus::ERROR;
    }
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    src = { 0, 0, WINDOW_WIDTH, -WINDOW_HEIGHT };
    dst = { 0, 0, scaledWidth, scaledHeight };

    // Load resources
    if (LoadResources() != AppStatus::OK)
    {
        LOG("Failed to load resources");
        return AppStatus::ERROR;
    }

    timeBetweenImages = 0.20f;
    timeBetweenImages_menu = 0.20f;
    timeBetweenImages_cover = 0.20f;
    timeBetweenImages_menu_final = 2.0f / menuImages_final.size();

    SetTargetFPS(60);
    SetExitKey(0);

    return AppStatus::OK;
}
AppStatus Game::LoadResources()
{
    ResourceManager& data = ResourceManager::Instance();

    // Load menu and initial images
    if (data.LoadTexture(Resource::IMG_MENU, "images/menu_photo.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_menu = data.GetTexture(Resource::IMG_MENU);

    if (data.LoadTexture(Resource::IMG_INITIAL, "images/initial_image.png") != AppStatus::OK)
    {
        return AppStatus::ERROR;
    }
    img_initial = data.GetTexture(Resource::IMG_INITIAL);


    // Load intro images 
    for (int i = 1; i <= 153; ++i)
    {
        std::string path = "images/intro/intro_" + std::to_string(i) + ".png";
        Texture2D introImage = LoadTexture(path.c_str());

        introImages.push_back(introImage);
    }


    // Load cover images
    for (int i = 1; i <= 21; ++i)
    {
        std::string path = "images/menu/menu_" + std::to_string(i) + ".png";
        Texture2D coverImage = LoadTexture(path.c_str());

        coverImages.push_back(coverImage);
    }


    // Load menu images
    for (int i = 21; i <= 47; ++i)
    {
        std::string path = "images/menu/menu_" + std::to_string(i) + ".png";
        Texture2D menuImage = LoadTexture(path.c_str());

        menuImages.push_back(menuImage);
    }


    // Load final menu images
    for (int i = 0; i <= 6; ++i)
    {
        std::string path = "images/final_transition/intro_" + std::to_string(i) + ".png";
        Texture2D menuImage_final = LoadTexture(path.c_str());

        menuImages_final.push_back(menuImage_final);
    }


    //Load music

    soundMusic[2] = LoadMusicStream("sound/music/Opening.ogg");
    soundMusic[1] = LoadMusicStream("sound/music/TitleScreen.ogg");
    soundMusic[5] = LoadMusicStream("sound/music/StageSelect.ogg");

    return AppStatus::OK;
}

// Begin playing the main menu
AppStatus Game::BeginPlayMenu()
{
    scene = new Scene();
    PlayMusicStream(soundMusic[1]);
   
    if (scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return AppStatus::ERROR;
    }
    if (scene->Init() != AppStatus::OK)
    {
        LOG("Failed to initialise Scene");
        return AppStatus::ERROR;
    }

    return AppStatus::OK;
}

// Begin playing the game
AppStatus Game::BeginPlay()
{
    scene = new Scene();
    soundMusic[0] = LoadMusicStream("sound/music/PhysicalLabor.ogg");
    PlayMusicStream(soundMusic[0]);
  
    if (scene == nullptr)
    {
        LOG("Failed to allocate memory for Scene");
        return AppStatus::ERROR;
    }
    if (scene->Init() != AppStatus::OK)
    {
        LOG("Failed to initialise Scene");
        return AppStatus::ERROR;
    }

    return AppStatus::OK;
}

// Set the selected level
void Game::SetSelectedLevel(int level)
{    selectedLevel = level;
   
// Change level based on selectedLevel
    switch (selectedLevel)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        scene->ChangeLevel(selectedLevel);
        StopMusicStream(soundMusic[0]);
        //PlayMusicStream(soundMusic[0]);
        break;
    default:
        break;
    }
    state = GameState::PLAYING;
}

void Game::FinishPlay()
{
    scene->Release();
    delete scene;
    scene = nullptr;
}


// Update the game state
AppStatus Game::Update()
{
    // Quit if window is closed
    if (WindowShouldClose()) return AppStatus::QUIT;

    // Update music streams
    UpdateMusicStream(soundMusic[0]);
    UpdateMusicStream(soundMusic[1]);
    UpdateMusicStream(soundMusic[2]);
    UpdateMusicStream(soundMusic[3]);
    
    // Update time
    time = GetTime();
   
    // Update game based on current state
    switch (state)
    {

    case GameState::START:
       
        // Display initial image and transition to main menu after 4 seconds
        DrawTexture(*img_initial, 0, 0, WHITE);
        if (time == 4)
        {
            BeginPlayMenu();
            state = GameState::INTRO;
        }
        break;

    case GameState::INTRO:
       
        // Display intro animation and transition to cover state after completion
        StopMusicStream(soundMusic[0]);
        StopMusicStream(soundMusic[1]);
        StopMusicStream(soundMusic[3]);

        if (!IsMusicStreamPlaying(soundMusic[2]))
        {
            PlayMusicStream(soundMusic[2]); // Start intro music
        }
        framesCounter += GetFrameTime();
        if (framesCounter >= timeBetweenImages)
        {
            framesCounter = 0;
            currentIntroFrame++;
        }

        if (currentIntroFrame < introImages.size())
        {
            DrawTexture(introImages[currentIntroFrame], 0, 0, WHITE);
        }
        else {
            Transition();
            StopMusicStream(soundMusic[2]);

            framesCounter_cover = 0;
            currentCoverFrame = 0;
            PlayMusicStream(soundMusic[5]);

            state = GameState::COVER;
        }
        break;

    case GameState::COVER:

        // Display cover animation and transition to main menu after completion
        UpdateMusicStream(soundMusic[5]);

        if (!IsMusicStreamPlaying(soundMusic[5]))
        {
            PlayMusicStream(soundMusic[5]); // Start intro music
        }
        framesCounter_cover += GetFrameTime();
        if (framesCounter_cover >= timeBetweenImages_cover)
        {
            framesCounter_cover = 0;
            currentCoverFrame++;
        }

        if (currentCoverFrame < coverImages.size())
        {
            DrawTexture(coverImages[currentCoverFrame], 0, 0, WHITE);
        }
        else {
            Transition();
            StopMusicStream(soundMusic[5]);

            BeginPlayMenu();
            PlayMusicStream(soundMusic[1]);

            state = GameState::MAIN_MENU;
            framesCounter_menu = 0;
            currentMenuFrame = 0;
        }
        break;

    case GameState::MAIN_MENU:
       
        // Display main menu and handle level selection
        wonOrLost = false;

        framesCounter_menu += GetFrameTime();
        if (framesCounter_menu >= timeBetweenImages_menu)
        {
            framesCounter_menu = 0;
            currentMenuFrame++;
        }

        if (currentMenuFrame < menuImages.size())
        {
            DrawTexture(menuImages[currentMenuFrame], 0, 0, WHITE);
        }
        
        if (IsKeyPressed(KEY_ONE)) {
            
            SetSelectedLevel(1);
            Transition();

            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;
        }

        else if (IsKeyPressed(KEY_TWO)) {
            
            SetSelectedLevel(2);
            Transition();

            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;
          }
        else if (IsKeyPressed(KEY_THREE)) {
          
            SetSelectedLevel(3);
            Transition();

            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;        
        }

        else if (IsKeyPressed(KEY_FOUR)) {
           
            SetSelectedLevel(4);
            Transition();

            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;      
        }
       
        else if (IsKeyPressed(KEY_FIVE)) {

            SetSelectedLevel(5);
            Transition();

            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;
        }

        else if (IsKeyPressed(KEY_SIX)) {
           
            SetSelectedLevel(6);
            Transition();

            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;         
        }
       
        else if (IsKeyPressed(KEY_SEVEN)) {
           
            SetSelectedLevel(7);
            Transition();

            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;            
        }
       
        else if (IsKeyPressed(KEY_EIGHT)) {
           
            SetSelectedLevel(8);
            Transition();

            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;
           }
       

        if (IsKeyPressed(KEY_ESCAPE))
        {
            return AppStatus::QUIT;
        }

        if (IsKeyPressed(KEY_SPACE) or (IsKeyPressed(KEY_R) and state == GameState::PLAYING))
        {
            Transition();
            state = GameState::FINAL_IMAGES;
            framesCounter_menu_final = 0;
            currentMenuFrame_final = 0;
        }
        break;


    case GameState::FINAL_IMAGES:
       
        // Display final images and transition to game playing state after completion
        framesCounter_menu_final += GetFrameTime();

        if (currentMenuFrame_final < menuImages_final.size())
        {
            DrawTexture(menuImages_final[currentMenuFrame_final], 0, 0, WHITE);

            if (framesCounter_menu_final >= timeBetweenImages_menu_final)
            {
                framesCounter_menu_final = 0;
                currentMenuFrame_final++;

                if (currentMenuFrame_final >= menuImages_final.size())
                {
                    Transition();
                    BeginPlay();
                    StopMusicStream(soundMusic[0]);
                    StopMusicStream(soundMusic[2]);
                    StopMusicStream(soundMusic[3]);
                    soundEffect[0] = LoadSound("sound/SFX/Transition.wav");
                    PlaySound(soundEffect[0]);
                    StopMusicStream(soundMusic[1]);
                    if (BeginPlay() != AppStatus::OK) return AppStatus::ERROR;
                    state = GameState::PLAYING;
                    scene->ChangeLevel(selectedLevel);
                                    
                   
                }
            }
        }
        
        break;
    
    case GameState::PLAYING:
        if (IsKeyPressed(KEY_ESCAPE))
        {
            Transition();
          
            PlaySound(soundEffect[0]);
            BeginPlayMenu();
            FinishPlay();
            StopMusicStream(soundMusic[0]);
            StopMusicStream(soundMusic[2]);
            StopMusicStream(soundMusic[3]);

            if (BeginPlayMenu() != AppStatus::OK) return AppStatus::ERROR;
            state = GameState::MAIN_MENU;
        }
        if (scene->player->lost and !wonOrLost)
        {
            StopMusicStream(soundMusic[0]);
            soundMusic[3] = LoadMusicStream("sound/music/Failure.ogg");
            PlayMusicStream(soundMusic[3]);
            wonOrLost = true;

            if (IsKeyPressed(KEY_R))
            {
                StopMusicStream(soundMusic[2]);
                StopMusicStream(soundMusic[3]);
                PlayMusicStream(soundMusic[0]);
            }
        }
        if (scene->player->won and !wonOrLost)
        {
            StopMusicStream(soundMusic[0]);
            soundMusic[2] = LoadMusicStream("sound/music/StageComplete.ogg");
            PlayMusicStream(soundMusic[2]);
            wonOrLost = true;
        }

        if (scene->player->won)
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                Transition();
                state = GameState::MAIN_MENU;
            }
        }
        else
        {
            scene->Update();
        }
        break;



    }
    return AppStatus::OK;
}

// Perform transition effect
void Game::Transition()
{
    BeginTextureMode(target);


    a = 0;
    while (a <= 1)
    {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, a));
        a += 0.2;
    }
    while (a >= 0)
    {
        DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, a));
        a -= 0.2;
    }
    a = 0;


    EndTextureMode();
}

void Game::Render()
{

    // Begin rendering to target texture
    BeginTextureMode(target);
    ClearBackground(BLACK);


     
    // Render based on current game state
    switch (state)
    {
    case GameState::START:  // Render initial image
        
        DrawTexture(*img_initial, 0, 0, WHITE);
        break;

    case GameState::INTRO:  // Render intro animation
        
        if (currentIntroFrame < introImages.size())
        {
            DrawTexture(introImages[currentIntroFrame], 0, 0, WHITE);
        }
        break;

    case GameState::COVER:   // Render cover animation
        
        if (currentCoverFrame < coverImages.size())
        {
            DrawTexture(coverImages[currentCoverFrame], 0, 0, WHITE);
        }
        break;

    case GameState::MAIN_MENU:  // Render main menu
       
        if (currentMenuFrame < menuImages.size())
        {
            DrawTexture(menuImages[currentMenuFrame], 0, 0, WHITE);
        }
       
        if (currentMenuFrame > menuImages.size()) {
            DrawTexture(*img_menu, 0, 0, WHITE);
        }
         break;

    case GameState::FINAL_IMAGES:   // Render final images
      
        if (currentMenuFrame_final < menuImages_final.size())
       {
            DrawTexture(menuImages_final[currentMenuFrame_final], 0, 0, WHITE);
        }
        break;
       
    case GameState::PLAYING:

        scene->Render();
        break;
    }
    EndTextureMode();
    // Draw render texture to screen, properly scaled
    BeginDrawing();
    ClearBackground(BLACK); // Clear the entire screen to black

    // Draw render texture to screen, properly scaled
 /*   BeginDrawing();
    DrawTexturePro(target.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
    EndDrawing();*/
    // Calculate the center position for the render target
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float scaleX = screenWidth / WINDOW_WIDTH;
    float scaleY = screenHeight / WINDOW_HEIGHT;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
    float scaledWidth = WINDOW_WIDTH * scale;
    float scaledHeight = WINDOW_HEIGHT * scale;
    float offsetX = (screenWidth - scaledWidth) / 2;
    float offsetY = (screenHeight - scaledHeight) / 2;

    // Draw the render target texture centered on the screen
    Rectangle dest = { offsetX, offsetY, scaledWidth, scaledHeight };
    DrawTexturePro(target.texture, src, dest, { 0, 0 }, 0.0f, WHITE);

    // Draw black bars (margins) if necessary to maintain aspect ratio
    if (scaleX < scaleY)
    {
        float barHeight = (screenHeight - scaledHeight) / 2;
        DrawRectangle(0, 0, screenWidth, barHeight, BLACK); // Barra superior
        DrawRectangle(0, screenHeight - barHeight, screenWidth, barHeight, BLACK); // Barra inferior
    }
    else
    {
        float barWidth = (screenWidth - scaledWidth) / 2;
        DrawRectangle(0, 0, barWidth, screenHeight, BLACK); // Barra izquierda
        DrawRectangle(screenWidth - barWidth, 0, barWidth, screenHeight, BLACK); // Barra derecha
    }

    EndDrawing();

}

void Game::Cleanup()
{
    UnloadResources();
    CloseWindow();
        
}

// Unload game resources
void Game::UnloadResources()
{
    ResourceManager& data = ResourceManager::Instance();
    data.ReleaseTexture(Resource::IMG_MENU);

    UnloadRenderTexture(target);
    LOG("img_initial is not loaded");
}