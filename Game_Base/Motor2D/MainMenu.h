#ifndef __j1MENU_H__
#define __j1MENU_H__

#include "j1Module.h"
class UI_Button;
class UI_Image;
class UI_Scroll_Bar;

class MainMenu : public j1Module
{
public:

	MainMenu();
	~MainMenu();

public:

	void Init();
	void Active();
	bool Enable();
	void Disable();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	//GUI functions -------------------
	void		GUI_Input(UI_Element* target, GUI_INPUT input);
	void		GUI_Controller_Input(INPUT_EVENT input_event);
	UI_Element*	GetCorrectItemToSelect()const;

private:

	// MENU DATA
	//Main Menu
	UI_Element*	menu_branch = nullptr;
	UI_Button*	start_button = nullptr;
	UI_Button*	settings_button = nullptr;
	UI_Button*	quit_button = nullptr;

	//Settings Menu
	UI_Image*	settings_menu = nullptr;
	UI_Button*	settings_quit_button = nullptr;
	UI_Button*	settings_audio_button = nullptr;
	UI_Button*	settings_video_button = nullptr;

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
};
#endif
