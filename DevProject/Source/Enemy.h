#ifndef _ENEMY_
#define _ENEMY_

#include "Entity.h"
#include "DynArray.h"
#include "Animation.h"
#include "List.h"
#include "Pathfinding.h"

enum ENEMY_TYPE {

	NON = -1,
	FLYER,
	WALKER,
	UNKNOWN = 2
};

class Enemy : public Entity
{

public:

	Enemy(iPoint pos, ENEMY_TYPE type_);
	~Enemy() {}
	
public:

	//Create Enemy
	Enemy* CreateEnemy(iPoint pos, ENEMY_TYPE enemyType, const char* path, pugi::xml_document &EnemiesDocument); 

	void Enemy::OnCollision(Collider *c1, Collider *c2) override;

public:

	//Load Enemy
	bool LoadEnemy(const char*file_name, pugi::xml_document &EnemiesDocument);

	//Animations loading
	void LoadPushbacks(pugi::xml_node node, Animation &animation);
	virtual bool Shoot_Area() { return true; };
	

	virtual bool Detect_Area() { return true; };
	virtual void Patrol(float dt) {};
	virtual void Shoot() {};

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;
	

public:

	bool Path_Found = false;
	bool onGround;

	ENEMY_TYPE type;
	Direction Current_Direction;
	Collider* Last_collided = nullptr;
	iPoint Detect_Range;
	

	iPoint enemy_position;
	iPoint enemy_velocity;
	iPoint initial_velocity;

public:

	bool firstiteration;

	SDL_Texture *Enemy_tex = nullptr;
	SDL_Rect enemy_rect;
	SDL_Rect enemy_Collider_rect;

	bool CollidingX = false;
	int Y_Collider_Pos;

public:

	pugi::xml_node		EnemySettings;
	pugi::xml_node		Animation_node;
};

#endif