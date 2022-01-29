#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Map.h"
#include "Player.h"
#include "Enemy.h"
class UI_Element;

struct SDL_Texture;
struct MapData;
class Player;

enum LEVELS {

	MAIN_MENU = 0,
	LEVEL1,
	LEVEL2,
	NO_LEVEL,
	NO_CHANGE

};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Used to iterate between Levels
	void IterateLevel(int level_change, int increment);

	//Used to change the actual level
	void ChangeLevel(int level_change, int increment);

	//Load Map Objects
	bool LoadObjects(const char*file_name);

	//Load & Save
	bool Load(pugi::xml_node& data);

	//Save Game State
	bool Save(pugi::xml_node& data) const;

	void CreateUI_Elements();



public:

	bool Level1 = false;
	bool Level2 = true;
	bool Main_Menu = true;
	bool changing_same_Level = false;
	bool Change_Level = false;
	
	int LevelIterator = 0;

	LEVELS currentLevel = NO_LEVEL;

	MapData current_map;
	MapData current_pathfinding_map;

	//Player
	Player *Player = nullptr;

	Timer Level1_Timer;
	Timer Level2_Timer;
	int pause_time1 = 0;
	int pause_time2 = 0;

	UI_Element* gameName = nullptr;

	UI_Element* quitButton = nullptr;
	UI_Element* labelQuitButton = nullptr;

	UI_Element* playButton = nullptr;
	UI_Element* labelPlayButton = nullptr;

	UI_Element* continueButton = nullptr;
	UI_Element* labelContinueButton = nullptr;

	UI_Element* webPageButton = nullptr;
	UI_Element* iconWebPageButton = nullptr;

	UI_Element* settingsButton = nullptr;
	UI_Element* labelSettingsButton = nullptr;

	UI_Element* settingsButton2 = nullptr;
	UI_Element* labelSettingsButton2 = nullptr;

	UI_Element* settingsPanel = nullptr;
	UI_Element* labelSettingsPanel = nullptr;

	UI_Element* closeWinButon = nullptr;
	UI_Element* iconCloseWinButton = nullptr;

	UI_Element* closeCreditsPanel = nullptr;
	UI_Element* iconCloseCreditsPanel = nullptr;

	UI_Element* closePauseButon = nullptr;
	UI_Element* iconClosePauseButton = nullptr;

	UI_Element* creditButton = nullptr;
	UI_Element* labelcreditButton = nullptr;

	UI_Element* slideMusic = nullptr;
	UI_Element* slideSFX = nullptr;

	UI_Element* thumbMusic = nullptr;
	UI_Element* thumbSFX = nullptr;


	UI_Element* labelMusic = nullptr;
	UI_Element* labelSFX = nullptr;

	UI_Element* lifeBar = nullptr;
	UI_Element* lifeBarBackground = nullptr;

	UI_Element* coinsbarBackfround = nullptr;
	UI_Element* labelCoins = nullptr;

	UI_Element* labelSeconds;
	UI_Element* labelMinutes;
	UI_Element* labelBetSecMin;

	UI_Element* loadButton = nullptr;
	UI_Element* labelLoadButton = nullptr;

	UI_Element* resumeButton = nullptr;
	UI_Element* labelResumeButton = nullptr;

	UI_Element* pauseButton = nullptr;
	UI_Element* iconPauseButton = nullptr;

	UI_Element* returnMainMenuButton = nullptr;
	UI_Element* labelreturnMainMenuButton = nullptr;

	UI_Element* BloodAlert = nullptr;

	UI_Element* scoreText = nullptr;
	UI_Element* scoreTextBackground = nullptr;

	int bloodalpha = 255;
	SDL_Texture* bloodTex = nullptr;

	UI_Element* pausePanel = nullptr;
	UI_Element* creditsPanel = nullptr;

	UI_Element* githubButon = nullptr;
	UI_Element* icongitHubButon = nullptr;

	UI_Element* githubButonA = nullptr;
	UI_Element* icongitHubButonA = nullptr;

	UI_Element* iconToSave = nullptr;

	UI_Element* labelCreatorsofGame = nullptr;
	UI_Element* labelA = nullptr;
	UI_Element* PathfindingProgramer = nullptr;
	UI_Element* GUI_Programer = nullptr;
	UI_Element* MainArtist = nullptr;
	UI_Element* A3 = nullptr;
	UI_Element* A2 = nullptr;
	UI_Element* License = nullptr;

	List<UI_Element*> UI_Elements_List;
	List<UI_Element*> UI_Elements_List_Playing;


	iPoint lastMousePos = { 0,0 };
	iPoint newMousePos = { 0,0 };

	bool onAction = false;
	bool onTopSomething = false;
	bool pausedGame = false;

private:

	SDL_Color None = { 255,255,255,255 };
	MapData Intro_map;
	MapData Level1_map;
	MapData Level2_map;

	MapData Level1_pathfinding_map;
	MapData Level2_pathfinding_map;

	bool pathfinding = false;

	pugi::xml_document	EnemiesDocument;
	pugi::xml_document	SceneDocument;
	pugi::xml_node		music_node;

	pugi::xml_document EnemiesPositions;
	pugi::xml_node en_pos;
	

};

#endif // __SCENE_H__