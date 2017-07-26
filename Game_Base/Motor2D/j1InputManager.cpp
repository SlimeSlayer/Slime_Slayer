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
		//Get the current key node scancode id
		SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
		scancode = SDL_GetScancodeFromName(key_node.attribute("id").as_string());

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
				this->events_map.insert(new_key);
			}
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
	events_map.clear();
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

void j1InputManager::SendInputEvent(int id, INPUT_STATE state)
{
	//Search the event in the map
	std::map<int, INPUT_EVENT>::const_iterator key_event = events_map.find(id);

	
	while (key_event != events_map.end())
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