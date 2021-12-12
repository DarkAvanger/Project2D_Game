#include "EntityManager.h"
#include "Log.h"
#include "Entity.h"
#include "Scene.h"
#include "Collisions.h"


EntityManager::EntityManager()
{
	name.create("entities");
}


EntityManager::~EntityManager() {}

bool EntityManager::Awake() {

	LOG("AWAKING ENTITY MANAGER");
	times_per_sec = TIMES_PER_SEC;
	paths_per_sec = PATHFINDING_PER_SEC;

	update_ms_cycle = 1.0f / (float)times_per_sec;
	update_enemies = 1.0f / (float)paths_per_sec;

	return true;
}

bool EntityManager::Start() {

	List_item<Entity*>* item;
	item = entities_list.start;

	while (item != nullptr) {

		item->data->Start();
		item = item->next;
	}

	return true;
}

bool EntityManager::PreUpdate() {

	do_logic = false;
	return true;
}

bool EntityManager::Update(float dt) {


	accumulated_time += dt;
	accumulated_time_enemies += dt;

	if (accumulated_time >= update_ms_cycle)
		do_logic = true;

	if (accumulated_time_enemies >= update_enemies)
		do_enemies_logic = true;

	if (do_logic == true) {

		List_item<Entity*>*item = entities_list.start;
		for (; item != nullptr; item = item->next)
			if(item->data->type != ENTITY_TYPE::ENEMY_ENT)
				item->data->FixUpdate(dt);
	
	}

	if (do_enemies_logic = true) {

		List_item<Entity*>*item = entities_list.start;
		for (; item != nullptr; item = item->next)
			if (item->data->type == ENTITY_TYPE::ENEMY_ENT)
				item->data->FixUpdate(dt);

	}

	accumulated_time -= update_ms_cycle;
	accumulated_time_enemies -= update_enemies;

	List_item<Entity*>*item = entities_list.start;
	for (; item != nullptr; item = item->next)
		item->data->Update(dt);
	
	return true;
}

bool EntityManager::CleanUp() {

	LOG("Clean Up Entity Manager");

	List_item<Entity*>* item;
	item = entities_list.start;

	while (item != nullptr) {

		if (item->data->entity_collider != nullptr) {
			item->data->entity_collider->to_delete = true;
		}

		RELEASE(item->data);
		item = item->next;
	}

	entities_list.clear();

	return true;
}

Entity *EntityManager::CreateEntity(ENTITY_TYPE eType) {

	static_assert(ENTITY_TYPE::UNKNOWN == ENTITY_TYPE(3), "UPDATE ENTITY TYPES");
	
	Entity* Entity2 = nullptr;

	switch (eType) {

	case ENTITY_TYPE::ENEMY_ENT:
		Entity2 = new Entity(ENTITY_TYPE::ENEMY_ENT);
		break;
	case ENTITY_TYPE::PLAYER_ENT:
		Entity2 = new Entity(ENTITY_TYPE::PLAYER_ENT);
		break;
	default:
		break;

	}
	
	entities_list.add(Entity2);
	return Entity2;
}


void EntityManager::DestroyEntity(Entity *Entity2) {

	List_item<Entity*>*item = entities_list.start;

	while (item != nullptr) {

		if (item->data == Entity2) {
			if(Entity2->entity_collider!=nullptr)
				Entity2->entity_collider->to_delete = true;
			entities_list.del(item);
			break;
		}

		item = item->next;
	}
}

void EntityManager::DesrtroyEnemies() {

	List_item<Entity*>*item = App->entity_manager->entities_list.start;

	while (item != nullptr) {
		if (item->data->type == ENTITY_TYPE::ENEMY_ENT) {
			if (item->data->entity_collider != nullptr)
				item->data->entity_collider->to_delete = true;
			App->entity_manager->entities_list.del(item);
		}
		item = item->next;
	}
}


void EntityManager::LoadSceneEnemeies(pugi::xml_node &Enemy2, ENEMY_TYPE type, const char* path, pugi::xml_document &EnemiesDocument) {

	pugi::xml_node pos;

	for (pos = Enemy2.child("Position"); pos; pos = pos.next_sibling("Position")) {

		int x = pos.attribute("x").as_int();
		int y = pos.attribute("y").as_int();

		Enemy* Enemy2 = nullptr;
		Enemy2->CreateEnemy(iPoint(x, y), type, path, EnemiesDocument);
	}
}


bool EntityManager::Load(pugi::xml_node &data) {

	bool ret = true;
	
	List_item<Entity*>* item2 = entities_list.start;
	ret = true;

	while (item2 != NULL && ret == true) {

		if (item2->data->type != ENTITY_TYPE::ENEMY_ENT) {

			item2->data->Load(data.child(item2->data->name.GetString()));
			item2 = item2->next;

		}

		item2 = item2->next;
	}

	List_item<Entity*>* enemy_item = entities_list.start;
	ret = true;
	pugi::xml_node node = data.child("Enemy");
	
	while (enemy_item != NULL && ret == true) {
	
		if (enemy_item->data->type == ENTITY_TYPE::ENEMY_ENT) {

			enemy_item->data->Load(node);
			node = node.next_sibling("Enemy");

		}

		enemy_item = enemy_item->next;
	}
	
	return ret;
}


bool EntityManager::Save(pugi::xml_node &data) const {

	bool ret = false;

	List_item<Entity*>* item2 = entities_list.start;
	ret = true;

	while (item2 != NULL && ret == true) {

		item2->data->Save(data.append_child(item2->data->name.GetString()));
		item2 = item2->next;
	}

	return ret;
}