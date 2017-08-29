#include "j1Gui.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1FileSystem.h"
#include "j1InputManager.h"
#include "Scene.h"
#include "MainMenu.h"

//UI_Elements
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UI_Scroll.h"
#include "UI_Scroll_Bar.h"
#include "UI_Progressive_Bar.h"
#include "UI_String.h"
#include "UI_Text_Box.h"
#include "UI_Popup_Menu.h"
#include "UI_Fixed_Button.h"

j1Gui::j1Gui() : j1Module()
{
	name = "gui";
}

// Destructor
j1Gui::~j1Gui()
{

}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");

	pugi::xml_node atlas_node = conf.child("atlas");

	while (atlas_node != NULL)
	{
		atlas_file_names.push_back(atlas_node.attribute("file").as_string(""));

		//Generate an empty texture_ui with the loaded ID
		Texture_UI new_texture_ui;
		new_texture_ui.texture_id = StrToTextureID(atlas_node.attribute("id").as_string(""));
		ui_textures.push_back(new_texture_ui);

		atlas_node = atlas_node.next_sibling();
	}

	return true;
}

// Called before the first frame
bool j1Gui::Start()
{
	j1Timer time;

	//Load Textures & from loaded texture_ui
	uint size = atlas_file_names.size();
	for (uint k = 0; k < size; k++)
	{
		ui_textures[k].texture = App->tex->Load(atlas_file_names[k].c_str());
	}

	atlas_file_names.clear();
	
	LOG("GUI TIME: %.3f", time.ReadSec());

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	bool ret = true;

	std::list<UI_Element*>::iterator item = screens.begin();
	while (item != screens.end()) {

		if (item._Ptr->_Myval->GetActiveState())
			ret = item._Ptr->_Myval->Update();

		item++;
	}

	if (ItemSelected == nullptr)return ret;

	// Focus Up -------------
	if (App->input_manager->GetEvent(INPUT_EVENT::FOCUS_NEXT) == INPUT_DOWN)
	{
		ItemSelected->GetInputTarget()->GUI_Controller_Input(FOCUS_NEXT);
	}

	// Focus Down -----------
	else if (App->input_manager->GetEvent(INPUT_EVENT::FOCUS_PREV) == INPUT_DOWN || App->input_manager->GetEvent(INPUT_EVENT::FOCUS_PREV) == INPUT_REPEAT)
	{
		ItemSelected->GetInputTarget()->GUI_Controller_Input(FOCUS_PREV);
	}

	// Accept ---------------
	else if (App->input_manager->GetEvent(INPUT_EVENT::ACCEPT) == INPUT_DOWN)
	{
		ItemSelected->GetInputTarget()->GUI_Controller_Input(ACCEPT);
	}

	// Add Value ------------
	else if (App->input_manager->GetEvent(INPUT_EVENT::ADD_VALUE) == INPUT_DOWN || App->input_manager->GetEvent(INPUT_EVENT::ADD_VALUE) == INPUT_REPEAT)
	{
		ItemSelected->GetInputTarget()->GUI_Controller_Input(ADD_VALUE);
	}

	// Rest Value -----------
	else if (App->input_manager->GetEvent(INPUT_EVENT::REST_VALUE) == INPUT_DOWN || App->input_manager->GetEvent(INPUT_EVENT::REST_VALUE) == INPUT_REPEAT)
	{
		ItemSelected->GetInputTarget()->GUI_Controller_Input(REST_VALUE);
	}

	return ret;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	if (App->input_manager->GetEvent(UI_DEBUG_MODE) == INPUT_DOWN)
	{
		App->ui_debug = !App->ui_debug;
	}

	// Draw the UI screens
	std::list<UI_Element*>::iterator item = screens.begin();
	while (item != screens.end()) {

		if (item._Ptr->_Myval->GetActiveState() && item._Ptr->_Myval->GetInputTarget()->enabled)
			item._Ptr->_Myval->Draw(App->ui_debug);

		item++;
	}

	if (ItemSelected != nullptr && App->gui->controller_mode && App->render->GetCurrentEfType() != RENDER_EF_TYPE::FADE_EFFECT )
	{
		App->render->DrawCircle(ItemSelected->GetBox()->x + ItemSelected->GetBox()->w * 0.5f, ItemSelected->GetBox()->y + ItemSelected->GetBox()->h * 0.5f, 100, 55, 25, 250, 255, false);
	}

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	//LOG("Freeing GUI");
	bool ret = true;

	// CleanUp (Clear Lists)
	screens.clear();
	cursors_rects.clear();
	cursors_pivots.clear();

	UI_TYPE gui_type = UNDEFINED;
	std::list<UI_Element*>::iterator item = gui_elements.end();
	while (item != gui_elements.begin())
	{
		item--;
		delete item._Ptr->_Myval;
	}
	gui_elements.clear();

	uint size = ui_textures.size();
	for (uint k = 0; k < size; k++)
	{
		App->tex->UnLoad(ui_textures[k].texture);
	}
	ui_textures.clear();

	return true;
}

TEXTURE_ID j1Gui::StrToTextureID(const char * str)
{
	if (strcmp(str, "atlas") == 0)				return ATLAS;
	if (strcmp(str, "atlas_test") == 0)			return ATLAS_TEST;
	if (strcmp(str, "github_icon") == 0)		return GITHUB_ICON;
	if (strcmp(str, "avatar_icon") == 0)		return AVATAR_ICON;
	if (strcmp(str, "wave_shield_icon") == 0)	return WAVE_SHIELD_ICON;
	return TEXTURE_NONE;
}




void j1Gui::ActiveControllerMode()
{
	controller_mode = true;
	SDL_ShowCursor(SDL_DISABLE);

	if(App->GetCurrentScene() != nullptr)ItemSelected = App->GetCurrentScene()->GetCorrectItemToSelect();
	else ItemSelected = App->main_menu->GetCorrectItemToSelect();

	LOG("Controller Mode Activated!");
}

void j1Gui::DeactivateControllerMode()
{
	controller_mode = false;
	SDL_ShowCursor(SDL_ENABLE);
	if (ItemSelected != nullptr)
	{
		//Un focus item selected
	}
	LOG("Controller Mode Deactivated!");
}

// =================================================================
SDL_Texture * j1Gui::Get_UI_Texture(TEXTURE_ID tex_id)
{
	uint size = ui_textures.size();
	for (uint k = 0; k < size; k++)
	{
		if (ui_textures[k].texture_id == tex_id)return ui_textures[k].texture;
	}

	return nullptr;
}

uint j1Gui::PushScreen(const UI_Element* new_screen)
{
	screens.push_back((UI_Element*)new_screen);
	return screens.size();
}

UI_Element* j1Gui::GenerateUI_Element(UI_TYPE element_type)
{
	UI_Element* new_element = nullptr;
	switch (element_type)
	{
	case UNDEFINED:
		new_element = new UI_Element();
		break;

	case BUTTON:
		new_element = new UI_Button();
		break;

	case TEXT_BOX:
		new_element = new UI_Text_Box();
		break;

	case STRING:
		new_element = new UI_String();
		break;

	case IMG:
		new_element = new UI_Image();
		break;

	case SCROLL:
		new_element = new UI_Scroll();
		break;

	case SCROLL_BAR:
		new_element = new UI_Scroll_Bar();
		break;

	case PROGRESSIVE_BAR:
		new_element = new UI_Progressive_Bar();
		break;

	case POPUP_MENU:
		new_element = new UI_Popup_menu();
		break;

	case FIXED_BUTTON:
		new_element = new UI_Fixed_Button();
		break;
	}

	if (new_element != nullptr)
	{
		gui_elements.push_back(new_element);
	}

	return new_element;
}

UI_Element * j1Gui::GetActiveScreen()
{
	std::list<UI_Element*>::iterator item = screens.begin();
	while (item._Ptr != NULL)
	{
		if (item._Ptr->_Myval->GetActiveState())return item._Ptr->_Myval;
		item++;
	}
	return nullptr;
}

uint j1Gui::CalculateUpperElement(UI_Element* parent, uint layer)
{
	if (parent == nullptr)return 0;

	std::list<UI_Element*>::iterator item = parent->childs.begin();
	std::list<UI_Element*>::iterator end = parent->childs.end();

	while (item != end) {

		layer = CalculateUpperElement(item._Ptr->_Myval, layer);

		if (item._Ptr->_Myval->GetLogicalLayer() > layer && item._Ptr->_Myval->GetActiveState() && item._Ptr->_Myval->MouseIsIn())
		{
			layer = item._Ptr->_Myval->GetLogicalLayer();
		}
		item++;

	}
	upper_element = layer;

	return uint(layer);
}

void j1Gui::ChangeMouseTexture(CURSOR_TARGET type)
{
	cursor_type = type;
	cursor_rect = cursors_rects[type];
	cursor_pivot = cursors_pivots[type];
}

void j1Gui::LoadCursorTypes()
{
	int vector_size = 8;
	cursors_rects.reserve(vector_size);
	cursors_pivots.reserve(vector_size);

	//Load Menu configuration data from loaded folder
	char* buffer = nullptr;
	int size = App->fs->Load("gui/UI_Icons.xml", &buffer);
	pugi::xml_document icons_data;
	pugi::xml_parse_result result = icons_data.load_buffer(buffer, size);
	RELEASE_ARRAY(buffer);

	pugi::xml_node unit_node = icons_data.first_child().child("Mouse").first_child();

	SDL_Rect rect = { 0,0,0,0 };
	iPoint pivot = { 0,0 };

	int i = 0;
	while (i < vector_size)
	{
		rect = { unit_node.attribute("x").as_int(0),unit_node.attribute("y").as_int(0),unit_node.attribute("w").as_int(0),unit_node.attribute("h").as_int(0) };

		float pivx = unit_node.attribute("pX").as_float(0.0f)*rect.w;
		pivx = (pivx >(floor(pivx) + 0.5f)) ? ceil(pivx) : floor(pivx);
		pivot.x = (int)pivx;
		float pivy = unit_node.attribute("pY").as_float(0.0f)*rect.h;
		pivy = (pivy > (floor(pivy) + 0.5f)) ? ceil(pivy) : floor(pivy);
		pivot.y = (int)pivy;

		cursors_rects.push_back(rect);
		cursors_pivots.push_back(pivot);

		unit_node = unit_node.next_sibling();
		i++;
	}

}

void j1Gui::SetDefaultInputTarget(j1Module * target)
{
	default_input_target = target;
}

j1Module * j1Gui::GetDefaultInputTarget() const
{
	if (default_input_target == nullptr)return nullptr;
	return default_input_target;
}

char * j1Gui::SetStringFromInt(int number)
{
	static char buffer[128];
	int ret = snprintf(buffer, sizeof(buffer), "%d", number);
	return buffer;
}

char * j1Gui::SetStringFromFloat(float number)
{
	static char buffer[128];
	int ret = snprintf(buffer, sizeof(buffer), "%.3f", number);
	return buffer;
}
