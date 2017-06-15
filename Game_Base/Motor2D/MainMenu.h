#ifndef __j1MENU_H__
#define __j1MENU_H__

#include "j1Module.h"
class UI_Button;
class UI_Image;

class Menu : public j1Module
{
public:

	Menu();
	~Menu();

public:

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
	UI_Button*	start_button = nullptr;
	UI_Button*	settings_button = nullptr;
	UI_Image*	settings_menu = nullptr;

};
#endif
