#pragma once
#include <raylib.h>
#include "Player.h"
#include "TileMap.h"
#include "Object.h"

// Enum to define different debug modes
enum class DebugMode { OFF, SPRITES_AND_HITBOXES, ONLY_HITBOXES, SIZE };

// Class representing a game scene
class Scene
{
public:
    // Constructor and Destructor
    Scene();
    ~Scene();

    // Initialize the scene
    AppStatus Init();

    // Update the scene
    void Update();

    // Render the scene
    void Render();

    // Release resources
    void Release();

    // Player object
    Player* player;

    // Change the current level
    AppStatus ChangeLevel(int newLevel);

    // Zoom level for the camera
    float zoomlevel;

private:
    // Current level number
    int currentLevel;

    // Load a specific level
    AppStatus LoadLevel(int stage);

    // Load a specific level (alternative version)
    AppStatus LoadLevel_hight(int stage);

    // Check collisions between objects
    void CheckCollisions();

    // Clear the current level
    void ClearLevel();

    // Render all objects
    void RenderObjects() const;

    // Render objects for debugging purposes
    void RenderObjectsDebug(const Color& col) const;

    // Render GUI elements
    void RenderGUI() const;



    // TileMap object for the level
    TileMap* level;

    // Alternative TileMap object for a different level
    TileMap* level_two;

    // Vector to hold all objects in the scene
    std::vector<Object*> objects;

    // Camera for the scene
    Camera2D camera;

    // Debug mode for rendering
    DebugMode debug;
};