#ifndef _ENEMY_WALKER_
#define _ENEMY_WALKER_

#include "Enemy.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Audio.h"


class EnemyWalker : public Enemy
{

public:

	EnemyWalker(iPoint pos, const char* path, pugi::xml_document &EnemiesDocument);
	~EnemyWalker();

	void Draw(float dt);
	void Move(DynArray<iPoint>&path, float dt);
	void Update(float dt) override;
	void FixUpdate(float dt) override;

public:

	bool Detect_Area() override;
	bool Shoot_Area() override;
	bool Able_to_Shoot;

	void Patrol(float dt) override;
	void Shoot() override;

	bool Limit_Right_Reached = false;
	bool Limit_Left_Reached = false;

public:

	SDL_Rect pathrect;
	DynArray<iPoint>* enemy_path = nullptr;

	iPoint Detect_Shoot_Range;

	iPoint Patrol_velocity;
	int Patrol_Range[2];

private:

	Animation* current_animation = nullptr;
	Animation Idle;
	Animation Run;
	Animation Shoot_animation;
	Animation Dead;
	uint kunaiEnemy;
};

#endif