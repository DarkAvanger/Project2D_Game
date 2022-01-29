#include <iostream> 
#include <sstream> 

#include "Defs.h"
#include "Log.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "Map.h"
#include "Collisions.h"
#include "Fade.h"
#include "Pathfinding.h"
#include "Particles.h"
#include "App.h"
#include "EntityManager.h"
#include "List.h"
#include "Gui.h"
#include "Fonts.h"


// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{

	want_to_save = want_to_load = false;

	input = new Input();
	win = new Window();
	render = new Render();
	tex = new Textures();
	audio = new Audio();
	scene = new Scene();
	map = new Map();
	pathfinding = new PathFinding();
	collisions = new Collisions();
	particles = new Particles();
	entity_manager = new EntityManager();
	font = new Fonts();
	gui = new Gui();
	fade = new Fade();

	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);

	AddModule(map);
	AddModule(pathfinding);


	AddModule(collisions);
	AddModule(font);

	AddModule(gui);
	AddModule(scene);
	AddModule(particles);
	AddModule(entity_manager);


	AddModule(fade);
	AddModule(render);
}

// Destructor
j1App::~j1App()
{
	List_item<Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		ret = true;
		app_config = config.child("app");
		title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());
		frame_cap = app_config.attribute("framerate_cap").as_uint();

		if (frame_cap > 0)
			capped_ms = 1000 / frame_cap;
	}

	if(ret == true)
	{
		List_item<Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	return ret;
}


bool j1App::Start()
{
	bool ret = true;
	List_item<Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{

		ret = item->data->Start();
		item = item->next;
	}

	GetSaves("save_game.xml");
	return ret;
}

// Called each loop iteration
bool j1App::Update()
{

	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;
	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();

}

// ---------------------------------------------
void j1App::FinishUpdate()
{

	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true)
		LoadGameNow();

	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	//Title
	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.ReadSec();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];

	char *vsync_;
	char *cap_string;

	if (using_VSYNC)
		vsync_ = "ON";
	else
		vsync_ = "OFF";

	if (cap)
		cap_string = "ON";
	else
		cap_string = "OFF";
	

	sprintf_s(title, 256, "Kunai Warrior dt: %.3f || Last sec frames: %i   Av.FPS: %.2f   Last Frame Ms: %02u || VSYNC: %s   Framerate Cap: %s ", dt,
		frames_on_last_update, avg_fps, last_frame_ms, vsync_, cap_string);

	App->win->SetTitle(title);

	if (last_frame_ms < capped_ms)
		SDL_Delay(capped_ms - last_frame_ms);

}


bool j1App::PreUpdate()
{
	bool ret = true;
	List_item<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	List_item<Module*>* item;
	item = modules.start;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	List_item<Module*>* item;
	Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	List_item<Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

float j1App::GetDT() const {

	return dt;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void j1App::LoadGame(const char* file)
{
	load_game = file;
	want_to_load = true;
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	
	want_to_save = true;
	save_game.create(file);
}

bool j1App::GetSaves(const char* path) const
{

	bool ret = false;

	pugi::xml_document data;
	pugi::xml_parse_result result = data.load_file(path);

	if (result != NULL) {

		LOG("SAVES FOUND");
		ret = true;
	}
	else
		LOG("saves NOT found");

	data.reset();
	return ret;
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.GetString());


	if(result != NULL) {

		LOG("Loading new Game State from %s...", load_game.GetString());

		root = data.child("game_state");

		List_item<Module*>* item = modules.start;
		ret = true;

		while(item != NULL && ret == true)
		{
			ret = item->data->Load(root.child(item->data->name.GetString()));
			item = item->next;
		}

		data.reset();
		if(ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());

	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.GetString());

	// xml 
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");

	List_item<Module*>* item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
	}

	if(ret == true)
	{
	
		data.save_file(save_game.GetString());
		LOG("... finished saving ", save_game.GetString());
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}