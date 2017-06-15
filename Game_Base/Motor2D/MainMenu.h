#ifndef __j1MENU_H__
#define __j1MENU_H__

#include "j1Module.h"
class UI_Button;
class UI_Image;

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
	void GUI_Input(UI_Element* target, GUI_INPUT input);

private:

	// MENU DATA
	//Main
	UI_Element*	menu_branch = nullptr;
	UI_Button*	start_button = nullptr;
	UI_Button*	settings_button = nullptr;
	UI_Button*	quit_button = nullptr;

	//Settings
	UI_Image*	settings_menu = nullptr;
	UI_Button*	settings_quit = nullptr;

};
#endif
