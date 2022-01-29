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
#include "Fonts.h"
#include "Gui.h"

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
	bloodTex = App->tex->Load("gui/Blood.png");
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

	//App->audio->ControlMUSVolume(30);
	//App->audio->ControlSFXVolume(30);
	App->render->ResetCamera();
	CreateUI_Elements();
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	lastMousePos = newMousePos;
	App->input->GetMousePosition(newMousePos.x, newMousePos.y);
	List_item<UI_Element*>* UI_Item = UI_Elements_List.end;
	for (; UI_Item != nullptr; UI_Item = UI_Item->prev) {
		if (UI_Item->data->isActive) {
			if (UI_Item->data->Logic == LIFE_ALERT) {
				if (Player != nullptr && Player->life <= 20) {
					UI_Item->data->Alert(bloodTex, bloodalpha);
				}
			}
			if ((UI_Item->data->Logic == LOAD || UI_Item->data->Logic == CONTINUE) && onAction == false) {
				if (App->GetSaves("save_game.xml") == true) {
					if (UI_Item->data->Clicked()) {
						App->LoadGame("save_game.xml");

						if (currentLevel == MAIN_MENU) {

							playButton->isActive = false;
							creditButton->isActive = false;
							quitButton->isActive = false;
							continueButton->isActive = false;
							settingsButton->isActive = false;
							webPageButton->isActive = false;
							gameName->isActive = false;

							//active inGame UI
							labelMinutes->isActive = true;
							coinsbarBackfround->isActive = true;
							pauseButton->isActive = true;
							lifeBarBackground->isActive = true;
							scoreTextBackground->isActive = true;

							pausedGame = false;
						}
					}
				}
				else
					UI_Item->data->CurrentRect = &UI_Item->data->UI_Rect_Pushed;
			}
			if (App->entity_manager->coins >= 5) {
				iconToSave->isActive = true;
			}
			else
				iconToSave->isActive = false;
			if (UI_Item->data->Logic == SAVE_REMEMBER) {
				UI_Item->data->Position.x = Player->player_position.x + 10;
				UI_Item->data->Position.y = Player->player_position.y - UI_Item->data->UI_Rect.h + 10;
			}
			if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && currentLevel != MAIN_MENU) {

				if (currentLevel == LEVEL1)
					pause_time1 = Level1_Timer.Read();
				else if (currentLevel == LEVEL2)
					pause_time2 = Level2_Timer.Read();
				UI_Item->data->Active(pausePanel);
				pausedGame = true;
			}

			if (UI_Item->data->Logic == ACTIVEWIN) {
				if (UI_Item->data->Clicked()) {
					if (UI_Item->data == settingsButton || UI_Item->data == settingsButton2) {
						if (UI_Item->data == settingsButton)
							settingsPanel->Position.y = 170;
						else
							settingsPanel->Position.y = 60;

						UI_Item->data->Active(settingsPanel);
					}
					else if (UI_Item->data == pauseButton) {
						if (currentLevel == LEVEL1)
							pause_time1 = Level1_Timer.Read();
						else if (currentLevel == LEVEL2)
							pause_time2 = Level2_Timer.Read();

						UI_Item->data->Active(pausePanel);
						pausedGame = true;
					}
					else if (UI_Item->data == creditButton) {
						UI_Item->data->Active(creditsPanel);
					}
				}
			}
			if (UI_Item->data->Logic == TIME) {
				if (UI_Item->data == labelSeconds) {
					if (currentLevel == LEVEL1) {
						int sec = (int)Level1_Timer.ReadSec() % 60;
						UI_Item->data->Time(sec);
					}
					else if (currentLevel == LEVEL2) {
						int sec = (int)Level2_Timer.ReadSec() % 60;
						UI_Item->data->Score(Level2_Timer.ReadSec());
					}
				}
				else if (UI_Item->data == labelMinutes) {
					if (currentLevel == LEVEL1) {
						int minutes = (int)Level1_Timer.ReadSec() / 60;
						UI_Item->data->Time(minutes);
					}
					else if (currentLevel == LEVEL2) {
						int minutes = (int)Level2_Timer.ReadSec() / 60;
						UI_Item->data->Time(minutes);
					}
				}
			}
			if (UI_Item->data->Logic == COINS) {
				UI_Item->data->Score(App->entity_manager->coins);
			}
			if (UI_Item->data->Logic == CLOSEWIN) {
				if (UI_Item->data->Clicked() && !onAction) {
					if (UI_Item->data == closeWinButon) {
						UI_Item->data->Deactive(settingsPanel);
					}
					if (UI_Item->data == closeCreditsPanel) {
						UI_Item->data->Deactive(creditsPanel);
					}
				}
			}
			if (UI_Item->data->Logic == SCORE) {
				UI_Item->data->Score(App->entity_manager->score);
			}
			if (UI_Item->data->Logic == DRAGVOLUME) {
				if (UI_Item->data->Clicked()) {
					int initialPos = 100;
					UI_Item->data->VolumeControl(newMousePos, lastMousePos);
					if (UI_Item->data == thumbMusic)
						App->audio->ControlMUSVolume(initialPos * UI_Item->data->initialPosition.x / 150);
					else if (UI_Item->data == thumbSFX)
						App->audio->ControlSFXVolume(initialPos * UI_Item->data->initialPosition.x / 150);

					onAction = true;
				}
			}
			if (UI_Item->data->Logic == WEB) {
				if (UI_Item->data->Clicked()) {
					if (UI_Item->data == webPageButton)
						UI_Item->data->goWeb("https://github.com/DarkAvanger/Project2D_Game");
					else if (UI_Item->data == githubButon)
						UI_Item->data->goWeb("https://github.com/DarkAvanger");
				}
			}
			if (UI_Item->data->Logic == QUIT) {
				if (UI_Item->data->Clicked()) {
					//SDL_Quit();
					return false;
				}
			}
			if (UI_Item->data->Logic == PLAY) {
				if (UI_Item->data->Clicked()) {
					Change_Level = true;
					//Desactive all Main menu
					if (currentLevel == MAIN_MENU) {
						playButton->toDesactive = true;
						quitButton->toDesactive = true;
						continueButton->toDesactive = true;
						settingsButton->toDesactive = true;
						webPageButton->toDesactive = true;
						gameName->toDesactive = true;
						creditButton->toDesactive = true;
						creditsPanel->isActive = false;

						//active inGame UI
						labelMinutes->toActive = true;
						coinsbarBackfround->toActive = true;
						pauseButton->toActive = true;
						lifeBarBackground->toActive = true;
						scoreTextBackground->toActive = true;

						pausedGame = false;
					}
				}
			}
			if (UI_Item->data->Logic == RESUME_GAME) {
				if (UI_Item->data->Clicked() && !onAction) {
					if (currentLevel == LEVEL1)
						Level1_Timer.StartFrom(-pause_time1);
					else if (currentLevel == LEVEL2)
						Level2_Timer.StartFrom(-pause_time2);
					UI_Item->data->Deactive(pausePanel);
					UI_Item->data->Deactive(settingsPanel);
					pausedGame = false;
				}
			}
			if (UI_Item->data->Logic == RETURN_MAIN_MENU) {
				if (UI_Item->data->Clicked() && !onAction) {
					if (currentLevel == LEVEL1) {
						currentLevel = LEVEL2;
						Change_Level = true;
					}
					else
						Change_Level = true;
					playButton->toActive = true;
					quitButton->toActive = true;
					continueButton->toActive = true;
					webPageButton->toActive = true;
					gameName->toActive = true;
					creditButton->toActive = true;
					pauseButton->toDesactive = true;
					labelMinutes->toDesactive = true;
					scoreTextBackground->toDesactive = true;
					lifeBarBackground->toDesactive = true;
					pausePanel->toDesactive = true;
					coinsbarBackfround->toDesactive = true;

				}
			}
			if (UI_Item->data->Logic == LIFE) {
				if (Player != nullptr) {
					float initialw = 363;
					UI_Item->data->UI_Rect.w = initialw * ((float)Player->life / 100);
					if (Player->life <= 20) {
						BloodAlert->isActive = true;
					}
				}
			}
		}
	}
	onAction = false;
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{


	if (Change_Level) {

		App->fade->Fade2(2.0f);

		if (App->fade->current_step == App->fade->fade_from_black) {
			ChangeLevel(currentLevel, 0);
			Change_Level = false;
		}
	}

	else if (changing_same_Level) {

		App->fade->Fade2(2.0f);

		if (App->fade->current_step == App->fade->fade_from_black) {

			ChangeLevel(NO_CHANGE, 0);
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
			ChangeLevel(LEVEL2+1, 0);
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

void Scene::ChangeLevel(int level_change, int increment) {
	
	App->entity_manager->CleanUp();
	/*App->entity_manager->DesrtroyEnemies();

	if(Player!=nullptr)
		Player->entity_collider->to_delete = true;

	App->entity_manager->DestroyEntity(Player);
	RELEASE(Player);*/
	LEVELS aux = currentLevel;

	if (level_change != NO_CHANGE) {
		App->collisions->CleanUp();
		IterateLevel(level_change, increment);
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


		//LOAD SCENE ENEMIES
		//Load Enemy Flyers
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
		LoadObjects(current_map.Filename.GetString());
		App->collisions->AssignMapColliders(current_map.Filename.GetString());

		App->entity_manager->LoadSceneEnemeies(en_pos, WALKER, "Enemy1_Tileset.png", EnemiesDocument);

		//	PERF_START(Level1_Timer);
		//	Level1_Timer.StartFrom(-pause_time1);

	}
	if (currentLevel == LEVEL2) {

		App->audio->PlayMusic(music_node.attribute("level").as_string());
		App->render->camera.x = -7;
		App->render->camera.y = -903;
		Player = Player->CreatePlayer(iPoint(200, 1116));
		Player->LoadPlayer("Character_tileset.png");

		//LOAD SCENE ENEMIES
		//Load Enemy Flyers
		pugi::xml_parse_result Enemies_doc_res = EnemiesDocument.load_file("Enemy2_Settings.xml");
		if (Enemies_doc_res == NULL)
			LOG("The xml file containing the player tileset fails. Pugi error: %s", Enemies_doc_res.description());

		pugi::xml_parse_result enemies_pos_result = EnemiesPositions.load_file("Enemy_Positions.xml");
		en_pos = EnemiesPositions.child("config").child("Level2").child("Enemy2");

		App->entity_manager->LoadSceneEnemeies(en_pos, FLYER, "Enemy2_Tileset.png", EnemiesDocument);

		//Load enemy Walkers
		pugi::xml_parse_result Enemies_doc_res2 = EnemiesDocument.load_file("Enemy1_Settings.xml");
		if (Enemies_doc_res2 == NULL)
			LOG("The xml file containing the player tileset fails. Pugi error: %s", Enemies_doc_res2.description());

		en_pos = EnemiesPositions.child("config").child("Level2").child("Enemy1");


		LoadObjects(current_map.Filename.GetString());
		App->collisions->AssignMapColliders(current_map.Filename.GetString());

		App->entity_manager->LoadSceneEnemeies(en_pos, WALKER, "Enemy1_Tileset.png", EnemiesDocument);


		/*	PERF_START(Level2_Timer);
			Level2_Timer.StartFrom(-pause_time2);*/

	}
	if (currentLevel == MAIN_MENU) {
		App->collisions->CleanUp();
		playButton->isActive = true;
		quitButton->isActive = true;
		continueButton->isActive = true;
		settingsButton->isActive = true;
		webPageButton->isActive = true;
		gameName->isActive = true;
		creditButton->isActive = true;
		scoreTextBackground->isActive = false;
		coinsbarBackfround->isActive = false;
		pauseButton->isActive = false;
		labelSeconds->isActive = false;
		labelMinutes->isActive = false;
		BloodAlert->isActive = false;
		lifeBarBackground->isActive = false;

		pausePanel->isActive = false;
		App->audio->PlayMusic(music_node.attribute("intro2").as_string());

	}

	if (currentLevel == 1) {

		if (pause_time1 > 0) {


			PERF_START(Level1_Timer);
			Level1_Timer.StartFrom(-pause_time1);
		}
		else
			PERF_START(Level1_Timer);

	}
	else if (currentLevel == 2) {

		if (pause_time2 > 0) {
			PERF_START(Level2_Timer);
			Level2_Timer.StartFrom(-pause_time2);

		}
		else
			PERF_START(Level2_Timer);
	}
	/*if (currentLevel!=aux) {
		LoadObjects(current_map.Filename.GetString());
		App->collisions->AssignMapColliders(current_map.Filename.GetString());
	}*/
}

void Scene::IterateLevel(int level_change, int increment) {

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

bool Scene::Load(pugi::xml_node& data)
{

	int level_it = data.child("scene").attribute("current_level").as_int();
	pause_time2 = data.child("scene").attribute("paused_time2").as_int();
	pause_time1 = data.child("scene").attribute("paused_time1").as_int();

	App->map->TriggerActive = data.child("scene").attribute("trigger").as_bool();

	if (currentLevel != level_it)
		App->scene->ChangeLevel(level_it, 0);


	if (currentLevel == 1) {

		if (pause_time1 > 0) {


			PERF_START(Level1_Timer);
			Level1_Timer.StartFrom(-pause_time1);
		}
		else
			PERF_START(Level1_Timer);

	}
	else if (currentLevel == 2) {

		if (pause_time2 > 0) {
			PERF_START(Level2_Timer);
			Level2_Timer.StartFrom(-pause_time2);

		}
		else
			PERF_START(Level2_Timer);
	}
	return true;
}

// Save Game State
bool Scene::Save(pugi::xml_node& data) const
{
	pugi::xml_node scene_node = data.append_child("scene");
	scene_node.append_attribute("current_level") = currentLevel;
	scene_node.append_attribute("trigger") = App->map->TriggerActive;

	if (currentLevel == 1) {

		scene_node.append_attribute("paused_time1") = Level1_Timer.Read();
		scene_node.append_attribute("paused_time2") = 0;
	}
	else if (currentLevel == 2) {

		scene_node.append_attribute("paused_time1") = 0;
		scene_node.append_attribute("paused_time2") = Level2_Timer.Read();
	}

	return true;
}



void Scene::CreateUI_Elements() {

	float scale = 0.8f;
	//MAIN MENU
	gameName = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(210, 50), SDL_Rect{ 515, 3, 506, 117 }, NONE_LOGIC);

	playButton = App->gui->Add_UIElement(BUTTON, iPoint(360, 200), { 254, 178, 242, 76 }, PLAY, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, scale);
	labelPlayButton = App->gui->Add_UIElement(LABEL, iPoint(50, 7), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, playButton, "PLAY");

	continueButton = App->gui->Add_UIElement(BUTTON, iPoint(360, 300), { 254, 178, 242, 76 }, CONTINUE, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, scale);
	labelContinueButton = App->gui->Add_UIElement(LABEL, iPoint(10, 7), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, continueButton, "CONTINUE");

	settingsButton = App->gui->Add_UIElement(BUTTON, iPoint(360, 400), { 254, 178, 242, 76 }, ACTIVEWIN, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, scale);
	labelSettingsButton = App->gui->Add_UIElement(LABEL, iPoint(10, 7), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsButton, "SETTINGS");

	creditButton = App->gui->Add_UIElement(BUTTON, iPoint(15, 350), { 254, 178, 242, 76 }, ACTIVEWIN, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, scale);
	labelcreditButton = App->gui->Add_UIElement(LABEL, iPoint(15, 10), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, creditButton, "CREDITS");

	creditsPanel = App->gui->Add_UIElement(PANEL, iPoint(310, 20), { 776, 479, 247, 382 }, NONE_LOGIC, NULL_RECT, NULL_RECT, 1.2f);
	creditsPanel->isActive = false;

	closeCreditsPanel = App->gui->Add_UIElement(BUTTON, iPoint(250, -10), { 3, 438, 76, 76 }, CLOSEWIN, { 3, 349, 76, 76 }, { 3, 259, 76, 76 }, scale, None, creditsPanel);
	iconCloseCreditsPanel = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(18, 18), { 197, 352, 34, 34 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, closeCreditsPanel);



	webPageButton = App->gui->Add_UIElement(BUTTON, iPoint(770, 400), { 387,267,108,110 }, WEB, { 389,390,108, 110 }, { 387,267,108,110 }, scale);
	iconWebPageButton = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(30, 20), { 269, 404, 32, 38 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, webPageButton);

	quitButton = App->gui->Add_UIElement(BUTTON, iPoint(15, 430), { 0, 178, 242, 76 }, QUIT, { 0, 88, 242, 76 }, { 0, 0, 242, 76 }, scale);
	labelQuitButton = App->gui->Add_UIElement(LABEL, iPoint(55, 7), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, quitButton, "QUIT");


	//CREDITS
	labelCreatorsofGame = App->gui->Add_UIElement(LABEL, iPoint(90, 30), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.5, None, creditsPanel, "CREATED BY");
	labelA = App->gui->Add_UIElement(LABEL, iPoint(70, 70), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.3, None, creditsPanel, "Angel Consola");
	githubButon = App->gui->Add_UIElement(BUTTON, iPoint(90, 100), { 512,235,119,119 }, WEB, { 639,235,119,119 }, { 512,235,119,119 }, 0.6, None, creditsPanel);
	githubButonA = App->gui->Add_UIElement(BUTTON, iPoint(200, 100), { 512,235,119,119 }, WEB, { 639,235,119,119 }, { 512,235,119,119 }, 0.6, None, creditsPanel);
	License = App->gui->Add_UIElement(LABEL, iPoint(70, 240), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.3, None, creditsPanel, "LICENSED UNDER MIT");
	//IN GAME UI

	iconToSave = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(0, 0), { 794, 164, 187,117 }, SAVE_REMEMBER, NULL_RECT, NULL_RECT, scale, None);
	iconToSave->isActive = false;

	pauseButton = App->gui->Add_UIElement(BUTTON, iPoint(780, 10), { 94,438,75,78 }, ACTIVEWIN, { 94,349,75,78 }, { 94,258,75,78 }, scale);
	iconPauseButton = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(20, 20), { 201, 301, 31,31 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, pauseButton);
	pauseButton->isActive = false;

	lifeBarBackground = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(10, 10), { 6, 525, 345, 87 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale);
	lifeBarBackground->isActive = false;
	lifeBar = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(67, 18), { 513, 481, 254, 40 }, LIFE, NULL_RECT, NULL_RECT, scale, None, lifeBarBackground);

	pausePanel = App->gui->Add_UIElement(PANEL, iPoint(310, 80), { 776, 479, 247, 382 }, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.98);
	pausePanel->isActive = false;

	settingsButton2 = App->gui->Add_UIElement(BUTTON, iPoint(55, 160), { 254, 178, 242, 76 }, ACTIVEWIN, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, 0.7, None, pausePanel);
	labelSettingsButton2 = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.7, None, settingsButton2, "SETTINGS");

	returnMainMenuButton = App->gui->Add_UIElement(BUTTON, iPoint(55, 280), { 0, 178, 242, 76 }, RETURN_MAIN_MENU, { 0, 88, 242, 76 }, { 0, 0, 242, 76 }, 0.7, None, pausePanel);
	labelreturnMainMenuButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.7, None, returnMainMenuButton, "QUIT");

	resumeButton = App->gui->Add_UIElement(BUTTON, iPoint(55, 30), { 254, 178, 242, 76 }, RESUME_GAME, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, 0.7, None, pausePanel);
	labelResumeButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.7, None, resumeButton, "RESUME");

	loadButton = App->gui->Add_UIElement(BUTTON, iPoint(55, 95), { 254, 178, 242, 76 }, LOAD, { 254, 88, 242, 76 }, { 254, 0, 242, 76 }, 0.7, None, pausePanel);
	labelLoadButton = App->gui->Add_UIElement(LABEL, iPoint(10, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, 0.7, None, loadButton, "LOAD");

	BloodAlert = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(0, 0), { 0, 0, 900, 500 }, LIFE_ALERT, NULL_RECT, NULL_RECT);
	BloodAlert->texture = bloodTex;
	BloodAlert->isActive = false;

	scoreTextBackground = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(565, 10), { 512, 135, 253, 87 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale);
	scoreText = App->gui->Add_UIElement(LABEL, iPoint(75, 11), NULL_RECT, SCORE, NULL_RECT, NULL_RECT, scale, None, scoreTextBackground);
	scoreTextBackground->isActive = false;

	coinsbarBackfround = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(10, 425), { 368, 524, 254, 87 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale);
	labelCoins = App->gui->Add_UIElement(LABEL, iPoint(125, 11), NULL_RECT, COINS, NULL_RECT, NULL_RECT, scale, None, coinsbarBackfround);
	coinsbarBackfround->isActive = false;

	labelMinutes = App->gui->Add_UIElement(LABEL, iPoint(320, 20), NULL_RECT, TIME, NULL_RECT, NULL_RECT, scale, None);
	labelSeconds = App->gui->Add_UIElement(LABEL, iPoint(70, 0), NULL_RECT, TIME, NULL_RECT, NULL_RECT, scale, None, labelMinutes);
	labelBetSecMin = App->gui->Add_UIElement(LABEL, iPoint(55, 0), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, labelMinutes, ":");
	labelMinutes->isActive = false;


	SDL_SetTextureBlendMode(bloodTex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(bloodTex, 200);

	//GENERAL UI
	settingsPanel = App->gui->Add_UIElement(PANEL, iPoint(180, 170), { 0, 657, 698, 365 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale);
	settingsPanel->isActive = false;

	closeWinButon = App->gui->Add_UIElement(BUTTON, iPoint(670 * scale, 0), { 3, 438, 76, 76 }, CLOSEWIN, { 3, 349, 76, 76 }, { 3, 259, 76, 76 }, scale, None, settingsPanel);
	iconCloseWinButton = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(18, 18), { 197, 352, 34, 34 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, closeWinButon);

	slideMusic = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(210, 120), { 8, 616, 388, 38 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsPanel);
	slideSFX = App->gui->Add_UIElement(NON_INTERACTIVE, iPoint(210, 190), { 8, 616, 388, 38 }, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsPanel);
	labelMusic = App->gui->Add_UIElement(LABEL, iPoint(30, 100), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsPanel, "MUSIC");
	labelSFX = App->gui->Add_UIElement(LABEL, iPoint(30, 170), NULL_RECT, NONE_LOGIC, NULL_RECT, NULL_RECT, scale, None, settingsPanel, "SFX");

	thumbMusic = App->gui->Add_UIElement(PANEL, iPoint(0, -10), { 249, 260, 78, 64 }, DRAGVOLUME, NULL_RECT, NULL_RECT, scale, None, slideMusic);
	thumbMusic->initialPosition.x = 50;
	App->audio->ControlMUSVolume(100 * thumbMusic->initialPosition.x / 150);
	thumbSFX = App->gui->Add_UIElement(PANEL, iPoint(0, -10), { 249, 260, 78, 64 }, DRAGVOLUME, NULL_RECT, NULL_RECT, scale, None, slideSFX);
	thumbSFX->initialPosition.x = 70;
	App->audio->ControlSFXVolume(100 * thumbSFX->initialPosition.x / 150);

	UI_Elements_List.add(gameName);
	UI_Elements_List.add(playButton);
	UI_Elements_List.add(githubButon);
	UI_Elements_List.add(githubButonA);
	UI_Elements_List.add(quitButton);
	UI_Elements_List.add(continueButton);
	UI_Elements_List.add(iconToSave);
	UI_Elements_List.add(settingsButton);
	UI_Elements_List.add(webPageButton);
	UI_Elements_List.add(creditButton);
	UI_Elements_List.add(settingsPanel);
	UI_Elements_List.add(settingsButton2);
	UI_Elements_List.add(closeWinButon);
	UI_Elements_List.add(resumeButton);
	UI_Elements_List.add(returnMainMenuButton);
	UI_Elements_List.add(loadButton);
	UI_Elements_List.add(pauseButton);
	UI_Elements_List.add(thumbMusic);
	UI_Elements_List.add(closeCreditsPanel);
	UI_Elements_List.add(thumbSFX);
	UI_Elements_List.add(lifeBarBackground);
	UI_Elements_List.add(lifeBar);
	UI_Elements_List.add(BloodAlert);
	UI_Elements_List.add(scoreTextBackground);
	UI_Elements_List.add(scoreText);
	UI_Elements_List.add(coinsbarBackfround);
	UI_Elements_List.add(labelCoins);
	UI_Elements_List.add(labelSeconds);
	UI_Elements_List.add(labelMinutes);

}