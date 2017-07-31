#include "j1InputManager.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "p2Log.h"

// Constructors =================================
j1InputManager::j1InputManager()
{
	name = "input_manager";
}

// Destructors ==================================
j1InputManager::~j1InputManager()
{

}


//Game Loop =====================================
// Called when before render is available
bool j1InputManager::Awake(pugi::xml_node& config)
{
	//Node where first key is located
	pugi::xml_node key_node = config.child("key");

	while (key_node != NULL)
	{
		//Scancodes enums
		SDL_Scancode				scancode = SDL_SCANCODE_UNKNOWN;
		SDL_GameControllerButton	controller_scancode = SDL_CONTROLLER_BUTTON_INVALID;

		//Check if the programmed input is for the keyboard or the controller
		// and get the scancode
		char* str = nullptr;
		str = (char*)key_node.attribute("id").as_string("error");
		if (str != "error")
		{
			scancode = SDL_GetScancodeFromName(str);
		}
		else
		{
			controller_scancode = SDL_GameControllerGetButtonFromString(key_node.attribute("controller_id").as_string());
		}
		
		// Keyboard case
		if (scancode != SDL_SCANCODE_UNKNOWN)
		{
			//Map pair where new key is allocated
			std::pair<int, INPUT_EVENT> new_key;

			//Build pair with the loaded data
			new_key.first = scancode;
			new_key.second = StrToInputEvent(key_node.attribute("event").as_string());
			if (new_key.second == 0)
			{
				LOG("Error Loading Input Key!");
			}
			else
			{
				keyboard_events_map.insert(new_key);
			}
		}
		//Controller case
		else if (controller_scancode != SDL_CONTROLLER_BUTTON_INVALID)
		{
			//Map pair where new key is allocated
			std::pair<int, INPUT_EVENT> new_key;

			//Build pair with the loaded data
			new_key.first = controller_scancode;
			new_key.second = StrToInputEvent(key_node.attribute("event").as_string());
			if (new_key.second == 0)
			{
				LOG("Error Loading Controller Input Key!");
			}
			else
			{
				controller_events_map.insert(new_key);
			}
		}
		//Error case
		else
		{
			LOG("Error loading scancode!");
		}
		
		//Focus the next key node
		key_node = key_node.next_sibling();
	}

	return true;
}


// Called after all Updates
bool j1InputManager::PostUpdate()
{
	current_events.clear();
	return true;
}

// Called before quitting
bool j1InputManager::CleanUp()
{
	current_events.clear();
	keyboard_events_map.clear();
	controller_events_map.clear();

	return true;
}


//Functionality ==================================
INPUT_EVENT j1InputManager::StrToInputEvent(const char * str) const
{
	if (strcmp(str, "colliders_debug") == 0)		return INPUT_EVENT::COLLIDERS_DEBUG_MODE;
	if (strcmp(str, "ui_debug") == 0)				return INPUT_EVENT::UI_DEBUG_MODE;
	if (strcmp(str, "escape") == 0)					return INPUT_EVENT::ESCAPE;
	if (strcmp(str, "go_left") == 0)				return INPUT_EVENT::GO_LEFT;
	if (strcmp(str, "go_right") == 0)				return INPUT_EVENT::GO_RIGHT;
	if (strcmp(str, "jump") == 0)					return INPUT_EVENT::JUMP;
	if (strcmp(str, "crouch") == 0)					return INPUT_EVENT::CROUCH;
	if (strcmp(str, "interact") == 0)				return INPUT_EVENT::INTERACT;
	if (strcmp(str, "attack") == 0)					return INPUT_EVENT::ATTACK;
	return UNKNOWN_INPUT;
}

void j1InputManager::SendKeyboardInputEvent(int id, INPUT_STATE state)
{
	//Search the event in the map
	std::map<int, INPUT_EVENT>::const_iterator key_event = keyboard_events_map.find(id);

	
	while (key_event != keyboard_events_map.end())
	{

		//If the event is found we add it to the current events map
		if (key_event->first == id)
		{
			std::pair<INPUT_EVENT, INPUT_STATE> key_pressed;
			key_pressed.first = key_event->second;
			key_pressed.second = state;
			current_events.insert(key_pressed);
			break;
		}
		key_event++;
	}
}

void j1InputManager::SendControllerInputEvent(int id, INPUT_STATE state)
{
	//Search the event in the map
	std::map<int, INPUT_EVENT>::const_iterator key_event = controller_events_map.find(id);


	while (key_event != controller_events_map.end())
	{

		//If the event is found we add it to the current events map
		if (key_event->first == id)
		{
			std::pair<INPUT_EVENT, INPUT_STATE> key_pressed;
			key_pressed.first = key_event->second;
			key_pressed.second = state;
			
			std::map<INPUT_EVENT, INPUT_STATE>::const_iterator cur_event = current_events.find((INPUT_EVENT)key_pressed.first);
			
			if (cur_event != current_events.end())
			{
				if (state == INPUT_STATE::INPUT_DOWN && cur_event._Ptr->_Myval.second == INPUT_STATE::INPUT_NONE)cur_event._Ptr->_Myval.second = INPUT_STATE::INPUT_DOWN;
				else cur_event._Ptr->_Myval.second = state;
			}

			break;
		}
		key_event++;
	}
}

INPUT_STATE j1InputManager::GetEvent(INPUT_EVENT _event)
{
	if (!current_events.empty())
	{
		std::map<INPUT_EVENT, INPUT_STATE>::iterator item = current_events.find(_event);
		if (item != current_events.end())
		{
			return item->second;
		}
	}
	return INPUT_NONE;
}