#ifndef __APP_H__
#define __APP_H__

#include "List.h"
#include "Module.h"
#include "PerfTimer.h"
#include "Timer.h"

#include "PugiXml\src\pugixml.hpp"
#include "SDL/include/SDL.h"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class Map;
class Collisions;
class Fade;
class PathFinding;
class Particles;
class EntityManager;

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	virtual ~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	float GetDT() const;

	SString get_load_game() const { return load_game; }
	SString get_save_game() const { return save_game; }

	void LoadGame(const char* file);
	void SaveGame(const char* file) const;
	bool GetSaves(const char* path) const;

	bool cap = true;
	bool using_VSYNC = false;
	uint32 frame_cap;
	uint32				capped_ms;

private:

	// Load config file
	pugi::xml_node LoadConfig(pugi::xml_document&) const;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow() const;


public:

	// Modules
	Window*			win;
	Input*			input;
	Render*			render;
	Textures*			tex;
	Audio*			audio;
	Scene*			scene;
	Map*				map;
	Collisions*		collisions;
	Fade*				fade;
	PathFinding*		pathfinding;
	Particles*		particles;
	EntityManager*	entity_manager;

private:

	List<Module*>	modules;
	int					argc;
	char**				args;

	SString			title;
	SString			organization;

	mutable bool		want_to_save;
	bool				want_to_load;
	SString			load_game;
	mutable SString	save_game;

	//Time for fps
	PerfTimer			ptimer;

	Timer				startup_time;
	Timer				frame_time;
	Timer				last_sec_frame_time;

	uint64				frame_count = 0;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;

	
	float				dt = 0.0f;
};

extern j1App* App; 

#endif