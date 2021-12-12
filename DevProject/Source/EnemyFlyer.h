#ifndef _ENEMY_FLYER_
#define _ENEMY_FLYER_

#include "Enemy.h"
#include "Animation.h"
#include "Audio.h"

class EnemyFlyer : public Enemy
{

public:

	EnemyFlyer(iPoint pos, const char* path, pugi::xml_document &EnemiesDocument);
	~EnemyFlyer();

	void Update(float dt) override;
	void FixUpdate(float dt) override;
	void Draw(float dt);


	bool Detect_Area() override;
	bool Exploding_Area();
	bool Start_exploding = false;

	void Move(DynArray<iPoint>&path, float dt);

public:

	DynArray<iPoint>* enemy_path = nullptr;
	
	SDL_Rect pathrect;

	iPoint Detect_Exploding_Range;
	Timer Explosion_Time;


private:

	Animation* current_animation = nullptr;
	Animation Idle;
	Animation Exploding;
	Animation Dead;
	uint explosion;

};

#endif