#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Map.h"
#include "Player.h"
#include "Enemy.h"

struct SDL_Texture;
struct MapData;
class Player;

enum LEVELS {

	MAIN_MENU = 0,
	LEVEL1,
	LEVEL2,
	NO_LEVEL,
	NO_CHANGE

};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Used to iterate between Levels
	void IterateLevel(int level_change);

	//Used to change the actual level
	void ChangeLevel(int level_change);

	//Load Map Objects
	bool LoadObjects(const char*file_name);

public:

	bool Level1 = false;
	bool Level2 = true;
	bool Main_Menu = true;
	bool changing_same_Level = false;
	bool Change_Level = false;
	
	int LevelIterator = 0;

	LEVELS currentLevel = NO_LEVEL;

	MapData current_map;
	MapData current_pathfinding_map;

	//Player
	Player *Player = nullptr;

private:

	MapData Intro_map;
	MapData Level1_map;
	MapData Level2_map;

	MapData Level1_pathfinding_map;
	MapData Level2_pathfinding_map;

	bool pathfinding = false;

	pugi::xml_document	EnemiesDocument;
	pugi::xml_document	SceneDocument;
	pugi::xml_node		music_node;

	pugi::xml_document EnemiesPositions;
	pugi::xml_node en_pos;
	

};

#endif // __SCENE_H__