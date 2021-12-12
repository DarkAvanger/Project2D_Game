#include "Player.h"
#include "Render.h"
#include "Input.h"
#include "Scene.h"
#include "Collisions.h"
#include "Map.h"
#include "Animation.h"
#include "Textures.h"
#include "Pathfinding.h"
#include "Timer.h"
#include "Particles.h"
#include "Fade.h"
#include "Player.h"
#include "EntityManager.h"

#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )


Player::Player(iPoint pos) : Entity(ENTITY_TYPE::PLAYER_ENT), player_position(pos) {
  
	name.create("player");
	pugi::xml_parse_result result = PlayerDocument.load_file("PlayerSettings.xml");

	if (result == NULL)
		LOG("The xml file containing the player tileset fails. Pugi error: %s", result.description());

	PlayerSettings = PlayerDocument.child("config");
	
	kunai = App->audio->LoadFx("audio/fx/Kunai1.wav");
	button = App->audio->LoadFx("audio/fx/Button.wav");
	checkpoint = App->audio->LoadFx("audio/fx/Checkpoint.wav");

	life = 40;
	
}

Player::~Player() {}

void Player::CleanUp() { 

	LOG("Cleaning Up Player");

	if (current_animation != nullptr)
		current_animation = nullptr;

	App->tex->UnLoad(Player_texture);
	active = false;

}


void Player::Start() {

	LoadPlayer("Character_tileset.png");

}


void Player::FixUpdate(float dt) {

	HandleInput(dt);

	if (life > 0) {

		//Kunai
		if ((!jump || !fall) && state != pl_RUN) {

			Kunai_Run.Reset();
			current_animation = &Idle;
			Kunai_current_animation = &Kunai_Idle;

			//Kunai ADJUST
			Adjusting_Kunai_position.x = 0;
			Adjusting_Kunai_position.y = player_rect.h / 3;

		}

		//SHOOT
		if (Shooting) {

			Kunai_current_animation = &Kunai_Shot;

			if (Kunai_Shot.Finished()) {
				Shooting = false;
				Kunai_Shot.Reset();
			}

			Adjusting_Kunai_position.x = 2;
			Adjusting_Kunai_position.y = player_rect.h / 3.5f;
		}


		if (desaccelerating == true) {

			if (acceleration.x > 0) {

				acceleration.x -= 0.2f;

				if (direction_x == pl_RIGHT)
					player_position.x += acceleration.x;

				else if (direction_x == pl_LEFT)
					player_position.x -= acceleration.x;
			}
			else
				desaccelerating = false;

		}

		//JUMP
		if (jump || doublejump) {

			if (jump)
				current_animation = &Jump;

			if (player_velocity.y >= 0 && !jump_falling) {

				if (direction_x == pl_RIGHT) {

					if (angle < 0)
						angle *= (-1);

					if (angle < 20)
						angle += 1;

				}
				else if (direction_x == pl_LEFT) {

					if (angle > 0)
						angle *= (-1);

					if (angle > -20)
						angle -= 1;

				}

				fall = false;
				player_position.y -= player_velocity.y;
				player_velocity.y -= 0.32f;
			}

			else if (player_velocity.y < 0) {

				jump_falling = true;
				fall = true;

			}
		}

		if (!jump)
			doublejump = true;


		//FALL
		if (fall) {

			Jump.Reset();

			current_animation = &Fall;
			if (player_velocity.y < initial_vel.y) {

				if (direction_x == pl_RIGHT) {

					if (angle < 0)
						angle *= (-1);

					if (angle > 0)
						angle -= 1;

				}

				else if (direction_x == pl_LEFT) {

					if (angle > 0)
						angle *= (-1);

					if (angle < 0)
						angle += 1;

				}

				player_velocity.y += acceleration.y;
				acceleration.y += 0.2f;
			}

			player_position.y += player_velocity.y;
		}


		//GOD MODE
		if (God) {

			LOG("GOD MODE ACTIVE");
			jump = true;
			fall = false;

		}
		else {
			fall = true;
		}
	}

	else if (life <= 0) {

		current_animation = &Dead;
		Kunai_current_animation = &Kunai_None;
		if(Dead.Finished())
			App->entity_manager->DestroyEntity(this);

	}

}


void Player::Update(float dt) {

	Draw(dt);

}

void Player::Draw(float dt) {


	if (direction_x == pl_RIGHT) {

		entity_collider->SetPos(player_position.x, player_position.y);
		App->render->Blit(Player_texture, player_position.x, player_position.y, &(current_animation->GetCurrentFrame(dt)), 1, 0, 0, 0, SDL_FLIP_NONE, 0.4f);
		App->render->Blit(Player_texture, player_position.x + Adjusting_Kunai_position.x, player_position.y + Adjusting_Kunai_position.y, &(Kunai_current_animation->GetCurrentFrame(dt)), 1, angle, 0, 0, SDL_FLIP_NONE, 0.4f);
	}

	if (direction_x == pl_LEFT) {

		entity_collider->SetPos(player_position.x, player_position.y);
		App->render->Blit(Player_texture, player_position.x, player_position.y, &(current_animation->GetCurrentFrame(dt)), 1, 0, 0, 0, SDL_FLIP_HORIZONTAL, 0.4f);
		App->render->Blit(Player_texture, player_position.x + Adjusting_Kunai_position.x - 30, player_position.y + Adjusting_Kunai_position.y, &(Kunai_current_animation->GetCurrentFrame(dt)), 1, angle, 60, 0, SDL_FLIP_HORIZONTAL, 0.4f);
	}

}

void Player::HandleInput(float dt) {
	

	//God mode
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		God = !God;

	//RIGHT
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

		state = State::pl_RUN;

		desaccelerating = false;

		direction_x = pl_RIGHT;

		player_position.x += (player_velocity.x + acceleration.x);

		if (acceleration.x <= MaxVelocity.x)
			acceleration.x += 0.2f;


		Kunai_current_animation = &Kunai_Run;
		current_animation = &Run;

		//Kunai ADJUST
		Adjusting_Kunai_position.x = 0;
		Adjusting_Kunai_position.y = player_rect.h / 3.5f;

		if (!jump)
			angle = 0;
	}

	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP && !jump && !desaccelerating)
		desaccelerating = true;

	else
		state = State::pl_IDLE;

	//LEFT
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

		state = State::pl_RUN;

		desaccelerating = false;

		direction_x = pl_LEFT;

		player_position.x -= player_velocity.x + acceleration.x;

		if (acceleration.x < MaxVelocity.x)
			acceleration.x += 0.2f;

		Kunai_current_animation = &Kunai_Run;
		current_animation = &Run;

		//Kunai ADJUST
		Adjusting_Kunai_position.x = -5;
		Adjusting_Kunai_position.y = player_rect.h / 3.5f;

		if (!jump)
			angle = 0;

	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP && !jump && !desaccelerating)
		desaccelerating = true;

	if (!God) {

		//JUMP
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && !jump && !God && !fall) {

			jump = true;
			jump_falling = false;

			acceleration.y = 0.2f;
			auxY = player_position.y;
			player_velocity.y = initial_vel.y;
		}

		else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && jump&& doublejump && !God) {

			doublejump = false;
			jump_falling = false;

			acceleration.y = 0.2f;
			auxY = player_position.y;
			player_velocity.y = initial_vel.y;
		}
	}
	else {

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {

			player_velocity.y = initial_vel.y;
			player_position.y -= player_velocity.y;
		}

		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {

			player_velocity.y = initial_vel.y;
			player_position.y += player_velocity.y;
		}

	}

	//Kunai
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !Shooting) {

		angle = 0;
		App->audio->PlayFx(kunai);

		if (direction_x == pl_RIGHT) {

			App->particles->AddParticle(App->particles->Player_Shoot, player_position.x + Adjusting_Kunai_position.x + 20, player_position.y + Adjusting_Kunai_position.y - 14, COLLIDER_PLAYER_BULLET, fPoint(1000, 0), 0.8f); 
			App->particles->AddParticle(App->particles->Player_Shoot_Beam, player_position.x + Adjusting_Kunai_position.x + 20, player_position.y - 18, COLLIDER_NONE, fPoint(player_position.x - player_position.x + acceleration.x, 0), 1.1f);
		}

		else if (direction_x == pl_LEFT) {

			App->particles->AddParticle(App->particles->Player_Shoot, player_position.x + Adjusting_Kunai_position.x - 80, player_position.y + Adjusting_Kunai_position.y - 14, COLLIDER_PLAYER_BULLET, fPoint(-1000, 0), 0.8f, SDL_FLIP_HORIZONTAL); 
			App->particles->AddParticle(App->particles->Player_Shoot_Beam, player_position.x + Adjusting_Kunai_position.x - 80, player_position.y - 18, COLLIDER_NONE, fPoint(player_position.x - player_position.x - acceleration.x, 0), 1.1f);
		}

		Shooting = true;
	}
}

bool Player::Load(pugi::xml_node& data)
{
	player_position.x = data.child("player").attribute("x").as_int();
	player_position.y = data.child("player").attribute("y").as_int();
	life = data.child("player").attribute("life").as_int();

	return true;
}

// Save Game
bool Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node pl = data.append_child("player");

	pl.append_attribute("x") = player_position.x;
	pl.append_attribute("y") = player_position.y;
	pl.append_attribute("life") = life;

	return true;
}

void Player::OnCollision(Collider *c1, Collider *c2) {

	if (c2->type == COLLIDER_ENEMY_BULLET) {

		if (Last_collided == c2) 
			return;
		
		if (App->render->DoCameraShake == false) {
			App->render->DoCameraShake = true;
			App->render->power = 3.0f;
			App->render->Time_Doing_Shake = 0.3f;
			PERF_START(App->render->CameraShake_Time);
		}

		life -= 5;
		Last_collided = c2;
		App->particles->AddParticle(App->particles->Blood, player_position.x-35, player_position.y-30, COLLIDER_NONE, fPoint(0, 0), 1.5f, SDL_FLIP_NONE);

	}
	if (c2->type == COLLIDER_STATIC || (c2->type == COLLIDER_BLINKING && App->map->TriggerActive == true)) {
		
		int error_margin = 0;

		if (direction_x == pl_RIGHT)
			error_margin = (c1->rect.x + c1->rect.w) - c2->rect.x;
		else if (direction_x == pl_LEFT)
			error_margin = (c2->rect.x + c2->rect.w) - c1->rect.x;

		if (error_margin > 1) {

			if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y >= c2->rect.y + c2->rect.h - player_velocity.y - acceleration.y) { 
			
				fall = false;
				doublejump = false;
				player_velocity.y = 0.0f;
				acceleration.y = 0.2f;
				player_position.y = c1->rect.y + c2->rect.h - (c1->rect.y - c2->rect.y) + 3;
			}
			else if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + player_velocity.y + acceleration.y) { 
				
				fall = false;
				jump = false;
				player_velocity.y = 0.0f;
				acceleration.y = 0.2f; 
				player_position.y = c1->rect.y - ((c1->rect.y + c1->rect.h) - c2->rect.y);
			}
		}

		if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + player_velocity.x + acceleration.x) { 
			
			desaccelerating = false;
			player_velocity.x = 0.0f;
			acceleration.x /= 1.1f; 
			player_position.x -= (c1->rect.x + c1->rect.w) - c2->rect.x + 1;

		}
		else if (c1->rect.x <= c2->rect.x + c2->rect.w && c1->rect.x >= c2->rect.x + c2->rect.w - player_velocity.x-acceleration.x) { 
			
			desaccelerating = false;
			player_velocity.x = 0.0f;
			acceleration.x /= 1.1f; 
			player_position.x += (c2->rect.x + c2->rect.w) - c1->rect.x + 1;

		}
	}

	if (!God) {

		if (c1->type == COLLIDER_FALL || c2->type == COLLIDER_FALL) 
			App->LoadGame("save_game.xml");
	}

	if (c1->type == TRIGGER_PUSH || c2->type == TRIGGER_PUSH) { 
		
		App->map->TriggerActive = true;
		App->audio->PlayFx(button);
	}

	if (c1->type == TRIGGER_PUSHOFF || c2->type == TRIGGER_PUSHOFF) {
	
		App->map->TriggerActive = false;
		App->audio->PlayFx(checkpoint);
	}

	if (c1->type == TRIGGER_WIN || c2->type == TRIGGER_WIN) {

		App->map->TriggerActive = false;
	}
}

void Player::LoadPushbacks(pugi::xml_node node, Animation &animation) {

	animation.loop = node.attribute("loop").as_bool();
	animation.speed = node.attribute("speed").as_float();
	SDL_Rect rect;
	
	for (node = node.child("Pushback"); node; node = node.next_sibling("Pushback")) {

		rect.x = node.attribute("x").as_int();
		rect.y = node.attribute("y").as_int();
		rect.w = node.attribute("w").as_int();
		rect.h = node.attribute("h").as_int();
		animation.PushBack({ rect });
	}
}


Player* Player::CreatePlayer(iPoint pos) {

	Player* Player2 = nullptr;
	Player2 = new Player(pos);

	App->entity_manager->entities_list.add(Player2);
	return Player2;
}

void Player::LoadPlayer(const char *file_name) {
	
	//Loading Settings
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Idle");
	LoadPushbacks(Animation_node, Idle);
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Run");
	LoadPushbacks(Animation_node, Run);
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Jump");
	LoadPushbacks(Animation_node, Jump);
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Fall");
	LoadPushbacks(Animation_node, Fall);
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Dead");
	LoadPushbacks(Animation_node, Dead);
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Gun").child("None");
	LoadPushbacks(Animation_node, Kunai_None);
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Gun").child("Idle");
	LoadPushbacks(Animation_node, Kunai_Idle);
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Gun").child("Run");
	LoadPushbacks(Animation_node, Kunai_Run);
	Animation_node = PlayerDocument.child("config").child("AnimationCoords").child("Gun").child("Shoot");
	LoadPushbacks(Animation_node, Kunai_Shot);


	PlayerSettings = PlayerDocument.child("config");

	life = PlayerSettings.child("PlayerSettings").child("Life").attribute("life").as_int();

	SString tmp("maps\\%s", file_name);
	Player_texture = App->tex->Load(tmp.GetString());

	acceleration.x = PlayerSettings.child("PlayerSettings").child("Acceleration").attribute("a.x").as_float();
	acceleration.y = PlayerSettings.child("PlayerSettings").child("Acceleration").attribute("a.y").as_float();

	MaxVelocity.x = initial_vel.x = PlayerSettings.child("PlayerSettings").child("MaxVelocity").attribute("velocity.x").as_float();
	initial_vel.x = PlayerSettings.child("PlayerSettings").child("Velocity").attribute("velocity.x").as_float();
	initial_vel.y = PlayerSettings.child("PlayerSettings").child("Velocity").attribute("velocity.y").as_float();
	direction_x = pl_RIGHT;

	if (App->scene->currentLevel == LEVEL1) {

		player_position.x = PlayerSettings.child("PlayerSettings").child("StartingPose").child("Level1").attribute("position.x").as_int();
		player_position.y = PlayerSettings.child("PlayerSettings").child("StartingPose").child("Level1").attribute("position.y").as_int();

		Kunai_position = player_position;
	}

	else if (App->scene->currentLevel == LEVEL2) {

		player_position.x = PlayerSettings.child("PlayerSettings").child("StartingPose").child("Level2").attribute("position.x").as_int();
		player_position.y = PlayerSettings.child("PlayerSettings").child("StartingPose").child("Level2").attribute("position.y").as_int();
	}

	else if (App->scene->currentLevel == MAIN_MENU)
		player_position.x = -200;

	fall = true;

	player_rect.x = player_position.x;
	player_rect.y = player_position.y;
	player_rect.w = PlayerSettings.child("image").attribute("w").as_int();
	player_rect.h = PlayerSettings.child("image").attribute("h").as_int();

	entity_collider = App->collisions->AddColliderEntity({ player_rect.x + 50, player_rect.y, (player_rect.w - 65), (player_rect.h) - 28 }, COLLIDER_PLAYER, this);

	state = State::pl_IDLE;

}