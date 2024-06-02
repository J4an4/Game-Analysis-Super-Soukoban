#include "Scene.h"
#include <stdio.h>

#include "Globals.h"
#define KEY_F2 289 
#define KEY_SPACE 32

Scene::Scene()
{
	player = nullptr;
	level = nullptr;
	level_two = nullptr;

	camera.target = { 0, 0 };				//Center of the screen
	camera.offset = { 0, MARGIN_GUI_Y };	//Offset from the target (center of the screen)
	camera.rotation = 0.0f;					//No rotation
	camera.zoom = 1.0f;						//Default zoom
		
	debug = DebugMode::OFF;
}
Scene::~Scene()
{
	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}
	if (level != nullptr)
	{
		level->Release();
		delete level;
		level = nullptr;
	}
	if (level_two != nullptr)
	{
		level_two->Release();
		delete level_two;
		level_two = nullptr;
	}
	for (Entity* obj : objects)
	{
		delete obj;
	}
	objects.clear();
}

AppStatus Scene::Init()
{
	// Create player
	player = new Player({ 0, 0 }, State::IDLE, Look::RIGHT);
	if (player == nullptr || player->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialize Player");
		return AppStatus::ERROR;
	}

	// Create levels
	level = new TileMap();
	level_two = new TileMap();
	if (level == nullptr || level_two == nullptr ||
		level->Initialise() != AppStatus::OK || level_two->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialize Level");
		return AppStatus::ERROR;
	}

	// Load initial level
	if (LoadLevel(1) != AppStatus::OK)
	{
		LOG("Failed to load Level 1");
		return AppStatus::ERROR;
	}

	// Set tile maps for the player
	player->SetTileMap(level);
	player->SetAuxTileMap(level_two);

	return AppStatus::OK;
}

AppStatus Scene::LoadLevel(int stage)
{
	int size;
	int x, y, i;
	Tile tile;
	Point pos;

	Object* obj;

	ClearLevel();

	size = LEVEL_WIDTH * LEVEL_HEIGHT;
	int* map = nullptr;
	int* map_two = nullptr;

	if (stage == 1)
	{
		player->level = 1;
		player->steps = 0;
		player->lost = false;
		player->won = false;

		map = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 2, 3, 3, 3, 3, 3, 2, 6, 6,
				6, 6, 2, 5, 5, 7, 7, 7, 2, 6, 6,
				6, 6, 2, 5, 5, 5, 3, 3, 3, 2, 6,
				6, 2, 3, 3, 1, 5, 5, 5, 5, 2, 6,
				6, 2, 5, 5, 5, 2, 1, 3, 5, 2, 6,
				6, 2, 5, 1, 5, 2, 5, 5, 5, 2, 6,
				6, 2, 5, 5, 5, 2, 3, 3, 3, 3, 6,
				6, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
		};

		map_two = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 2, 3, 3, 3, 3, 3, 2, 6, 6,
				6, 6, 2, 5, 5, 7, 7, 7, 2, 6, 6,
				6, 6, 2, 5, 5, 5, 3, 3, 3, 2, 6,
				6, 2, 3, 3, 5, 5, 5, 5, 5, 2, 6,
				6, 2, 5, 5, 5, 2, 5, 3, 5, 2, 6,
				6, 2, 5, 5, 5, 2, 5, 5, 5, 2, 6,
				6, 2, 5, 5, 5, 2, 3, 3, 3, 3, 6,
				6, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
		};
		player->InitScore();



	}

	else if (stage == 2) {
		player->level = 2;
		player->steps = 0;
		player->lost = false;
		player->won = false;

		map = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 2, 3, 3, 3, 2, 6, 6, 6, 6, 6,
				6, 2, 5, 5, 5, 2, 6, 6, 6, 6, 6,
				6, 2, 5, 1, 1, 2, 6, 2, 3, 2, 6,
				6, 2, 5, 1, 5, 2, 6, 2, 7, 2, 6,
				6, 3, 2, 2, 5, 3, 3, 3, 7, 2, 6,
				6, 6, 2, 3, 5, 5, 5, 5, 7, 2, 6,
				6, 6, 2, 5, 5, 5, 2, 5, 5, 3, 6,
				6, 6, 2, 5, 5, 5, 2, 3, 3, 3, 6,
				6, 6, 3, 3, 3, 3, 3, 6, 6, 6, 6
		};


		map_two = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 2, 3, 3, 3, 2, 6, 6, 6, 6, 6,
				6, 2, 5, 5, 5, 2, 6, 6, 6, 6, 6,
				6, 2, 5, 5, 5, 2, 6, 2, 3, 2, 6,
				6, 2, 5, 5, 5, 2, 6, 2, 7, 2, 6,
				6, 3, 2, 2, 5, 3, 3, 3, 7, 2, 6,
				6, 6, 2, 3, 5, 5, 5, 5, 7, 2, 6,
				6, 6, 2, 5, 5, 5, 2, 5, 5, 3, 6,
				6, 6, 2, 5, 5, 5, 2, 3, 3, 3, 6,
				6, 6, 3, 3, 3, 3, 3, 6, 6, 6, 6
		};
		player->InitScore();

	}

	else if (stage == 3) {
		player->level = 3;
		player->steps = 0;
		player->lost = false;
		player->won = false;

		map = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 2, 3, 3, 2, 6, 6, 6,
				6, 6, 6, 2, 3, 5, 5, 2, 6, 6, 6,
				6, 6, 6, 2, 5, 1, 5, 2, 6, 6, 6,
				6, 6, 6, 2, 2, 1, 5, 3, 2, 6, 6,
				6, 6, 6, 2, 3, 5, 1, 5, 2, 6, 6,
				6, 6, 6, 2, 7, 1, 5, 5, 2, 6, 6,
				6, 6, 6, 2, 7, 7, 4, 7, 2, 6, 6,
				6, 6, 6, 3, 3, 3, 3, 3, 3, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};


		map_two = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 2, 3, 3, 2, 6, 6, 6,
				6, 6, 6, 2, 3, 5, 5, 2, 6, 6, 6,
				6, 6, 6, 2, 5, 5, 5, 2, 6, 6, 6,
				6, 6, 6, 2, 2, 5, 5, 3, 2, 6, 6,
				6, 6, 6, 2, 3, 5, 5, 5, 2, 6, 6,
				6, 6, 6, 2, 7, 5, 5, 5, 2, 6, 6,
				6, 6, 6, 2, 7, 7, 7, 7, 2, 6, 6,
				6, 6, 6, 3, 3, 3, 3, 3, 3, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};
		player->InitScore();

	}

	else if (stage == 4)
	{
		player->level = 4;
		player->steps = 0;
		player->lost = false;
		player->won = false;

		map = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 2, 2, 2, 2, 2, 2, 3, 6, 6,
				6, 6, 2, 5, 5, 5, 5, 5, 3, 3, 2,
				6, 2, 3, 1, 3, 3, 3, 5, 5, 5, 2,
				6, 2, 5, 5, 5, 1, 5, 5, 1, 5, 2,
				6, 2, 5, 7, 7, 2, 5, 1, 5, 2, 3,
				6, 3, 2, 7, 7, 2, 5, 5, 5, 2, 6,
				6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};

		map_two = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 2, 2, 2, 2, 2, 2, 3, 6, 6,
				6, 6, 2, 5, 5, 5, 5, 5, 3, 3, 2,
				6, 2, 5, 5, 3, 3, 3, 5, 5, 5, 2,
				6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2,
				6, 2, 5, 7, 7, 2, 5, 5, 5, 2, 3,
				6, 3, 2, 7, 7, 2, 5, 5, 5, 2, 6,
				6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};
		player->InitScore();
	}

	else if (stage == 5)
	{
		player->level = 5;
		player->steps = 0;
		player->lost = false;
		player->won = false;

		map = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 2, 3, 3, 2, 6, 6, 6, 6,
				6, 6, 6, 2, 5, 5, 3, 3, 2, 6, 6,
				6, 6, 6, 2, 5, 1, 5, 5, 2, 6, 6,
				6, 6, 2, 3, 2, 5, 2, 5, 3, 2, 6,
				6, 6, 2, 7, 3, 5, 3, 5, 5, 2, 6,
				6, 6, 2, 7, 1, 5, 5, 3, 5, 2, 6,
				6, 6, 2, 7, 5, 5, 5, 1, 5, 2, 6,
				6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};

		map_two = new int[size] {
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 2, 3, 3, 2, 6, 6, 6, 6,
				6, 6, 6, 2, 5, 5, 3, 3, 2, 6, 6,
				6, 6, 6, 2, 5, 5, 5, 5, 2, 6, 6,
				6, 6, 2, 3, 2, 5, 2, 5, 3, 2, 6,
				6, 6, 2, 7, 3, 5, 3, 5, 5, 2, 6,
				6, 6, 2, 7, 5, 5, 5, 3, 5, 2, 6,
				6, 6, 2, 7, 5, 5, 5, 5, 5, 2, 6,
				6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};
		player->InitScore();
	}
	else if (stage == 6)
	{
		player->level = 6;
		player->steps = 0;
		player->lost = false;
		player->won = false;

		map = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 2, 3, 3, 3, 3, 2, 6,
				6, 6, 2, 3, 3, 5, 5, 5, 5, 2, 6,
				6, 2, 3, 7, 5, 1, 3, 3, 5, 3, 2,
				6, 2, 7, 7, 1, 5, 1, 5, 5, 5, 2,
				6, 2, 7, 7, 5, 1, 5, 1, 5, 2, 3,
				6, 3, 3, 3, 3, 3, 2, 5, 5, 2, 6,
				6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};

		map_two = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 2, 3, 3, 3, 3, 2, 6,
				6, 6, 2, 3, 3, 5, 5, 5, 5, 2, 6,
				6, 2, 3, 7, 5, 5, 3, 3, 5, 3, 2,
				6, 2, 7, 7, 5, 5, 5, 5, 5, 5, 2,
				6, 2, 7, 7, 5, 5, 5, 5, 5, 2, 3,
				6, 3, 3, 3, 3, 3, 2, 5, 5, 2, 6,
				6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};
		player->InitScore();
	}
	else if (stage == 7)
	{
		player->level = 7;
		player->steps = 0;
		player->lost = false;
		player->won = false;

		map = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 2, 3, 3, 3, 3, 2, 6, 6,
				6, 6, 6, 2, 5, 5, 5, 5, 2, 6, 6,
				6, 2, 3, 3, 1, 1, 1, 5, 2, 6, 6,
				6, 2, 5, 5, 1, 7, 7, 5, 3, 6, 6,
				6, 2, 5, 1, 7, 7, 7, 2, 3, 6, 6,
				6, 3, 3, 3, 2, 5, 5, 2, 6, 6, 6,
				6, 6, 6, 6, 3, 3, 3, 3, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};

		map_two = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 2, 3, 3, 3, 3, 2, 6, 6,
				6, 6, 6, 2, 5, 5, 5, 5, 2, 6, 6,
				6, 2, 3, 3, 5, 5, 5, 5, 2, 6, 6,
				6, 2, 5, 5, 5, 7, 7, 5, 3, 6, 6,
				6, 2, 5, 5, 7, 7, 7, 2, 3, 6, 6,
				6, 3, 3, 3, 2, 5, 5, 2, 6, 6, 6,
				6, 6, 6, 6, 3, 3, 3, 3, 6, 6, 6,
				6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		};
		player->InitScore();
	}

	else if (stage == 8)
	{
		player->level = 8;
		player->steps = 0;
		player->lost = false;
		player->won = false;

		map = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 2, 3, 3, 3, 2, 3, 3, 2,
				6, 6, 6, 2, 5, 5, 5, 3, 7, 5, 2,
				6, 6, 2, 3, 5, 5, 1, 7, 7, 7, 2,
				6, 6, 2, 5, 5, 1, 5, 2, 4, 7, 2,
				6, 2, 3, 5, 3, 3, 1, 3, 5, 3, 2,
				6, 2, 5, 5, 5, 1, 5, 5, 1, 5, 2,
				6, 2, 5, 5, 5, 2, 5, 5, 5, 5, 2,
				6, 3, 3, 3, 3, 3, 3, 2, 5, 5, 2,
				6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3,
		};

		map_two = new int[size] {
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
				6, 6, 6, 2, 3, 3, 3, 2, 3, 3, 2,
				6, 6, 6, 2, 5, 5, 5, 3, 7, 5, 2,
				6, 6, 2, 3, 5, 5, 5, 7, 7, 7, 2,
				6, 6, 2, 5, 5, 5, 5, 2, 7, 7, 2,
				6, 2, 3, 5, 3, 3, 5, 3, 5, 3, 2,
				6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2,
				6, 2, 5, 5, 5, 2, 5, 5, 5, 5, 2,
				6, 3, 3, 3, 3, 3, 3, 2, 5, 5, 2,
				6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3,
		};
		player->InitScore();
	}

	else
	{
		//Error level doesn't exist or incorrect level number
		LOG("Failed to load level, stage %d doesn't exist", stage);
		return AppStatus::ERROR;
	}

	//Entities and objects
	i = 0;
	for (y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (x = 0; x < LEVEL_WIDTH; ++x)
		{
			tile = (Tile)map[i];
			if (tile == Tile::EMPTY)
			{
				map[i] = 0;
			}
			else if (tile == Tile::PLAYER)
			{
				// Code for handling player entity
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				player->SetPos(pos);
				map[i] = 0;
			}

			else if (tile == Tile::COMPLETION)
			{
				// Code for handling completion object
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				objects.push_back(obj);
				map[i] = 0;
			}
			++i;
		}
	}

	i = 0;
	for (y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (x = 0; x < LEVEL_WIDTH; ++x)
		{
			tile = (Tile)map[i];
			if (tile == Tile::EMPTY)
			{
				map_two[i] = 0;
			}
			else if (tile == Tile::PLAYER)
			{
				// Code for handling player entity
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				player->SetPos(pos);
				map_two[i] = 0;
			}

			else if (tile == Tile::COMPLETION)
			{
				// Code for handling completion object
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				objects.push_back(obj);
				map_two[i] = 0;
			}
			++i;
		}
	}

	// Setting player's initial position based on the stage
	if (stage == 1) {
		pos.x = 4 * TILE_SIZE;
		pos.y = 3 * TILE_SIZE - 1;
		player->SetPos(pos);
	}

	else if (stage == 2) {
		pos.x = 2 * TILE_SIZE;
		pos.y = 3 * TILE_SIZE - 1;
		player->SetPos(pos);
	}

	else if (stage == 3) {
		pos.x = 4 * TILE_SIZE;
		pos.y = 4 * TILE_SIZE - 1;
		player->SetPos(pos);
	}

	else if (stage == 4) {
		pos.x = 3 * TILE_SIZE;
		pos.y = 6 * TILE_SIZE - 1;
		player->SetPos(pos);
	}

	else if (stage == 5) {
		pos.x = 5 * TILE_SIZE;
		pos.y = 3 * TILE_SIZE - 1;
		player->SetPos(pos);
	}

	else if (stage == 6) {
		pos.x = 9 * TILE_SIZE;
		pos.y = 6 * TILE_SIZE - 1;
		player->SetPos(pos);
	}

	else if (stage == 7) {
		pos.x = 2 * TILE_SIZE;
		pos.y = 6 * TILE_SIZE - 1;
		player->SetPos(pos);
	}

	else if (stage == 8) {
		pos.x = 8 * TILE_SIZE;
		pos.y = 8 * TILE_SIZE - 1;
		player->SetPos(pos);
	}

	
	//Tile map
	level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT);
	level_two->Load(map_two, LEVEL_WIDTH, LEVEL_HEIGHT);
	
	return AppStatus::OK;
}

void Scene::Update()
{
	Point p1, p2;
	AABB box;

	//Switch between the different debug modes: off, on (sprites & hitboxes), on (hitboxes) 
	if (IsKeyPressed(KEY_F1))
	{
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);
	}
	

	level->Update();
	player->Update();
	CheckCollisions();
}


void Scene::Render()
{
	BeginMode2D(camera);

	level->Render();
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
	{
		RenderObjects();
		player->Draw();
	}
	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
	{
		RenderObjectsDebug(YELLOW);
		player->DrawDebug(GREEN);
	}

	EndMode2D();

	RenderGUI();
}

void Scene::Release()
{
	level->Release();
	level_two->Release();
	player->Release();
	ClearLevel();
}

void Scene::CheckCollisions()
{
	AABB player_box, obj_box;

	player_box = player->GetHitbox();
	auto it = objects.begin();
	while (it != objects.end())
	{
		obj_box = (*it)->GetHitbox();
		if (player_box.TestAABB(obj_box))
		{
			player->IncrScore((*it)->Points());

			//Delete the object

			delete* it;
			//Erase the object from the vector and get the iterator to the next valid element
			it = objects.erase(it);
		}
		else
		{
			//Move to the next object
			++it;
		}
	}

}

AppStatus Scene::ChangeLevel(int newLevel) {
	ClearLevel();

	// Load new level based on newLevel parameter
	AppStatus status = LoadLevel(newLevel);
	if (status != AppStatus::OK) {
		LOG("Failed to load new level");
		return status;
	}

	// Update player's association with the new level
	if (newLevel == 1) {
		player->SetTileMap(level);
	}

	else if (newLevel == 2) {
		player->SetTileMap(level);
	}

	else if (newLevel == 3) {
		player->SetTileMap(level);
	}
	else if (newLevel == 4) {
		player->SetTileMap(level);
	}

	else if (newLevel == 5) {
		player->SetTileMap(level);
	}
	else if (newLevel == 6) {
		player->SetTileMap(level);
	}

	else if (newLevel == 7) {
		player->SetTileMap(level);
	}

	else if (newLevel == 8) {
		player->SetTileMap(level);
	}
	
	return AppStatus::OK;
}

void Scene::ClearLevel()
{
	for (Object* obj : objects)
	{
		delete obj;
	}
	objects.clear();
}

void Scene::RenderObjects() const
{
	for (Object* obj : objects)
	{
		obj->Draw();
	}
}

void Scene::RenderObjectsDebug(const Color& col) const
{
	for (Object* obj : objects)
	{
		obj->DrawDebug(col);
	}
}

void Scene::RenderGUI() const
{
	if (player->level == 1)
	{
		DrawText(TextFormat("STAGE", player->GetScore()), 10, 10, 8, YELLOW);
		DrawText(TextFormat("01", player->GetScore()), 58, 10, 8, LIGHTGRAY);

		DrawText(TextFormat("STEP", player->steps), 10, 24, 8, YELLOW);

		if (player->steps < 10)
		{
			DrawText(TextFormat("000%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 100)
		{
			DrawText(TextFormat("00%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 1000)
		{
			DrawText(TextFormat("0%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		DrawText(TextFormat("LIMIT", player->steps), 10, 34, 8, YELLOW);
		DrawText(TextFormat("0090", player->steps), 45, 34, 8, LIGHTGRAY);

		if (player->PushingLeft)
		{
			level->DrawBox(player->x - TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingRight)
		{
			level->DrawBox(player->x + TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingUp)
		{
			level->DrawBox(player->x, player->y - TILE_SIZE * 2);
		}
		if (player->PushingDown)
		{
			level->DrawBox(player->x, player->y);
		}
		if (player->won)
		{
			level->DrawWin();
		}
		if (player->lost)
		{
			level->DrawLose();
		}
	}

	else if (player->level == 2)
	{
		DrawText(TextFormat("STAGE", player->GetScore()), 10, 10, 8, YELLOW);
		DrawText(TextFormat("02", player->GetScore()), 58, 10, 8, LIGHTGRAY);

		DrawText(TextFormat("STEP", player->steps), 10, 24, 8, YELLOW);

		if (player->steps < 10)
		{
			DrawText(TextFormat("000%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 100)
		{
			DrawText(TextFormat("00%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 1000)
		{
			DrawText(TextFormat("0%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		DrawText(TextFormat("LIMIT", player->steps), 10, 34, 8, YELLOW);
		DrawText(TextFormat("0120", player->steps), 45, 34, 8, LIGHTGRAY);

		if (player->PushingLeft)
		{
			level->DrawBox(player->x - TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingRight)
		{
			level->DrawBox(player->x + TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingUp)
		{
			level->DrawBox(player->x, player->y - TILE_SIZE * 2);
		}
		if (player->PushingDown)
		{
			level->DrawBox(player->x, player->y);
		}
		if (player->won)
		{
			level->DrawWin();
		}
		if (player->lost)
		{
			level->DrawLose();
		}
	}

	else if (player->level == 3)
	{
		DrawText(TextFormat("STAGE", player->GetScore()), 10, 10, 8, YELLOW);
		DrawText(TextFormat("03", player->GetScore()), 58, 10, 8, LIGHTGRAY);

		DrawText(TextFormat("STEP", player->steps), 10, 24, 8, YELLOW);

		if (player->steps < 10)
		{
			DrawText(TextFormat("000%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 100)
		{
			DrawText(TextFormat("00%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 1000)
		{
			DrawText(TextFormat("0%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		DrawText(TextFormat("LIMIT", player->steps), 10, 34, 8, YELLOW);
		DrawText(TextFormat("0050", player->steps), 45, 34, 8, LIGHTGRAY);

		if (player->PushingLeft)
		{
			level->DrawBox(player->x - TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingRight)
		{
			level->DrawBox(player->x + TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingUp)
		{
			level->DrawBox(player->x, player->y - TILE_SIZE * 2);
		}
		if (player->PushingDown)
		{
			level->DrawBox(player->x, player->y);
		}
		if (player->won)
		{
			level->DrawWin();
		}
		if (player->lost)
		{
			level->DrawLose();
		}
	}

	else if (player->level == 4)
	{
		DrawText(TextFormat("STAGE", player->GetScore()), 10, 10, 8, YELLOW);
		DrawText(TextFormat("04", player->GetScore()), 58, 10, 8, LIGHTGRAY);

		DrawText(TextFormat("STEP", player->steps), 10, 24, 8, YELLOW);

		if (player->steps < 10)
		{
			DrawText(TextFormat("000%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 100)
		{
			DrawText(TextFormat("00%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 1000)
		{
			DrawText(TextFormat("0%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		DrawText(TextFormat("LIMIT", player->steps), 10, 34, 8, YELLOW);
		DrawText(TextFormat("0140", player->steps), 45, 34, 8, LIGHTGRAY);

		if (player->PushingLeft)
		{
			level->DrawBox(player->x - TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingRight)
		{
			level->DrawBox(player->x + TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingUp)
		{
			level->DrawBox(player->x, player->y - TILE_SIZE * 2);
		}
		if (player->PushingDown)
		{
			level->DrawBox(player->x, player->y);
		}
		if (player->won)
		{
			level->DrawWin();
		}
		if (player->lost)
		{
			level->DrawLose();
		}
	}

	else if (player->level == 5)
	{
		DrawText(TextFormat("STAGE", player->GetScore()), 10, 10, 8, YELLOW);
		DrawText(TextFormat("05", player->GetScore()), 58, 10, 8, LIGHTGRAY);

		DrawText(TextFormat("STEP", player->steps), 10, 24, 8, YELLOW);

		if (player->steps < 10)
		{
			DrawText(TextFormat("000%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 100)
		{
			DrawText(TextFormat("00%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 1000)
		{
			DrawText(TextFormat("0%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		DrawText(TextFormat("LIMIT", player->steps), 10, 34, 8, YELLOW);
		DrawText(TextFormat("0065", player->steps), 45, 34, 8, LIGHTGRAY);

		if (player->PushingLeft)
		{
			level->DrawBox(player->x - TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingRight)
		{
			level->DrawBox(player->x + TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingUp)
		{
			level->DrawBox(player->x, player->y - TILE_SIZE * 2);
		}
		if (player->PushingDown)
		{
			level->DrawBox(player->x, player->y);
		}
		if (player->won)
		{
			level->DrawWin();
		}
		if (player->lost)
		{
			level->DrawLose();
		}
	}

	else if (player->level == 6)
	{
		DrawText(TextFormat("STAGE", player->GetScore()), 10, 10, 8, YELLOW);
		DrawText(TextFormat("06", player->GetScore()), 58, 10, 8, LIGHTGRAY);

		DrawText(TextFormat("STEP", player->steps), 10, 24, 8, YELLOW);

		if (player->steps < 10)
		{
			DrawText(TextFormat("000%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 100)
		{
			DrawText(TextFormat("00%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 1000)
		{
			DrawText(TextFormat("0%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		DrawText(TextFormat("LIMIT", player->steps), 10, 34, 8, YELLOW);
		DrawText(TextFormat("0060", player->steps), 45, 34, 8, LIGHTGRAY);

		if (player->PushingLeft)
		{
			level->DrawBox(player->x - TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingRight)
		{
			level->DrawBox(player->x + TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingUp)
		{
			level->DrawBox(player->x, player->y - TILE_SIZE * 2);
		}
		if (player->PushingDown)
		{
			level->DrawBox(player->x, player->y);
		}
		if (player->won)
		{
			level->DrawWin();
		}
		if (player->lost)
		{
			level->DrawLose();
		}
	}

	else if (player->level == 7)
	{
		DrawText(TextFormat("STAGE", player->GetScore()), 10, 10, 8, YELLOW);
		DrawText(TextFormat("07", player->GetScore()), 58, 10, 8, LIGHTGRAY);

		DrawText(TextFormat("STEP", player->steps), 10, 24, 8, YELLOW);

		if (player->steps < 10)
		{
			DrawText(TextFormat("000%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 100)
		{
			DrawText(TextFormat("00%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 1000)
		{
			DrawText(TextFormat("0%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		DrawText(TextFormat("LIMIT", player->steps), 10, 34, 8, YELLOW);
		DrawText(TextFormat("0080", player->steps), 45, 34, 8, LIGHTGRAY);

		if (player->PushingLeft)
		{
			level->DrawBox(player->x - TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingRight)
		{
			level->DrawBox(player->x + TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingUp)
		{
			level->DrawBox(player->x, player->y - TILE_SIZE * 2);
		}
		if (player->PushingDown)
		{
			level->DrawBox(player->x, player->y);
		}
		if (player->won)
		{
			level->DrawWin();
		}
		if (player->lost)
		{
			level->DrawLose();
		}
	}
	else if (player->level == 8)
	{
		DrawText(TextFormat("STAGE", player->GetScore()), 10, 10, 8, YELLOW);
		DrawText(TextFormat("08", player->GetScore()), 58, 10, 8, LIGHTGRAY);

		DrawText(TextFormat("STEP", player->steps), 10, 24, 8, YELLOW);

		if (player->steps < 10)
		{
			DrawText(TextFormat("000%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 100)
		{
			DrawText(TextFormat("00%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		else if (player->steps < 1000)
		{
			DrawText(TextFormat("0%i", player->steps), 45, 24, 8, LIGHTGRAY);
		}
		DrawText(TextFormat("LIMIT", player->steps), 10, 34, 8, YELLOW);
		DrawText(TextFormat("0160", player->steps), 45, 34, 8, LIGHTGRAY);

		if (player->PushingLeft)
		{
			level->DrawBox(player->x - TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingRight)
		{
			level->DrawBox(player->x + TILE_SIZE, player->y - TILE_SIZE + 1);
		}
		if (player->PushingUp)
		{
			level->DrawBox(player->x, player->y - TILE_SIZE * 2);
		}
		if (player->PushingDown)
		{
			level->DrawBox(player->x, player->y);
		}
		if (player->won)
		{
			level->DrawWin();
		}
		if (player->lost)
		{
			level->DrawLose();
		}
	}
}