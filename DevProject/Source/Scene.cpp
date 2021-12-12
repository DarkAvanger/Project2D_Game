#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Scene.h"
#include "Player.h"
#include "Collisions.h"
#include "PathFinding.h"
#include "Fade.h"
#include "EntityManager.h"
#include "Enemy.h"
#include "Player.h"
#include "Objects.h"


Scene::Scene() : Module()
{
	name.create("scene");
	Main_Menu = true;
	Level1 = false;
	Level2 = false;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{

	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
  
	pugi::xml_parse_result load_scenedoc_res = SceneDocument.load_file("config.xml");
	music_node = SceneDocument.child("config").child("music");

	if (load_scenedoc_res == NULL)
		LOG("The xml file containing the music fails. Pugi error: %s", load_scenedoc_res.description());
	
	App->map->Load("Main_Menu.tmx", Intro_map);
	App->map->Load("Level1.tmx", Level1_map);
	App->map->Load("Level1_WalkabilityMap.tmx", Level1_pathfinding_map);


	if (Main_Menu == true) {

		current_map = Intro_map;
		currentLevel = MAIN_MENU;

		App->audio->PlayMusic(music_node.attribute("intro").as_string());

		pathfinding = false;
	}

	if (Level1 == true) {

		App->collisions->AssignMapColliders("Level1.tmx");
		LoadObjects("Level1.tmx");
	
		if (Player == nullptr)
			Player = Player->CreatePlayer(iPoint(200, 1080));

		current_map = Level1_map;
		currentLevel = LEVEL1;

		pathfinding = true;

		if(Player->active)
			Player->Start();

		if(!App->collisions->active)
			App->collisions->Start();

	}

	else if (Level2 == true) {

		App->collisions->AssignMapColliders("Level2.tmx");
		LoadObjects("Level2.tmx");

		if (Player == nullptr)
			Player = Player->CreatePlayer(iPoint(580, 1400));

		current_map = Level2_map;
		current_pathfinding_map = Level2_pathfinding_map;
		currentLevel = LEVEL2;

		pathfinding = false;

		if (Player->active)
			Player->Start();

		if (!App->collisions->active)
			App->collisions->Start();

	}

	App->audio->ControlMUSVolume(30);
	App->audio->ControlSFXVolume(30);
	App->render->ResetCamera();

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{


	if (Change_Level) {

		App->fade->Fade2(2.0f);

		if (App->fade->current_step == App->fade->fade_from_black) {
			ChangeLevel(currentLevel);
			Change_Level = false;
		}
	}

	else if (changing_same_Level) {

		App->fade->Fade2(2.0f);

		if (App->fade->current_step == App->fade->fade_from_black) {

			ChangeLevel(NO_CHANGE);
			changing_same_Level = false;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) 
		App->LoadGame("save_game.xml");
	

	if(App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN && !changing_same_Level) {
		
		App->map->TriggerActive = false;
		changing_same_Level = true;
		
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && !changing_same_Level) {

		App->map->TriggerActive = false;
		changing_same_Level = true;

	}

	if (currentLevel == MAIN_MENU && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && !Change_Level)
		Change_Level = true;

	
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {

		App->cap = !App->cap;

		if (App->cap)
			App->frame_cap = 30;
		else
			App->frame_cap = 65; //Fix frames to 60 average;

		App->capped_ms = 1000 / App->frame_cap;
	}

	if (Player != nullptr) {

		if (Player->life <= 0) {
			if(Player->Dead.Finished())
			ChangeLevel(LEVEL2+1);
		}
	}

	App->map->Draw(current_map);
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{

	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{

	LOG("Freeing scene");
	return true;
}

void Scene::ChangeLevel(int level_change) {
	
	App->entity_manager->DesrtroyEnemies();

	if(Player!=nullptr)
		Player->entity_collider->to_delete = true;

	App->entity_manager->DestroyEntity(Player);
	RELEASE(Player);
	LEVELS aux = currentLevel;


	if (level_change != NO_CHANGE) {
		App->collisions->CleanUp();
		IterateLevel(level_change);
	}

	if (pathfinding) {
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data, current_pathfinding_map))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	if (currentLevel == LEVEL1) {

		App->audio->PlayMusic(music_node.attribute("level").as_string());
		App->render->camera.x = -7;
		App->render->camera.y = -1242;
		Player = Player->CreatePlayer(iPoint(580, 1400));
		Player->LoadPlayer("Character_tileset.png");


		pugi::xml_parse_result Enemies_doc_res = EnemiesDocument.load_file("Enemy2_Settings.xml");
		if (Enemies_doc_res == NULL)
			LOG("The xml file containing the player tileset fails. Pugi error: %s", Enemies_doc_res.description());

		pugi::xml_parse_result enemies_pos_result = EnemiesPositions.load_file("Enemy_Positions.xml");
		en_pos = EnemiesPositions.child("config").child("Level1").child("Enemy2");

		App->entity_manager->LoadSceneEnemeies(en_pos, FLYER, "Enemy2_Tileset.png", EnemiesDocument);

		//Load enemy Walkers
		pugi::xml_parse_result Enemies_doc_res2 = EnemiesDocument.load_file("Enemy1_Settings.xml");
		if (Enemies_doc_res2 == NULL)
			LOG("The xml file containing the player tileset fails. Pugi error: %s", Enemies_doc_res2.description());

		en_pos = EnemiesPositions.child("config").child("Level1").child("Enemy1");

		App->entity_manager->LoadSceneEnemeies(en_pos, WALKER, "Enemy1_Tileset.png", EnemiesDocument);

	
	}
	if (currentLevel == MAIN_MENU) {
		App->collisions->CleanUp();
	}
	if (currentLevel!=aux) {
		LoadObjects(current_map.Filename.GetString());
		App->collisions->AssignMapColliders(current_map.Filename.GetString());
	}
}

void Scene::IterateLevel(int level_change) {

	level_change++;
	LevelIterator = level_change;
	

	if (LevelIterator > 2)
		LevelIterator = 0;
	

	if (LevelIterator == 0) {

		Main_Menu = true;
		Level2 = false;
		Level1 = false;
		currentLevel = MAIN_MENU;
		current_map = Intro_map;
		pathfinding = false;
		
	}

	else if (LevelIterator == 1) {

			Main_Menu = false;
			Level2 = false;
			Level1 = true;
			currentLevel = LEVEL1;
			current_map = Level1_map;
			current_pathfinding_map = Level1_pathfinding_map;
			pathfinding = true;
	}
}


bool Scene::LoadObjects(const char*file_name) {

	SString tmp("%s%s", App->map->folder.GetString(), file_name);

	pugi::xml_parse_result result = App->map->map_file.load_file(tmp.GetString());

	if (result == NULL) {

		LOG("Could not load xml file %s. pugi error: %s", file_name, result.description());
		return true;
	}

	pugi::xml_node collider;
	pugi::xml_node type;
	const char* collidertype;

	for (type = App->map->map_file.child("map").child("objectgroup"); type && result; type = type.next_sibling("objectgroup")) {

		collidertype = type.attribute("name").as_string();

		for (collider = type.child("object"); collider&&result; collider = collider.next_sibling("object")) {

			if (strcmp(collidertype, "Push_Triggers") == 0) {

				int x = collider.attribute("x").as_int();
				int y = collider.attribute("y").as_int();

				App->collisions->AddCollider({ x, y, collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::TRIGGER_PUSH);
				Objects* object = nullptr;
				object = object->CreateObject(iPoint(x, y), OBJECT_TYPE::PUSHON);


			}

			if (strcmp(collidertype, "PushOff_Triggers") == 0) {

				int x = collider.attribute("x").as_int();
				int y = collider.attribute("y").as_int();

				App->collisions->AddCollider({ x, y, collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::TRIGGER_PUSHOFF);
				Objects* object = nullptr;
				object = object->CreateObject(iPoint(x, y), OBJECT_TYPE::PUSHOFF);

			}

			if (strcmp(collidertype, "Win_Trigger") == 0) {
				
				int x = collider.attribute("x").as_int();
				int y = collider.attribute("y").as_int();

				Objects* object = nullptr;
				object = object->CreateObject(iPoint(x, y), OBJECT_TYPE::WIN);
			}

			if (strcmp(collidertype, "Bloc_Colliders") == 0) {

				int x = collider.attribute("x").as_int();
				int y = collider.attribute("y").as_int();

				App->collisions->AddCollider({ x, y, collider.attribute("width").as_int(),collider.attribute("height").as_int() }, COLLIDER_TYPE::COLLIDER_BLINKING);
				Objects* object = nullptr;
				object = object->CreateObject(iPoint(x, y), OBJECT_TYPE::BLINK_BLOCK);

			}
		}
	}


	return true;
}