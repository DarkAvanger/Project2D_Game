#include "App.h"
#include "Collisions.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Map.h"
#include "Log.h"
#include "Entity.h"


Collisions::Collisions()
{
	name.create("Collisions");

	for (int i = 0; i < MAX_COLLIDERS; i++) 
		colliders[i] = nullptr;

	for (int i = 0; i < COLLIDER_MAX; i++) {
		for (int j = 0; j < COLLIDER_MAX; j++)
			matrix[i][j] = false;
	}

	for (int p = 0; p < COLLIDER_MAX - 1; p++) {

		if (p != COLLIDER_NONE || p != COLLIDER_PLAYER|| p!=COLLIDER_PLAYER_BULLET)
			matrix[COLLIDER_PLAYER][p] = true;
	}

	for (int p = 0; p < COLLIDER_MAX - 1; p++) {

		if (p == COLLIDER_STATIC || p == COLLIDER_PLAYER || p == COLLIDER_BLINKING)
			matrix[COLLIDER_ENEMY_BULLET][p] = true;
	}

	for (int p = 0; p < COLLIDER_MAX - 1; p++) {

		if (p == COLLIDER_STATIC || p == COLLIDER_ENEMY || p == COLLIDER_BLINKING)
			matrix[COLLIDER_PLAYER_BULLET][p] = true;
	}


	matrix[TRIGGER_PUSHOFF][COLLIDER_PLAYER] = true;
	matrix[TRIGGER_PUSH][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_BLINKING][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_FALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_STATIC][TRIGGER_PUSHOFF] = true;
	matrix[COLLIDER_STATIC][COLLIDER_PLAYER] = true;

	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER_BULLET] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_STATIC] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_FALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_BLINKING] = true;

}


Collisions::~Collisions()
{
}

bool Collisions::Awake() {

	LOG("Collisions Module Loading");
	bool ret = true;
	return ret;
}

bool Collisions::Start() {
	
	Init();

	return true;
}


bool Collisions::PreUpdate() {
	bool ret = true;

	for (uint i = 0; i < MAX_COLLIDERS; ++i) {

		if (colliders[i] != nullptr && colliders[i]->to_delete) {

			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return ret;
}


bool Collisions::Update(float dt) {

	
	Collider *c1;
	Collider *c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i) {

		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		for (uint j = 0; j < MAX_COLLIDERS; ++j) {

			if (colliders[j] == nullptr)
				continue;

			c2 = colliders[j];

			if (c1->CheckCollision(c2->rect)) {
					if (matrix[c1->type][c2->type] && c1->callback)
						c1->callback->OnCollision(c1, c2);
					if (matrix[c2->type][c1->type] && c2->callback)
						c2->callback->OnCollision(c2, c1);

					if (matrix[c1->type][c2->type] && c1->callback2)
						c1->callback2->OnCollision(c1, c2);
					if (matrix[c2->type][c1->type] && c2->callback2)
						c2->callback2->OnCollision(c2, c1);
			}
		}
	}
	return true;
}

bool Collisions::PostUpdate() {

	DebugDraw();
	return true;
}

void Collisions::DebugDraw() {

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) //Show colliders
		debug = !debug;

	if (!debug)
		return;

	for (uint i = 0; i < MAX_COLLIDERS; i++) {

		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type) {

		case COLLIDER_NONE:
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, 40);
			break;
		case COLLIDER_STATIC:
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, 40);
			break;
		case COLLIDER_PLAYER:
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, 40);
			break;
		case COLLIDER_FALL:
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, 40);
			break;
		case COLLIDER_BLINKING:
			App->render->DrawQuad(colliders[i]->rect, 0, 200, 200, 40);
			break;
		case TRIGGER_PUSH:
			App->render->DrawQuad(colliders[i]->rect, 200, 0, 200, 40);
			break;
		case TRIGGER_PUSHOFF:
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 200, 40);
			break;
		case TRIGGER_WIN:
			App->render->DrawQuad(colliders[i]->rect, 200 ,100, 255, 40);
			break;
		case COLLIDER_ENEMY:
			App->render->DrawQuad(colliders[i]->rect, 200, 200, 200, 40);
			break;
		case COLLIDER_PLAYER_BULLET:
			App->render->DrawQuad(colliders[i]->rect, 100, 50, 73, 100);
			break;
		case COLLIDER_ENEMY_BULLET:
			App->render->DrawQuad(colliders[i]->rect, 50, 200, 73, 80);
			break;
		default:
			break;

		}
	}
}

bool Collisions::CleanUp(){

	for (uint i = 0; i < MAX_COLLIDERS; ++i) {

		if (colliders[i] != nullptr) {

			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}


Collider* Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, Module *callback) {

	Collider *ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i) {

		if (colliders[i] == nullptr) {

			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}
	return ret;
}

Collider* Collisions::AddColliderEntity(SDL_Rect rect, COLLIDER_TYPE type, Entity *callback) {

	Collider *ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i) {

		if (colliders[i] == nullptr) {

			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}
	return ret;
}


bool Collider::CheckCollision(const SDL_Rect &r) const {

	if ((r.x + r.w) < rect.x || (r.y + r.h) < rect.y || r.x > (rect.x + rect.w) || r.y > (rect.y + rect.h)) { return false; }
	return true;
}

void Collisions::AssignMapColliders(const char* file_name) { 

	SString tmp("%s%s", App->map->folder.GetString(), file_name);

	pugi::xml_parse_result result = App->map->map_file.load_file(tmp.GetString());

	if (result == NULL) {

		LOG("Could not load xml file %s. pugi error: %s", file_name, result.description());
		return;
	}
	pugi::xml_node collider;
	pugi::xml_node type;
	const char* collidertype;

	for (type = App->map->map_file.child("map").child("objectgroup");type && result; type = type.next_sibling("objectgroup")) {

		collidertype = type.attribute("name").as_string();

		for (collider = type.child("object"); collider&&result; collider= collider.next_sibling("object")) {
			
			if (strcmp(collidertype, "Colliders") == 0) 
				AddCollider({ collider.attribute("x").as_int(),collider.attribute("y").as_int(),collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::COLLIDER_STATIC);
			

			if (strcmp(collidertype, "Death_Colliders") == 0) 
				AddCollider({ collider.attribute("x").as_int(),collider.attribute("y").as_int(),collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::COLLIDER_FALL);
			
		}
	}
}