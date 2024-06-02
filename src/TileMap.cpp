#include "TileMap.h"
#include "Globals.h"
#include "ResourceManager.h"
#include <cstring>
#include <raylib.h>


Music soundMusicWoL[10];



TileMap::TileMap()
{
	map = nullptr;
	map_two = nullptr;
	width = 0;
	height = 0;
	img_tiles = nullptr;
	img_hud = nullptr;
	img_box = nullptr;
	img_win = nullptr;
	img_lose = nullptr;

	InitTileDictionary();
	currentLevel = 1;  // Start at level 1
}

TileMap::~TileMap()
{
	if (map != nullptr)
	{
		delete[] map;
		map = nullptr;
	}
	if (map_two != nullptr)
	{
		delete[] map_two;
		map_two = nullptr;
	}
}
void TileMap::InitTileDictionary()
{
	const int n = TILE_SIZE;

	dict_rect[(int)Tile::EXTERIOR_FLOOR] = { 0,  0, n, n };

	dict_rect[(int)Tile::OTHER_BRICKS] = { n,  0, n, n };

	dict_rect[(int)Tile::BRICKS] = { n * 2, 0, n, n };

	dict_rect[(int)Tile::BOX_STATIC] = { n * 3, 0, n, n };

	dict_rect[(int)Tile::BOX_DYNAMIC] = { n * 4, 0, n, n };

	dict_rect[(int)Tile::FLOOR] = { n * 5, 0, n, n };

	dict_rect[(int)Tile::FLOORRED] = { n * 6, 0, n, n };
}
AppStatus TileMap::Initialise()
{
	ResourceManager& data = ResourceManager::Instance();

	if (data.LoadTexture(Resource::IMG_TILES, "images/Background.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	img_tiles = data.GetTexture(Resource::IMG_TILES);

	if (data.LoadTexture(Resource::IMG_HUD, "images/hud.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	img_hud = data.GetTexture(Resource::IMG_HUD);

	if (data.LoadTexture(Resource::IMG_BOX, "images/box.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	img_box = data.GetTexture(Resource::IMG_BOX);

	if (data.LoadTexture(Resource::IMG_WIN, "images/win.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	img_win = data.GetTexture(Resource::IMG_WIN);

	if (data.LoadTexture(Resource::IMG_LOSE, "images/lose.png") != AppStatus::OK)
	{
		return AppStatus::ERROR;
	}
	img_lose = data.GetTexture(Resource::IMG_LOSE);

	return AppStatus::OK;
}
AppStatus TileMap::Load(int data[], int w, int h)
{
	size = w * h;
	width = w;
	height = h;

	if (map != nullptr)	delete[] map;

	map = new Tile[size];
	if (map == nullptr)
	{
		LOG("Failed to allocate memory for tile map");
		return AppStatus::ERROR;
	}
	memcpy(map, data, size * sizeof(int));

	if (map_two != nullptr)	delete[] map_two;

	map_two = new Tile[size];
	if (map_two == nullptr)
	{
		LOG("Failed to allocate memory for tile map");
		return AppStatus::ERROR;
	}
	memcpy(map_two, data, size * sizeof(int));

	return AppStatus::OK;
}
void TileMap::Update()
{

}

bool TileMap::CheckWin(int winCount)
{
	Tile tile;
	int count = 0;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			tile = map[i * width + j];
			if (tile == Tile::BOX_STATIC)
			{
				count++;
			}
		}
	}


	// Verificar si se cumplió la condición de victoria
	if (count == winCount)
	{
		// Condición de victoria activa
		return true;
	}
	else
	{
		return false;
	}
}



Tile TileMap::GetTileIndex(int x, int y) const
{
	int idx = x + y * width;
	if (idx < 0 || idx >= size)
	{
		LOG("Error: Index out of bounds. Tile map dimensions: %dx%d. Given index: (%d, %d)", width, height, x, y)
			return Tile::AIR;
	}
	return map[x + y * width];
}
Tile TileMap::GetOldTileIndex(int x, int y) const
{
	int idx = x + y * width;
	if (idx < 0 || idx >= size)
	{
		LOG("Error: Index out of bounds. Tile map dimensions: %dx%d. Given index: (%d, %d)", width, height, x, y)
			return Tile::AIR;
	}
	return map_two[x + y * width];
}
void TileMap::ChangeTile(int x, int y, Tile t) const
{
	int idx = x + y * width;
	if (idx < 0 || idx >= size)
	{
		LOG("Error: Index out of bounds. Tile map dimensions: %dx%d. Given index: (%d, %d)", width, height, x, y)
	}
	map[x + y * width] = t;

}
bool TileMap::IsTileSolid(Tile tile) const
{
	return (Tile::SOLID_FIRST <= tile && tile <= Tile::SOLID_LAST);
}
bool TileMap::IsTileBox(Tile tile) const
{
	return (tile == Tile::BOX_DYNAMIC or tile == Tile::BOX_STATIC);
}
bool TileMap::IsTileGoal(Tile tile) const
{
	return (tile == Tile::FLOORRED);
}

void TileMap::UpdateBoxState(int x, int y) {
	
	Tile tileBelow = GetTileIndex(x, y);
	Tile currentTile = GetTileIndex(x, y);
	
	if (tileBelow == Tile::FLOORRED) {
		if (currentTile == Tile::BOX_DYNAMIC) {
			ChangeTile(x, y, Tile::BOX_STATIC);
		}
	}
	else {
		
		if (currentTile == Tile::BOX_STATIC) {
			ChangeTile(x, y, Tile::BOX_DYNAMIC);
		}
	}
}



bool TileMap::TestCollisionWallLeft(const AABB& box) const
{
	return CollisionX(box.pos, box.height);
}
bool TileMap::MoveBoxLeft(const AABB& box)
{
	bool canMove = BoxLeft(box.pos, box.width);
	if (canMove) {
		// Realizar el movimiento de la caja
		// Luego actualizar su estado en el mapa
		int x = box.pos.x / TILE_SIZE;
		int y = box.pos.y / TILE_SIZE;
		UpdateBoxState(x, y);
	}
	return canMove;
}
bool TileMap::TestBoxLeft(const AABB& box) const
{
	return CollisionBoxX(box.pos, box.height);
}


bool TileMap::TestCollisionWallRight(const AABB& box) const
{
	return CollisionX(box.pos + Point(box.width - 1, 0), box.height);
}
bool TileMap::MoveBoxRight(const AABB& box)
{
	bool canMove = BoxRight(box.pos, box.width);
	if (canMove) {
		// Realizar el movimiento de la caja
		// Luego actualizar su estado en el mapa
		int x = (box.pos.x + box.width) / TILE_SIZE;  // Calcula la columna siguiente
		int y = box.pos.y / TILE_SIZE;                // Mantiene la misma fila
		UpdateBoxState(x, y);
	}
	return canMove;
}
bool TileMap::TestBoxRight(const AABB& box) const
{
	return CollisionBoxX(box.pos + Point(box.width - 1, 0), box.height);
}


bool TileMap::TestCollisionWallUp(const AABB& box) const
{
	if (CollisionY(box.pos + Point(box.height - 1, 0), box.width) == true or CollisionY(box.pos, box.width) == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool TileMap::MoveBoxUp(const AABB& box)
{
	bool canMove = BoxUp(box.pos, box.height);
	if (canMove) {
		// Realizar el movimiento de la caja hacia arriba
		// Luego actualizar su estado en el mapa
		int x = box.pos.x / TILE_SIZE;
		int y = box.pos.y / TILE_SIZE;
		UpdateBoxState(x, y);
	}
	return canMove;
}
bool TileMap::TestBoxUp(const AABB& box) const
{
	if (CollisionBoxY(box.pos + Point(box.height - 1, 0), box.width) == true or CollisionBoxY(box.pos, box.width) == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool TileMap::TestCollisionWallDown(const AABB& box) const
{
	if (CollisionY(box.pos + Point(box.height - 1, 0), box.width) == true or CollisionY(box.pos, box.width) == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool TileMap::MoveBoxDown(const AABB& box) 
{
	bool canMove = BoxDown(box.pos, box.height);
	if (canMove) {
		// Realizar el movimiento de la caja hacia abajo
		// Luego actualizar su estado en el mapa
		int x = box.pos.x / TILE_SIZE;                // Mantiene la misma columna
		int y = (box.pos.y + box.height) / TILE_SIZE; // Calcula la fila siguiente
		UpdateBoxState(x, y);
	}
	return canMove;
}
bool TileMap::TestBoxDown(const AABB& box) const
{
	if (CollisionBoxY(box.pos + Point(box.height - 1, 0), box.width) == true or CollisionBoxY(box.pos, box.width) == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool TileMap::CollisionX(const Point& p, int distance) const
{
	int x, y, y0, y1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y0 = p.y / TILE_SIZE;
	y1 = (p.y + distance - 1) / TILE_SIZE;

	//Iterate over the tiles within the vertical range
	for (y = y0; y <= y1; ++y)
	{
		//One solid tile is sufficient
		if (IsTileSolid(GetTileIndex(x, y)))
			return true;
	}
	return false;
}
bool TileMap::CollisionBoxX(const Point& p, int distance) const
{
	int x, y, y0, y1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y0 = p.y / TILE_SIZE;
	y1 = (p.y + distance - 1) / TILE_SIZE;

	//Iterate over the tiles within the vertical range
	for (y = y0; y <= y1; ++y)
	{
		//One solid tile is sufficient
		if (IsTileBox(GetTileIndex(x, y)))
			return true;
	}
	return false;
}
bool TileMap::CollisionY(const Point& p, int distance) const
{
	int x, y, y0, y1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y0 = p.y / TILE_SIZE;
	y1 = (p.y + distance - 1) / TILE_SIZE;

	//Iterate over the tiles within the vertical range
	for (y = y0; y <= y1; ++y)
	{
		//One solid tile is sufficient
		if (IsTileSolid(GetTileIndex(x, y)))
			return true;
	}
	return false;
}
bool TileMap::CollisionBoxY(const Point& p, int distance) const
{
	int x, y, y0, y1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y0 = p.y / TILE_SIZE;
	y1 = (p.y + distance - 1) / TILE_SIZE;

	//Iterate over the tiles within the vertical range
	for (y = y0; y <= y1; ++y)
	{
		//One solid tile is sufficient
		if (IsTileBox(GetTileIndex(x, y)))
			return true;
	}
	return false;
}
bool TileMap::BoxRight(const Point& p, int distance) const
{
	int x, y, x1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y = p.y / TILE_SIZE;
	x1 = p.x / TILE_SIZE + 2;

	if (IsTileSolid(GetTileIndex(x1, y)))
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool TileMap::BoxLeft(const Point& p, int distance) const
{
	int x, y, x1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y = p.y / TILE_SIZE;
	x1 = p.x / TILE_SIZE - 1;

	if (IsTileSolid(GetTileIndex(x1, y)))
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool TileMap::BoxUp(const Point& p, int distance) const
{
	int x, y, y1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y = p.y / TILE_SIZE;
	y1 = p.y / TILE_SIZE - 1;

	if (IsTileSolid(GetTileIndex(x, y1)))
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool TileMap::BoxDown(const Point& p, int distance) const
{
	int x, y, y1;

	//Calculate the tile coordinates and the range of tiles to check for collision
	x = p.x / TILE_SIZE;
	y = p.y / TILE_SIZE;
	y1 = p.y / TILE_SIZE + 2;

	if (IsTileSolid(GetTileIndex(x, y1)))
	{
		return false;
	}
	else
	{
		return true;
	}
}
void TileMap::Render()
{
	Tile tile;
	Rectangle rc; // Declaración de la variable rc
	Vector2 pos;

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			tile = map[i * width + j];
			if (tile != Tile::AIR)
			{
				pos.x = (float)j * TILE_SIZE;
				pos.y = (float)i * TILE_SIZE;

				if (tile != Tile::COMPLETION)
				{
					rc = dict_rect[(int)tile];
					DrawTextureRec(*img_tiles, rc, pos, WHITE);
				}
			}
		}
	}
	DrawTexture(*img_hud, 5, 7, WHITE);
	//DrawTexture(*map->img_box, 5, 7, WHITE);
	//DrawBox();
}

void TileMap::DrawBox(int x, int y) const
{
	DrawTexture(*img_box, x, y, WHITE);
}

void TileMap::DrawWin() const
{
	DrawTexture(*img_win, 0, 0, WHITE);
}

void TileMap::DrawLose() const
{
	DrawTexture(*img_lose, 0, 0, WHITE);
}

void TileMap::Release()
{
	ResourceManager& data = ResourceManager::Instance();
	data.ReleaseTexture(Resource::IMG_TILES);
	data.ReleaseTexture(Resource::IMG_HUD);
	data.ReleaseTexture(Resource::IMG_BOX);
	data.ReleaseTexture(Resource::IMG_WIN);
	data.ReleaseTexture(Resource::IMG_LOSE);

	dict_rect.clear();
}