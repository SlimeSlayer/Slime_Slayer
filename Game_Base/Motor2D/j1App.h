#ifndef __j1APP_H__
#define __j1APP_H__

#include "j1PerfTimer.h"
#include "j1Timer.h"
#include "PugiXml\src\pugixml.hpp"
#include <list>
#include <vector>
#include "SDL_image/include/SDL_image.h"

#define TIME_TO_ENABLE 5
#define ENABLE_TIME_MARK 300

struct SDL_Texture;

// Modules
class j1Window;
class j1Input;
class j1Render;
class j1Textures;
class j1Audio;
class j1FileSystem;
class j1Fonts;
class j1Gui;
class j1Module;
class j1Animator;
class j1EntitiesManager;
class j1ParticleManager;
class j1InputManager;
class j1Video;
class j1Intro;
class j1Physics;
class j1Player;

class Scene;
class MainMenu;
class Tutorial;
class Endless;
class UI_Element;
class UI_String;
class UI_Progressive_Bar;

enum APP_CONTEXT
{
	UNKNOWN_CONTEXT = 0,
	IN_GAME_CONTEXT,
	PAUSE_CONTEXT,
	MAIN_MENU_CONTEXT
};

class j1App
{
public:

	// Constructor
	j1App(int argc, char* args[]);

	// Destructor
	~j1App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Exposing some properties for reading
	int			GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	float		GetDT() const;

	void LoadGeneralData(const char* file);
	void LoadPartyData(const char* file);
	void SaveGeneralData(const char* file);
	void SavePartyData(const char* file);

	bool IsXMLdir(const char* dir)const;

private:

	// Enable the current active modules
	void EnableActiveModulesNow();

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
	bool SavegameNow();

public:
	
	void EnableActiveModules();

public:

	// Modules
	j1Window*			win = NULL;
	j1Input*			input = NULL;
	j1InputManager*		input_manager = NULL;
	j1Render*			render = NULL;
	j1Textures*			tex = NULL;
	j1Audio*			audio = NULL;
	j1FileSystem*		fs = NULL;
	j1Physics*			physics = NULL;
	j1Fonts*			font = NULL;
	j1Gui*				gui = NULL;
	j1Video*			video = NULL;
	j1EntitiesManager*	entities_manager = NULL;
	j1ParticleManager*	particle_manager = NULL;
	j1Animator*			animator = NULL;
	j1Player*			player = NULL;

	// Scenes
	MainMenu*		main_menu = NULL;
	Tutorial*		tutorial = NULL;
	Endless*		endless = NULL;

	APP_CONTEXT		app_context = UNKNOWN_CONTEXT;

	// Used in data loading
	APP_CONTEXT	StrToAppContext(const char* str)const;

	// Add a new module to handle
	void		AddModule(j1Module* module);
	//Get Module pointer by name
	j1Module*	GetModule(const char* module_name);
	//Get number of modules
	uint		GetModulesNum()const;
	//Get module of position x in array
	j1Module*	GetModuleAt(uint index)const;

public:

	//Get config xml file
	pugi::xml_node GetConfigXML()const;

private:

	pugi::xml_node config_node;

	std::list<j1Module*>	modules;

	std::vector<j1Module*>	modules_to_disable;
	std::vector<j1Module*>	modules_to_enable;
	uint					enable_index = 0;	/*Index of the current enabled module*/

	int					argc = 0;
	char**				args = nullptr;

	std::string			title;
	std::string			organization;

public:

	mutable bool		want_to_g_save = false;	/*True when general save function called*/
	mutable bool		want_to_p_save = false;	/*True when party save function called*/
	mutable bool		want_to_g_load = false;	/*True when general load function called*/
	mutable bool		want_to_p_load = false;	/*True when party load function called*/


	std::string			load_game;	/*Folder where the game is loaded*/
	std::string			save_game;	/*Folder where the game is saved*/
	

private:

	j1PerfTimer			ptimer;
	uint64				frame_count = 0;
	j1Timer				startup_time;
	j1Timer				frame_time;
	j1Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = 0;

	//Modules management data ---------
	Scene*				current_scene = nullptr;	/*Current scene pointer used to adapt modules to game state*/
	bool				want_to_quit = false;		/*Ready to close app*/
	bool				want_to_enable = false;		/*If its true modules will be enabled at next frame*/
	
public:

	bool				pause = false;				/*If its true the game is paused*/

	//Scene change data
	bool				load_scene_enabled = false;
	bool				is_loading = false;

	//Loading screen data
	j1Timer				enable_timer;
	UI_String*			loading_string = nullptr;
	UI_Progressive_Bar*	loading_bar = nullptr;

	void DrawLoadProcess();
	void EndLoadProcess();

public:

	uint GetCurrentFrameTime()const;

	void SetQuit();
	bool GetQuit()const;
	
	//Scene change methods
	Scene*	GetCurrentScene()const;
	void	ActiveMainMenu();
	void	ActiveTutorial();
	void	ActiveEndless();

	//Debug booleans
	bool	collisions_debug = true;
	bool	ui_debug = false;

};

extern j1App* App;

#endif