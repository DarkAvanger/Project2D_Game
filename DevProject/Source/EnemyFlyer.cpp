#include "EnemyFlyer.h"
#include "Scene.h"
#include "Collisions.h"
#include "Render.h"
#include "Player.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Particles.h"
#include "EntityManager.h"


EnemyFlyer::EnemyFlyer(iPoint pos, const char* path, pugi::xml_document &EnemiesDocument) : Enemy(enemy_position, ENEMY_TYPE::FLYER) {

	enemy_position = pos;
	LoadEnemy(path, EnemiesDocument);

	Animation_node = EnemiesDocument.child("config").child("AnimationCoords").child("Idle");
	LoadPushbacks(Animation_node, Idle);

	Animation_node = EnemiesDocument.child("config").child("AnimationCoords").child("Exploding");
	LoadPushbacks(Animation_node, Exploding);

	Animation_node = EnemiesDocument.child("config").child("AnimationCoords").child("Dead");
	LoadPushbacks(Animation_node, Dead);

	Current_Direction = LEFT;
	current_animation = &Idle;

	Detect_Exploding_Range.x = EnemySettings.child("EnemySettings").child("Detection_Range").attribute("det_exp_x").as_int();
	Detect_Exploding_Range.y = EnemySettings.child("EnemySettings").child("Detection_Range").attribute("det_exp_y").as_int();
  
	explosion = App->audio->LoadFx("audio/fx/Explosion.wav");
	Start_exploding = false;
	life = 20;

}

EnemyFlyer::~EnemyFlyer() {}

void EnemyFlyer::FixUpdate(float dt) {


	if (Detect_Area() && life > 0) {

		iPoint initial_pos = App->map->WorldToMap(enemy_position.x, (enemy_position.y + 15), App->scene->current_pathfinding_map);
		iPoint final_pos = App->map->WorldToMap(App->scene->Player->player_position.x, App->scene->Player->player_position.y, App->scene->current_pathfinding_map);

		if (App->pathfinding->IsWalkable(initial_pos) && App->pathfinding->IsWalkable(final_pos)) {

			enemy_path = App->pathfinding->CreatePath(initial_pos, final_pos);
			Move(*enemy_path, dt);
		}
		
	}
	else if (enemy_path != nullptr)
		enemy_path->Clear();

	entity_collider->SetPos(enemy_position.x + 40, enemy_position.y);
}

void EnemyFlyer::Update(float dt) {


	if (enemy_path != nullptr && App->collisions->debug) {

		for (uint i = 0; i < enemy_path->Count(); ++i) {

			iPoint pos = App->map->MapToWorld(enemy_path->At(i)->x, enemy_path->At(i)->y, App->scene->current_pathfinding_map);
			pathrect.x = pos.x;
			pathrect.y = pos.y;
			pathrect.w = App->scene->current_map.width;
			pathrect.h = App->scene->current_map.height;
			App->render->DrawQuad(pathrect, 0, 0, 255, 100);
		}
	}

			
	Draw(dt);

	if (life <= 0)
		App->entity_manager->DestroyEntity(this);
	
}

bool EnemyFlyer::Detect_Area() {
	
	if ((App->scene->Player->player_position.x >= enemy_position.x - Detect_Range.x && App->scene->Player->player_position.x <= enemy_position.x + Detect_Range.x)
		&& (App->scene->Player->player_position.y >= enemy_position.y - Detect_Range.y && App->scene->Player->player_position.y <= enemy_position.y + Detect_Range.y)) {
		
		return true;
	}

	return false;
}


void EnemyFlyer::Move(DynArray<iPoint>&path, float dt) {

	Current_Direction = App->pathfinding->current_Direction(path);

	if (CollidingX && Detect_Area() && (RIGHT || LEFT || UP_RIGHT || UP_LEFT)) {

		if (enemy_position.y + enemy_Collider_rect.h >= Y_Collider_Pos) 
				Current_Direction = UP;
			CollidingX = false;
	}

	switch (Current_Direction) {

	case UP_RIGHT:
		enemy_position.x += enemy_velocity.x;
		enemy_position.y -= enemy_velocity.y;	
		break;
	case UP_LEFT:
		enemy_position.x -= enemy_velocity.x;
		enemy_position.y -= enemy_velocity.y;
		break;
	case DOWN_RIGHT:
		enemy_position.x += enemy_velocity.x;
		enemy_position.y += enemy_velocity.y;	
		break;
	case DOWN_LEFT:
		enemy_position.x -= enemy_velocity.x;
		enemy_position.y += enemy_velocity.y;
		break;
	case RIGHT:
		enemy_position.x += enemy_velocity.x;
		break;
	case LEFT:
		enemy_position.x -= enemy_velocity.x;
		break;
	case UP:
		enemy_position.y -= enemy_velocity.y;
		break;
	case DOWN:
		enemy_position.y += enemy_velocity.y;
		break;
	}

	if (!Exploding_Area()) {

		current_animation = &Idle;
		enemy_velocity = initial_velocity;
	}

	else {
    
		Start_exploding = false;
		current_animation = &Exploding;

		enemy_velocity = initial_velocity;

		if (Explosion_Time.ReadSec() >= 0.5f) {
			current_animation = &Dead;
		}
		if (Explosion_Time.ReadSec() > 1) {

			App->audio->PlayFx(explosion);

			App->render->DoCameraShake = true;
			App->render->power = 7.0f;
			App->render->Time_Doing_Shake = 1.0f;
			PERF_START(App->render->CameraShake_Time);
			life = 0;
			if (App->scene->Player->player_position.x >= enemy_position.x - 100 && App->scene->Player->player_position.x <= enemy_position.x + 100
				&& App->scene->Player->player_position.y >= enemy_position.y - 100 && App->scene->Player->player_position.y <= enemy_position.y + 100) {
				App->scene->Player->life -= 20;
			}

			App->particles->AddParticle(App->particles->Plasma_Explosion, enemy_position.x - 90, enemy_position.y - 70, COLLIDER_NONE, fPoint(0,0), 1.5f, SDL_FLIP_NONE);
			App->particles->AddParticle(App->particles->Plasma_Explosion, enemy_position.x - 150, enemy_position.y - 160, COLLIDER_NONE, fPoint(0, 0), 1.9f, SDL_FLIP_NONE, 350.0f);
			App->particles->AddParticle(App->particles->Plasma_Explosion, enemy_position.x - 70, enemy_position.y - 200, COLLIDER_NONE, fPoint(0, 0), 1.9f,SDL_FLIP_NONE,200.0f);
		}
	}

	if (Start_exploding == true) 
		PERF_START(Explosion_Time);
	
}

void EnemyFlyer:: Draw(float dt) {

	if( Current_Direction == RIGHT || Current_Direction == UP_RIGHT||Current_Direction==DOWN_RIGHT|| Current_Direction== UP|| Current_Direction==DOWN|| Current_Direction==NONE)
		App->render->Blit(Enemy_tex, enemy_position.x, enemy_position.y, &current_animation->GetCurrentFrame(dt), 1, 0, 0, 0, SDL_FLIP_NONE, 0.5);
	
	else if (Current_Direction ==LEFT||Current_Direction ==UP_LEFT || Current_Direction==DOWN_RIGHT) 
		App->render->Blit(Enemy_tex, enemy_position.x, enemy_position.y, &current_animation->GetCurrentFrame(dt), 1, 0, 0, 0, SDL_FLIP_HORIZONTAL, 0.5);
	
}


bool  EnemyFlyer::Exploding_Area() {

	if ((App->scene->Player->player_position.x >= enemy_position.x - Detect_Exploding_Range.x && App->scene->Player->player_position.x <= enemy_position.x + Detect_Exploding_Range.x)
		&& (App->scene->Player->player_position.y >= enemy_position.y - Detect_Exploding_Range.y && App->scene->Player->player_position.y <= enemy_position.y + Detect_Exploding_Range.y)) {
		
		return true;
	}
  
	Start_exploding = true;
	return false;
	
}