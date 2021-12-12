#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include "Collisions.h"
#include "EnemyFlyer.h"
#include "Pathfinding.h"
#include "Map.h"
#include "Scene.h"
#include "EntityManager.h"
#include "EnemyWalker.h"
#include "Particles.h"

Enemy::Enemy(iPoint pos, ENEMY_TYPE type_) : Entity(ENTITY_TYPE::ENEMY_ENT), enemy_position(pos)
{
	name.create("Enemy");
}

bool Enemy::LoadEnemy(const char*file_name, pugi::xml_document &EnemiesDocument) {

	EnemySettings = EnemiesDocument.child("config");

	//Starting Position & Velocities
	initial_velocity.x = EnemySettings.child("EnemySettings").child("Velocity").attribute("initial_velocity.x").as_int();
	initial_velocity.y = EnemySettings.child("EnemySettings").child("Velocity").attribute("initial_velocity.y").as_int();

	enemy_velocity.x = EnemySettings.child("EnemySettings").child("Velocity").attribute("velocity.x").as_int();
	enemy_velocity.y = EnemySettings.child("EnemySettings").child("Velocity").attribute("velocity.y").as_int();

	//Detection Range
	Detect_Range.x = EnemySettings.child("EnemySettings").child("Detection_Range").attribute("det_x").as_int();
	Detect_Range.y = EnemySettings.child("EnemySettings").child("Detection_Range").attribute("det_y").as_int();


	enemy_rect.x = 0;
	enemy_rect.y = 0;
	enemy_rect.w = EnemySettings.child("image").attribute("w").as_int();
	enemy_rect.h = EnemySettings.child("image").attribute("h").as_int();

	enemy_Collider_rect.w= EnemySettings.child("EnemySettings").child("Collider").attribute("w").as_int();
	enemy_Collider_rect.h = EnemySettings.child("EnemySettings").child("Collider").attribute("h").as_int();

	entity_collider = App->collisions->AddColliderEntity({ enemy_Collider_rect.x, enemy_rect.y,enemy_Collider_rect.w ,enemy_Collider_rect.h }, COLLIDER_ENEMY, this);

	Enemy_tex = App->tex->Load(file_name);

	return true;
}

void Enemy::LoadPushbacks(pugi::xml_node node, Animation &animation) {

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

void Enemy::OnCollision(Collider *c1, Collider *c2) {

	List_item<Entity*>* item = App->entity_manager->entities_list.start;

	for (; item != nullptr; item = item->next) {

		if (c1->type == COLLIDER_ENEMY && c2->type == COLLIDER_PLAYER_BULLET) {

			if (Last_collided == c2) 
				return;
			
			life -= 5;
			Last_collided = c2;
			App->particles->AddParticle(App->particles->Blood, enemy_position.x, enemy_position.y, COLLIDER_NONE, fPoint(0, 0), 1.5f, SDL_FLIP_NONE);	
		}
		if (item->data->type == ENTITY_TYPE::ENEMY_ENT ) {

			if (c2->type == COLLIDER_STATIC || (c2->type == COLLIDER_BLINKING && App->map->TriggerActive == true)) {

				int error_margin = 0;

				if (Current_Direction == RIGHT)
					error_margin = (c1->rect.x + c1->rect.w) - c2->rect.x;
				else if (Current_Direction == LEFT)
					error_margin = (c2->rect.x + c2->rect.w) - c1->rect.x;


				if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y >= c2->rect.y + c2->rect.h - enemy_velocity.y) {

					enemy_velocity.y = 0;
					enemy_position.y = c1->rect.y + c2->rect.h - (c1->rect.y - c2->rect.y) + 3;
				}
				else if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + enemy_velocity.y) {
					onGround = true;
					enemy_velocity.y = 0;
					enemy_position.y = c1->rect.y - ((c1->rect.y + c1->rect.h) - c2->rect.y);

				}
			

				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + enemy_velocity.x) { 

					enemy_velocity.x = 0;
					enemy_position.x -= (c1->rect.x + c1->rect.w) - c2->rect.x + 4;
					CollidingX = true;
					Y_Collider_Pos = c2->rect.y;

				}
				else if (c1->rect.x <= c2->rect.x + c2->rect.w && c1->rect.x >= c2->rect.x + c2->rect.w - enemy_velocity.x) { 

					enemy_velocity.x = 0;
					enemy_position.x += (c2->rect.x + c2->rect.w) - c1->rect.x + 4;
					CollidingX = true;
					Y_Collider_Pos = c2->rect.y;

				}
			}
		}
	}
}

Enemy* Enemy::CreateEnemy(iPoint pos, ENEMY_TYPE enemyType, const char* path, pugi::xml_document &EnemiesDocument) {

	SString tmp("maps\\%s", path);

	static_assert(ENEMY_TYPE::UNKNOWN == ENEMY_TYPE(2), "UPDATE ENEMY TYPES");
	Enemy* Enemy2 = nullptr;
	switch (enemyType) {

	case ENEMY_TYPE::FLYER:
		Enemy2 = new EnemyFlyer(pos, tmp.GetString(), EnemiesDocument);
		break;
	case ENEMY_TYPE::WALKER:
		Enemy2 = new EnemyWalker(pos, tmp.GetString(), EnemiesDocument);
		break;
	default:
		break;
	}

	App->entity_manager->entities_list.add(Enemy2);

	return Enemy2;
}


bool Enemy::Load(pugi::xml_node& data) {
	
	enemy_position.x = data.attribute("posX").as_int();
	enemy_position.y = data.attribute("posY").as_int();
	return true;
}


bool Enemy::Save(pugi::xml_node& data) const {

	data.append_attribute("posX") = enemy_position.x;
	data.append_attribute("posY") = enemy_position.y;
	return true;
}