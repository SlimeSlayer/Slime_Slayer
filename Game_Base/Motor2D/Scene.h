#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

class Parallax;
class PhysBody;
struct SDL_Texture;
class UI_Button;
class UI_Image;
class UI_Scroll_Bar;

#define MID_PARALLAX_VAL 15
#define BACK_PARALLAX_VAL 30

class Scene : public j1Module
{
public:

	Scene();
	~Scene();

public:

	void Init();
	bool Enable();
	void Disable();

	// Called before render is available
	bool Awake(pugi::xml_node& data_node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	//GUI functions -------------------
	void GUI_Input(UI_Element* target, GUI_INPUT input);

protected:
	
	//Scene generation data -
	pugi::xml_document	data_doc;
	pugi::xml_node		current_enable_node;
	bool				base_enabled = false;

	//UI Data ---------------
	//Main Menu
	UI_Element*	menu_branch = nullptr;
	UI_Button*	settings_button = nullptr;

	//Settings Menu
	UI_Image*	settings_menu = nullptr;
	UI_Button*	settings_quit_button = nullptr;
	UI_Button*	settings_audio_button = nullptr;
	UI_Button*	settings_video_button = nullptr;
	UI_Button*	settings_exit_scene_button = nullptr;

	//Audio Menu
	UI_Image*		audio_menu = nullptr;
	UI_Button*		audio_quit_button = nullptr;

public:

	//Audio scrolls are public because we need to sync them
	UI_Scroll_Bar*	master_audio_scroll = nullptr;
	UI_Scroll_Bar*	music_audio_scroll = nullptr;
	UI_Scroll_Bar*	fx_audio_scroll = nullptr;

protected:

	//Video Menu
	UI_Image*		video_menu = nullptr;
	UI_Button*		video_quit_button = nullptr;
	UI_Button*		vsync_video_button = nullptr;
	UI_Button*		fullscreen_video_button = nullptr;

	//Scene Data ------------
	//Ground collider
	PhysBody*	floor_collider = nullptr;
	//Parallax layers
	Parallax*	front_parallax = nullptr;
	Parallax*	mid_parallax = nullptr;
	Parallax*	back_parallax = nullptr;

public:

	//Functionality ---------
	void UpdateParallax(float disp);

};

#endif // __j1SCENE_H__