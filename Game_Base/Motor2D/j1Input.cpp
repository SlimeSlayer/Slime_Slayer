#define _CRT_SECURE_NO_WARNINGS
#include "j1Input.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Gui.h"
#include "UI_Text_Box.h"
#include "j1Window.h"
#include "j1InputManager.h"

#include "SDL/include/SDL.h"

#define MAX_KEYS 300

j1Input::j1Input() : j1Module()
{
	name = "input";

	keyboard = new j1KeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(j1KeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(j1KeyState) * NUM_MOUSE_BUTTONS);
	memset(controller_axis, JOYSTICK_NOTHING, sizeof(j1JoystickState) * NUM_CONTROLLER_AXIS);
}

// Destructor
j1Input::~j1Input()
{
	RELEASE_ARRAY(keyboard);
}

// Called before render is available
bool j1Input::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool j1Input::Start()
{
	//Identify & Open game controller
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			gamecontroller = SDL_GameControllerOpen(i);
			if (gamecontroller == nullptr)LOG("Gamepad not opened %s", SDL_GetError());
		}
	}

	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool j1Input::PreUpdate()
{
	//During app fade input is cancelled
	if (App->fade_in || App->fade_out)return true;

	//Reset mouse motion
	mouse_motion_x = mouse_motion_y = 0;

	static SDL_Event event;

	// Keyboard keys ------------------
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
			{
				if (App->gui->controller_mode)App->gui->DeactivateControllerMode();
				keyboard[i] = KEY_DOWN;
				App->input_manager->SendKeyboardInputEvent(i, INPUT_STATE::INPUT_DOWN);
			}
			else
			{
				if (App->gui->controller_mode)App->gui->DeactivateControllerMode();
				keyboard[i] = KEY_REPEAT;
				App->input_manager->SendKeyboardInputEvent(i, INPUT_STATE::INPUT_REPEAT);
			}
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
			{
				keyboard[i] = KEY_UP;
				App->input_manager->SendKeyboardInputEvent(i, INPUT_STATE::INPUT_UP);
			}
			else
			{
				keyboard[i] = KEY_IDLE;
				App->input_manager->SendKeyboardInputEvent(i, INPUT_STATE::INPUT_NONE);
			}
		}
	}

	// Mouse keys -----------
	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KEY_DOWN)
		{
			if (App->gui->controller_mode)App->gui->DeactivateControllerMode();
			mouse_buttons[i] = KEY_REPEAT;
		}

		if (mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	// Controller keys ------
	for (int i = 0; i < NUM_CONTROLLER_BUTTONS; ++i)
	{
		if (controller_buttons[i] == KEY_DOWN || controller_buttons[i] == KEY_REPEAT)
		{
			if (!App->gui->controller_mode)App->gui->ActiveControllerMode();

			controller_buttons[i] = KEY_REPEAT;
			App->input_manager->SendControllerInputEvent(i, INPUT_STATE::INPUT_REPEAT);
		}

		if (controller_buttons[i] == KEY_UP)
			controller_buttons[i] = KEY_IDLE;
	}

	/*for (int i = 0; i < NUM_CONTROLLER_AXIS; ++i)
	{
		if (controller_axis[i] == j1JoystickState::JOYSTICK_NEGATIVE)
			App->inputM->JoystickDetected(i, JSTATE::J_NEGATIVE);


		if (controller_axis[i] == j1JoystickState::JOYSTICK_POSITIVE)
			App->inputM->JoystickDetected(i, JSTATE::J_POSITIVE);


	}*/


	// SDL Events ----------------------------------------------
	//SDL_StartTextInput();
	while (SDL_PollEvent(&event) != 0)
	{

		switch (event.type)
		{
		case SDL_QUIT:
			windowEvents[WE_QUIT] = true;
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
				//case SDL_WINDOWEVENT_LEAVE:
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_FOCUS_LOST:
				windowEvents[WE_HIDE] = true;
				break;

				//case SDL_WINDOWEVENT_ENTER:
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:
				windowEvents[WE_SHOW] = true;
				break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouse_buttons[event.button.button - 1] = KEY_DOWN;
			//LOG("Mouse button %d down", event.button.button-1);
			break;

		case SDL_MOUSEBUTTONUP:
			mouse_buttons[event.button.button - 1] = KEY_UP;
			//LOG("Mouse button %d up", event.button.button-1);
			break;

		case SDL_MOUSEMOTION:
		{
			int scale = App->win->GetScale();
			mouse_motion_x = event.motion.xrel / scale;
			mouse_motion_y = event.motion.yrel / scale;
			mouse_x = event.motion.x / scale;
			mouse_y = event.motion.y / scale;
			break;
		}

		case SDL_EventType::SDL_KEYDOWN:

			break;

		case SDL_EventType::SDL_TEXTINPUT:
		{
			if (App->gui->ItemSelected != nullptr && App->gui->ItemSelected->GetUItype() == UI_TYPE::TEXT_BOX && event.text.text[0] != 'Â') {
				((UI_Text_Box*)App->gui->ItemSelected)->PushTextSegment(event.text.text, ((UI_Text_Box*)App->gui->ItemSelected)->GetCursorPos());
				((UI_Text_Box*)App->gui->ItemSelected)->SetCursorPos(((UI_Text_Box*)App->gui->ItemSelected)->GetCursorPos() + 1);
			}
		}
		break;

		case SDL_EventType::SDL_TEXTEDITING:

			/*
			Update the composition text.
			Update the cursor position.
			Update the selection length (if any).
			*/
			//composition = event.edit.text;
			//cursor = event.edit.start;
			//selection_len = event.edit.length;

			break;

		
		case SDL_CONTROLLERAXISMOTION:


			/*if (event.caxis.value < -DEAD_ZONE)
				controller_axis[event.caxis.axis] = j1JoystickState::JOYSTICK_NEGATIVE;

			else
			{
				if (event.caxis.value > DEAD_ZONE)
				{
					if (event.caxis.axis != SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.axis != SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
						controller_axis[event.caxis.axis] = j1JoystickState::JOYSTICK_POSITIVE;
					else
					{
						if (event.caxis.value > TRIGGER_ZONE)
							controller_axis[event.caxis.axis] = j1JoystickState::JOYSTICK_POSITIVE;
					}
				}

				else
				{
					controller_axis[event.caxis.axis] = j1JoystickState::JOYSTICK_NOTHING;

					if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
						App->inputM->JoystickDetected(event.caxis.axis, J_NONE);

					if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
						App->inputM->JoystickDetected(event.caxis.axis, J_NONE);
				}

			}*/

			break;

		case SDL_CONTROLLERBUTTONDOWN:

			LOG("BOTON: %i", event.cbutton.button);
			controller_buttons[event.cbutton.button] = KEY_DOWN;
			App->input_manager->SendControllerInputEvent(event.cbutton.button, INPUT_STATE::INPUT_DOWN);
			break;

		case SDL_CONTROLLERBUTTONUP:
			controller_buttons[event.cbutton.button] = KEY_UP;
			App->input_manager->SendControllerInputEvent(event.cbutton.button, INPUT_STATE::INPUT_UP);
			break;


		case SDL_CONTROLLERDEVICEADDED:

			if (SDL_IsGameController(event.cdevice.which))
			{
				gamecontroller = SDL_GameControllerOpen(event.cdevice.which);
			}

			break;

		case SDL_CONTROLLERDEVICEREMOVED:
			
			if (gamecontroller)
			{
				SDL_GameControllerClose(gamecontroller);
			}
			break;
		}
	}

	return true;
}

// Called before quitting
bool j1Input::CleanUp()
{
	//LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool j1Input::GetWindowEvent(j1EventWindow ev)
{
	return windowEvents[ev];
}

void j1Input::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void j1Input::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

int j1Input::GetMouseX() const
{
	return mouse_x;
}

int j1Input::GetMouseY() const
{
	return mouse_y;
}

void j1Input::ResetInputMaps()
{
	// Keyboard keys ------------------
	for (uint i = 0; i < MAX_KEYS; ++i)
	{
		keyboard[i] = KEY_IDLE;
	}
			
	// Mouse keys -----------
	for (uint i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		mouse_buttons[i] = KEY_IDLE;
	}

	// Controller keys ------
	for (uint i = 0; i < NUM_CONTROLLER_BUTTONS; ++i)
	{
		controller_buttons[i] = KEY_IDLE;
	}
}

