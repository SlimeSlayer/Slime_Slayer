#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"

#include "p2Point.h"
#include "SDL\include\SDL_rect.h"

#define CONTROLLER_ON_ADD_VALUE		350
#define CONTROLLER_ON_REST_VALUE	-350

class UI_Element;
struct SDL_Texture;
enum UI_TYPE;


enum GUI_INPUT {

	UP_ARROW,
	DOWN_ARROW,
	LEFT_ARROW,
	RIGHT_ARROW,
	MOUSE_LEFT_BUTTON_DOWN,
	MOUSE_LEFT_BUTTON_REPEAT,
	MOUSE_LEFT_BUTTON_UP,
	MOUSE_RIGHT_BUTTON,
	MOUSE_IN,
	MOUSE_OUT,
	SUPR,
	BACKSPACE,
	ENTER,
	TAB
};

enum CURSOR_TARGET
{
	DEFAULT,
	SELECT,
	ATTACK_,
	REPAIR,
	GUARD,
	CAPTURE,
	MARK,
	FIRE
};

enum TEXTURE_ID
{
	TEXTURE_NONE = 0,
	ATLAS,
	ATLAS_TEST
};
// ---------------------------------------------------

/// TEXTURE_UI ---------------------------------------
struct Texture_UI
{
	TEXTURE_ID		texture_id = TEXTURE_NONE;
	SDL_Texture*	texture = nullptr;
};
/// --------------------------------------------------

class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:

	//UI elements ---------------------
	std::list<UI_Element*>  gui_elements;
	std::list<UI_Element*>	screens;

	//UI Textures ---------------------
	std::vector<Texture_UI>		ui_textures;
	std::vector<std::string>	atlas_file_names;
	TEXTURE_ID					StrToTextureID(const char* str);

	//Target Module of gui input
	j1Module*				default_input_target = nullptr;

	//Cursor type;
	CURSOR_TARGET			cursor_type;
	SDL_Rect				cursor_rect = {0,0,0,0};
	iPoint					cursor_pivot = {0,0};

	std::vector<SDL_Rect>	cursors_rects;
	std::vector<iPoint>		cursors_pivots;

public:

	mutable UI_Element*	ItemSelected = nullptr;
	mutable uint		upper_element = 0;
	mutable bool		controller_mode = false;

	//Activate/Desactive controller mode
	void ActiveControllerMode();
	void DeactivateControllerMode();
	
	//UI textures functions
	SDL_Texture*		Get_UI_Texture(TEXTURE_ID tex_id);

	//UI elements functions
	uint				PushScreen(const UI_Element* new_screen);
	UI_Element*			GenerateUI_Element(UI_TYPE element_type);

	UI_Element*			GetActiveScreen();
	uint				CalculateUpperElement(UI_Element* parent, uint layer = 0);

	//mouse function
	void				ChangeMouseTexture(CURSOR_TARGET type);
	void				LoadCursorTypes();

	//Draw mouse
	void				DrawMouseTexture();

	//Gui taget funtions
	void				SetDefaultInputTarget(j1Module* target);
	j1Module*			GetDefaultInputTarget()const;

	//Get String from format
	char*				SetStringFromInt(int number);
	char*				SetStringFromFloat(float number);
};

#endif // __GUI_H__