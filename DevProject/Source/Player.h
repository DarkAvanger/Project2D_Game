#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Defs.h"
#include "Log.h"
#include "Point.h"
#include "Entity.h"
#include "App.h"
#include "Animation.h"
#include "DynArray.h"
#include "Audio.h"


enum direction {

	pl_RIGHT,
	pl_LEFT,
};
enum State {

	pl_IDLE,
	pl_RUN,
	pl_JUMP,
	pl_FALL
};


class Player : public Entity
{

public:

	Player(iPoint pos);
	virtual ~Player();

public:

	// Called before the first frame
	void Start() override;

	// Called each loop iteration
	void Update(float dt) override;

	// Called before quitting
	void CleanUp() override;

	void FixUpdate(float dt) override;

public:

	//Pushbacks loading
	void LoadPushbacks(pugi::xml_node node, Animation &animation);

	//Load & Save
	bool Load(pugi::xml_node& data);

	//Save Game 
	bool Save(pugi::xml_node& data) const;

	//Load player 
	void LoadPlayer(const char *file_name);

	void Draw(float dt);
	void HandleInput(float dt);


public:

	void OnCollision(Collider *c1, Collider *c2);
	Player* CreatePlayer(iPoint pos); 

public:

	//Player & Kunai 
	int angle;
	State state;
	bool Shooting;
	bool jump = false;
	bool doublejump = true;
	bool jump_falling = false;
	int auxY;


	iPoint player_position;
	iPoint Kunai_position;
	iPoint Adjusting_Kunai_position;

	int direction_x;

	fPoint player_velocity;
	fPoint MaxVelocity;
	fPoint initial_vel;

	fPoint acceleration;
	bool desaccelerating;
	

	bool active;
	bool fall = true;
	bool God = false;

	Timer desac;

private:

	uint kunai;
	uint button;
	uint checkpoint;

public:

	//Animations Kunai and Player
	Animation*		current_animation = nullptr;
	Animation*		Kunai_current_animation = nullptr;
	Animation		Idle;
	Animation		Run;
	Animation		Jump;
	Animation		Fall;
	Animation		Kunai_Idle;
	Animation		Kunai_Run;
	Animation		Kunai_Shot;
	Animation		Kunai_None;
	Animation		Dead;

	//Textures Kunai and Player
	SDL_Texture*	Player_texture = nullptr;
	SDL_Texture*	Kunai_Texture = nullptr;
	
	SDL_Rect player_rect;
	SDL_Rect Kunai_Rect;

	Collider *Last_collided = nullptr;
	

private :

	pugi::xml_document	PlayerDocument;
	pugi::xml_node		Animation_node;
	pugi::xml_node		PlayerSettings;

};

#endif