#ifndef _J1_INPUT_MANAGER_H_
#define _J1_INPUT_MANAGER_H_

#include "j1Module.h"
#include <map>

enum INPUT_EVENT
{
	UNKNOWN_INPUT = 0,
	
	//Escape
	ESCAPE,

	//Debug
	COLLIDERS_DEBUG_MODE,
	UI_DEBUG_MODE,

	//Player
	GO_LEFT,
	GO_RIGHT,
	JUMP,
	CROUCH,
	INTERACT,
	ATTACK
};

enum INPUT_STATE
{
	INPUT_NONE = 0,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_REPEAT
};

enum JOYSTICK_STATE
{
	JSTICK_NONE,
	JSTICK_POSITIVE,
	JSTICK_NEGATIVE
};

class j1InputManager : public j1Module
{
public:

	j1InputManager();
	~j1InputManager();

public:

	// Called when before render is available
	bool Awake(pugi::xml_node& );

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:
	
	//Functionality -------------------
	//Used when loading input keys
	INPUT_EVENT StrToInputEvent(const char* str)const;
	void		SendKeyboardInputEvent(int id, INPUT_STATE state);
	void		SendControllerInputEvent(int id, INPUT_STATE state);
	INPUT_STATE GetEvent(INPUT_EVENT _event);

private:

	//Maps with all the input events mapped
	std::multimap<int, INPUT_EVENT> keyboard_events_map; /*for the keyboard*/
	std::multimap<int, INPUT_EVENT> controller_events_map; /*for the controller*/

	//Events that are happening during this frame
	std::multimap<INPUT_EVENT, INPUT_STATE> current_events;

};
#endif // _J1_INPUT_MANAGER_H_
