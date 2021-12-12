#ifndef _COLLISIONS_H
#define _COLLISIONS_H

#include "Defs.h"
#include "Log.h"
#include "Point.h"
#include "Module.h"
#include "App.h"
#include "Entity.h"

#define MAX_COLLIDERS 700

enum COLLIDER_TYPE {

	COLLIDER_NONE = -1,
	TRIGGER_WIN,
	COLLIDER_STATIC,
	COLLIDER_FALL,
	COLLIDER_PLAYER,
	COLLIDER_BLINKING,
	TRIGGER_PUSH,
	TRIGGER_PUSHOFF,
	COLLIDER_ENEMY,
	COLLIDER_ENEMY_BULLET,
	COLLIDER_PLAYER_BULLET,
	COLLIDER_MAX=12
};

struct Collider {

	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type = COLLIDER_NONE;
	Module *callback = nullptr;
	Entity *callback2 = nullptr;

	Collider(SDL_Rect rect, COLLIDER_TYPE type, Module *callback = nullptr) :
		rect(rect), type(type), callback(callback) {}

	Collider(SDL_Rect rect, COLLIDER_TYPE type, Entity *callback = nullptr) :
		rect(rect), type(type), callback2(callback) {}
	
	void SetPos(int x, int y) { rect.x = x; rect.y = y; }
	bool CheckCollision(const SDL_Rect &r) const;
	void ChangeCollider(COLLIDER_TYPE type_) { type = type_; }


};

class Collisions : public Module{


public:

	Collisions();
	~Collisions();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	bool PostUpdate();

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

public:

	void DebugDraw();
	Collider* AddCollider(SDL_Rect r, COLLIDER_TYPE type, Module*callback = nullptr);
	Collider* AddColliderEntity(SDL_Rect r, COLLIDER_TYPE type, Entity*callback = nullptr);
	void AssignMapColliders(const char* file_name);

	bool debug = false;

private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];

	
};

#endif