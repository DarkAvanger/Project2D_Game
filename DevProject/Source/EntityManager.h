#ifndef _ENTITY_MANAGER
#define _ENTITY_MANAGER

#include "Module.h"
#include "Entity.h"
#include "List.h"
#include "Enemy.h"
#include "PugiXml\src\pugixml.hpp"

#define TIMES_PER_SEC 5
#define PATHFINDING_PER_SEC 3

class EntityManager : public Module
{
public:

	EntityManager();
	~EntityManager();

public:

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:

	Entity *CreateEntity(ENTITY_TYPE eType);
	void DestroyEntity(Entity *Entity2);
	void DesrtroyEnemies();

	void LoadSceneEnemeies(pugi::xml_node &Enemy, ENEMY_TYPE type, const char* path, pugi::xml_document &EnemiesDocument);


public:

	List<Entity*> entities_list;

	float accumulated_time;
	float update_ms_cycle;
	int times_per_sec;

	float accumulated_time_enemies;
	int paths_per_sec;
	float update_enemies;

	bool do_logic;
	bool do_enemies_logic;

	uint score = 0;
	uint coins = 0;
	uint flyer_counter = 0;

};

#endif