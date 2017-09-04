#define _CRT_SECURE_NO_WARNINGS

#include <iostream>		
#include <sstream> 
#include "p2Log.h"

#include "j1App.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1InputManager.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1FileSystem.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "j1Animator.h"
#include "j1Video.h"
#include "j1Physics.h"
#include "j1EntitiesManager.h"
#include "j1ParticleManager.h"
#include "j1Player.h"

#include "MainMenu.h"
#include "Tutorial.h"
#include "Endless.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{

	PERF_START(ptimer);

	input = new j1Input();
	input_manager = new j1InputManager();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	fs = new j1FileSystem();
	font = new j1Fonts();
	gui = new j1Gui();
	video = new j1Video();
	physics = new j1Physics();
	animator = new j1Animator();
	entities_manager = new j1EntitiesManager();
	particle_manager = new j1ParticleManager();
	player = new j1Player();

	main_menu = new MainMenu();
	tutorial = new Tutorial();
	endless = new Endless();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(fs);
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(font);
	AddModule(video);
	AddModule(physics);
	AddModule(gui);
	AddModule(animator);
	AddModule(entities_manager);
	AddModule(particle_manager);
	AddModule(player);


	// scene last
	AddModule(main_menu);
	AddModule(tutorial);
	AddModule(endless);

	// render last to swap buffer
	AddModule(render);
	AddModule(input_manager);

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	for (std::list<j1Module*>::iterator item = modules.begin(); item != modules.end(); item++)
	{
		RELEASE(item._Ptr->_Myval);
	}

	modules.clear();
}

APP_CONTEXT j1App::StrToAppContext(const char * str) const
{
	if (strcmp(str, "in_game_context") == 0)		return IN_GAME_CONTEXT;
	if (strcmp(str, "pause_context") == 0)			return PAUSE_CONTEXT;
	if (strcmp(str, "main_menu_context") == 0)		return MAIN_MENU_CONTEXT;
	return UNKNOWN_CONTEXT;
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;

	config = LoadConfig(config_file);

	//Load App config data
	if (config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title = app_config.child("title").child_value();
		organization = app_config.child("organization").child_value();

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if (cap > 0)
		{
			capped_ms = 1000 / cap;
		}
	}

	//Call modules Awake method
	if (ret == true)
	{
		for (std::list<j1Module*>::iterator item = modules.begin(); item != modules.end() && ret == true; item++)
		{
			ret = item._Ptr->_Myval->Awake(config.child(item._Ptr->_Myval->name.c_str()));
			modules.end();
		}
	}

	PERF_PEEK(ptimer);

	//Load Game State
	load_game = App->fs->general_save_file;
	want_to_g_load = true;
	this->LoadGameNow();

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	//Start all the modules
	PERF_START(ptimer);
	bool ret = true;
	std::list<j1Module*>::iterator item = modules.begin();

	while (item != modules.end() != NULL && ret == true)
	{
		ret = item._Ptr->_Myval->Start();
		item++;
	}
	startup_time.Start();

	PERF_PEEK(ptimer);

	//Build Loading Screen
	//String
	loading_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	loading_string->SetFont(App->font->GetFontByID(FONT_ID::MENU_UI_FONT)->font);
	loading_string->SetInputTarget(App->gui);
	loading_string->SetUseCamera(false);
	loading_string->SetString("Loading...");
	loading_string->AdjustBox();
	loading_string->SetBoxPosition(App->render->viewport.w * 0.5 - loading_string->GetBox()->w * 0.5f, App->render->viewport.h * 0.5 - loading_string->GetBox()->h * 0.5);
	
	//Bar
	loading_bar = (UI_Progressive_Bar*)App->gui->GenerateUI_Element(UI_TYPE::PROGRESSIVE_BAR);
	SDL_Rect rect = { App->render->viewport.w * 0.5 - 800 * 0.5f, App->render->viewport.h * 0.7,800,100 };
	loading_bar->SetBox(rect);
	loading_bar->SetFullColor({ 255,155,255,255 });
	loading_bar->SetToEmptyColor({0,0,0,255});
	loading_bar->SetEmptyColor({ 0,0,0,255 });
	loading_bar->SetInputTarget(main_menu);
	loading_bar->SetCurrentValue(0.0f);

	//Percent string
	loading_percent_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	loading_percent_string->SetFont(App->font->GetFontByID(FONT_ID::MENU_DIALOG_FONT)->font);
	loading_percent_string->SetInputTarget(App->gui);
	loading_percent_string->SetUseCamera(false);
	loading_percent_string->SetString("0 %");
	loading_percent_string->AdjustBox();
	loading_percent_string->SetBoxPosition(loading_bar->GetBox()->x + loading_bar->GetBox()->w * 0.5f - loading_percent_string->GetBox()->w * 0.5, loading_bar->GetBox()->y - loading_bar->GetBox()->h * 0.5 - -loading_percent_string->GetBox()->h * 0.5);


	//App starts in the main menu context
	app_context = MAIN_MENU_CONTEXT;

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;

	//Enable loading process
	if (want_to_enable)
	{
		if (App->render->GetCurrentEfType() == RENDER_EF_TYPE::LAYER_EFFECT || is_loading)
		{
			if (!is_loading)
			{
				loading_bar->SetCurrentValue(0.0f);
				loading_bar->SetMaxValue(modules_to_enable.size());
				loading_bar->GenerateTexture();
			}
			is_loading = true;
			EnableActiveModulesNow();
		}
	}


	//Game Loop
	PrepareUpdate();

	if (input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if (ret == true)
	{
		ret = PreUpdate();
	}

	if (ret == true)
	{
		ret = DoUpdate();
	}

	if (ret == true)
	{
		ret = PostUpdate();
	}
	
	FinishUpdate();

	return ret;
}

void j1App::EnableActiveModulesNow()
{

	if (App->load_scene_enabled && App->render->GetCurrentEfType() == RENDER_EF_TYPE::LAYER_EFFECT)
	{
		((Layer_Effect*)App->render->GetCurrentRenderEffect())->End();
		SDL_Color color = { 0,0,0,255 };
		App->render->CallRenderEffect(RENDER_EF_TYPE::FADE_EFFECT, true, FADE_IN_TIME, 255.0, 0.0, color, nullptr);
	}

	//Disable all the modules ready to be disabled at the first iteration
	uint size = modules_to_disable.size();
	for (uint k = 0; k < size; k++)
	{
		modules_to_disable[k]->Disable();
	}
	modules_to_disable.clear();

	if (enable_timer.Read() < ENABLE_TIME_MARK && load_scene_enabled)return;

	//Enable one module for iteration
	if (modules_to_enable[enable_index]->Enable())
	{
		//Update loading IU
		enable_index++;
		//Bar
		loading_bar->AddValue(1);
		loading_bar->UpdateTexture();
		//Percent string
		float cent_val = MAX(0, (loading_bar->GetCurrentValue() / loading_bar->GetMaxValue()) * 100);
		cent_val = (cent_val >(floor(cent_val) + 0.5f)) ? ceil(cent_val) : floor(cent_val);
		char buffer[6];
		_itoa(cent_val, buffer, 10);
		std::string w_str = buffer;
		w_str += " %";
		loading_percent_string->SetString(w_str.c_str());
		loading_percent_string->AdjustBox();
		loading_percent_string->SetBoxPosition(loading_bar->GetBox()->x + loading_bar->GetBox()->w * 0.5 - loading_percent_string->GetBox()->w * 0.5, loading_bar->GetBox()->y + loading_bar->GetBox()->h * 0.5 - loading_percent_string->GetBox()->h * 0.5);
		//Reset enable timer
		enable_timer.Start();
	}

	//Check if all the modules have been enabled
	size = modules_to_enable.size();
	if (enable_index == size)
	{
		//If true enable process ends
		want_to_enable = false;

		if (!App->load_scene_enabled)
		{
			((Layer_Effect*)App->render->GetCurrentRenderEffect())->End();
			SDL_Color color = { 0,0,0,255 };
			App->render->CallRenderEffect(RENDER_EF_TYPE::FADE_EFFECT, true, FADE_IN_TIME, 255.0, 0.0, color, nullptr);
			App->is_loading = false;
			App->load_scene_enabled = false;
		}
		else
		{
			((Layer_Effect*)App->render->GetCurrentRenderEffect())->End();
			SDL_Color color = { 0,0,0,255 };
			Render_Effect* ef = App->render->CallRenderEffect(RENDER_EF_TYPE::FADE_EFFECT, true, FADE_OUT_TIME, 0.0, 255.0, color, &j1App::EndLoadProcess);
			App->render->CallRenderEffect(RENDER_EF_TYPE::FADE_EFFECT, true, FADE_IN_TIME, 255.0, 0.0, color, nullptr);
		}
		modules_to_enable.clear();
		enable_index = 0;
	}

}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	char* buf;
	int size = App->fs->Load("config.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE_ARRAY(buf);

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if ((want_to_g_save ^ want_to_p_save) && !want_to_enable)
	{
		SavegameNow();
	}

	if ((want_to_g_load ^ want_to_p_load) == true && !want_to_enable)
	{
		LoadGameNow();
	}

	// Frame rate calculations --

	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[50];

	sprintf_s(title, 50, "Topota Game || DT: %u || FPS: %u", last_frame_ms, frames_on_last_update);
	

	App->win->SetTitle(title);

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		j1PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end())
	{
		if (item._Ptr->_Myval->enabled) ret = item._Ptr->_Myval->PreUpdate();

		item++;
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end())
	{
		if (item._Ptr->_Myval->enabled)ret = item._Ptr->_Myval->Update(dt);

		item++;
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end() && ret)
	{
		if (item._Ptr->_Myval->enabled)ret = item._Ptr->_Myval->PostUpdate();

		item++;
	}

	if (ret)ret = !want_to_quit;

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	std::list<j1Module*>::const_iterator item = modules.end();
	item--;

	while (item != modules.begin() && ret == true)
	{
		ret = item._Ptr->_Myval->CleanUp();
		item--;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if (index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.c_str();
}

// ---------------------------------------
float j1App::GetDT() const
{
	return dt;
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.c_str();
}

// Load / Save
void j1App::LoadGeneralData(const char* file)
{
	if (!fs->Exists(file))
	{
		LOG("ERROR: General data file don't exist!");
		return;
	}

	want_to_g_load = true;
	load_game = std::string(fs->save_directory.c_str()) + std::string(file);
}

void j1App::LoadPartyData(const char * file)
{
	if (!fs->Exists(file))
	{
		LOG("ERROR: Party data file don't exist!");
		return;
	}
	
	want_to_p_load = true;
	load_game = std::string(fs->save_directory.c_str()) + std::string(file);
}

// ---------------------------------------
void j1App::SaveGeneralData(const char* file)
{
	want_to_g_save = true;
	save_game = file;
}

void j1App::SavePartyData(const char * file)
{
	want_to_p_save = true;
	save_game = file;
}

// ---------------------------------------

bool j1App::IsXMLdir(const char * dir) const
{
	uint len = strlen(dir);
	if (len < 4)return false;
	bool format_zone = false;
	char* temp = new char[strlen(dir)];
	uint j = 0;
	for (uint k = 0; k < len; k++)
	{
		if (dir[k] == '.')format_zone = true;
		else if (format_zone)
		{
			temp[j] = dir[k];
			j++;
		}

	}
	temp[j] = '\0';
	std::string str = temp;
	delete temp;
	if (str == "xml")return true;

	return false;
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	char* buffer;
	uint size = fs->Load(load_game.c_str(), &buffer);

	if (size > 0)
	{
		pugi::xml_document data;
		pugi::xml_node root;

		pugi::xml_parse_result result = data.load_buffer(buffer, size);
		RELEASE(buffer);

		if (result != NULL)
		{
			LOG("Loading new Game State from %s...", load_game.c_str());

			if(want_to_g_load)root = data.child("game_state");
			else if (want_to_p_load)root = data.child("party_state");

			std::list<j1Module*>::const_iterator item = modules.begin();
			ret = true;

			while (item != modules.end() && ret == true)
			{
				if (want_to_g_load)ret = item._Ptr->_Myval->GeneralLoad(root.child(item._Ptr->_Myval->name.c_str()));
				if (want_to_p_load)ret = item._Ptr->_Myval->PartyLoad(root.child(item._Ptr->_Myval->name.c_str()));
				
				item++;
			}

			data.reset();
			if (ret == true)
				LOG("...finished loading");
			else
				LOG("...loading process interrupted with error on module %s", (item._Ptr->_Myval != NULL) ? item._Ptr->_Myval->name.c_str() : "unknown");
		}
		else
			LOG("Could not parse game state xml file %s. pugi error: %s", load_game.c_str(), result.description());
	}
	else
		LOG("Could not load game state xml file %s", load_game.c_str());

	want_to_p_load = want_to_g_load = false;

	return ret;
}

bool j1App::SavegameNow()
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.c_str());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	if(want_to_p_save)root = data.append_child("party_state");
	else if(want_to_g_save)root = data.append_child("game_state");

	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end() && ret == true)
	{
		if(want_to_p_save)ret = item._Ptr->_Myval->PartySave(root.append_child(item._Ptr->_Myval->name.c_str()));
		else if (want_to_g_save)ret = item._Ptr->_Myval->GeneralSave(root.append_child(item._Ptr->_Myval->name.c_str()));
		item++;
	}

	if (ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		fs->Save(save_game.c_str(), stream.str().c_str(), stream.str().length());
		LOG("... finished saving correctly", save_game.c_str());
	}
	else
		LOG("Save process halted from an error in module %s", (item._Ptr->_Myval != NULL) ? item._Ptr->_Myval->name.c_str() : "unknown");

	data.reset();
	want_to_g_save = want_to_p_save = false;
	return ret;
}

void j1App::EnableActiveModules()
{
	std::list<j1Module*>::const_iterator item = modules.begin();
	while (item != modules.end())
	{
		if (item._Ptr->_Myval->active) modules_to_enable.push_back(item._Ptr->_Myval);

		item++;
	}

	want_to_enable = true;
}

j1Module * j1App::GetModule(const char* module_name)
{
	std::list<j1Module*>::const_iterator item = modules.begin();

	while (item != modules.end())
	{
		if (strcmp(item._Ptr->_Myval->name.c_str(),module_name) == 0)
		{
			return item._Ptr->_Myval;
		}

		item++;
	}
	return nullptr;
}

uint j1App::GetModulesNum() const
{
	return modules.size();
}

j1Module * j1App::GetModuleAt(uint index)const
{
	std::list<j1Module*>::const_iterator item = modules.begin();

	for (uint k = 0; k < index; k++)
		item++;

	return item._Ptr->_Myval;
}

pugi::xml_node j1App::GetConfigXML() const
{
	return config_node;
}

void j1App::DrawLoadProcess()
{
	App->render->DrawQuad(App->render->viewport, 200, 0, 200, 255, true, false);
	App->render->Blit(loading_string->GetTextTexture(), loading_string->GetBox()->x, loading_string->GetBox()->y, NULL, true);
	App->render->Blit(loading_bar->GetTexture(), loading_bar->GetBox()->x, loading_bar->GetBox()->y, NULL, true);
	App->render->Blit(loading_percent_string->GetTextTexture(), loading_percent_string->GetBox()->x, loading_percent_string->GetBox()->y, NULL, true);
}

void j1App::EndLoadProcess()
{
	LOG("YES");
	is_loading = load_scene_enabled = false;
}

uint j1App::GetCurrentFrameTime() const
{
	return frame_time.Read();
}

void j1App::SetQuit()
{
	want_to_quit = true;
}

bool j1App::GetQuit() const
{
	return want_to_quit;
}

Scene * j1App::GetCurrentScene() const
{
	return current_scene;
}

void j1App::ActiveMainMenu()
{
	//Clear all the input
	App->input->ResetInputMaps();

	// Deactivate the current scene
	modules_to_disable.push_back(current_scene);
	modules_to_disable.push_back(player);
	modules_to_disable.push_back(entities_manager);
	modules_to_disable.push_back(particle_manager);
	modules_to_disable.push_back(physics);
	modules_to_disable.push_back(animator);

	// Active all the necessary scene modules
	App->main_menu->Active();
	App->particle_manager->Active();

	//Enable active modules
	want_to_enable = true;
	EnableActiveModules();

	//Start render & audio fade
	SDL_Color	color = { 0,0,0,255 };
	App->render->CallRenderEffect(RENDER_EF_TYPE::FADE_EFFECT, true, FADE_OUT_TIME, 0.0, 255.0, color, nullptr);
	App->render->CallRenderEffect(RENDER_EF_TYPE::LAYER_EFFECT, color);

	//Start render & audio fade
	current_scene = nullptr;

	//Set the correct app context
	app_context = MAIN_MENU_CONTEXT;
	
	//Reset Item Selected
	App->gui->ItemSelected = App->main_menu->GetCorrectItemToSelect();
}

void j1App::ActiveTutorial()
{
	//Clear all the input
	App->input->ResetInputMaps();

	// Deactivate all the necessary modules to start/reset the tutorial
	modules_to_disable.push_back(main_menu);
	if(current_scene != nullptr)modules_to_disable.push_back(current_scene);
	modules_to_disable.push_back(player);
	modules_to_disable.push_back(entities_manager);
	modules_to_disable.push_back(particle_manager);
	modules_to_disable.push_back(animator);

	// Active all the necessary scene modules
	App->player->Active();
	App->entities_manager->Active();
	App->particle_manager->Active();
	App->tutorial->Active();
	App->physics->Active();
	App->animator->Active();

	//Enable active modules
	want_to_enable = true;
	load_scene_enabled = true;
	EnableActiveModules();

	//Start render & audio fade
	SDL_Color	color = { 0,0,0,255 };
	App->render->CallRenderEffect(RENDER_EF_TYPE::FADE_EFFECT, true, FADE_OUT_TIME, 0.0, 255.0, color,nullptr);
	App->render->CallRenderEffect(RENDER_EF_TYPE::LAYER_EFFECT, color);
	
	//Set the correct app context
	current_scene = App->tutorial;
	app_context = IN_GAME_CONTEXT;
	pause = false;

	//Reset Item Selected
	App->gui->ItemSelected = current_scene->GetCorrectItemToSelect();
}

void j1App::ActiveEndless()
{
	//Clear all the input
	App->input->ResetInputMaps();

	// Deactivate all the necessary modules to start/reset the endless
	if (current_scene != nullptr)modules_to_disable.push_back(current_scene);
	else modules_to_disable.push_back(main_menu);
	modules_to_disable.push_back(player);
	modules_to_disable.push_back(entities_manager);
	modules_to_disable.push_back(particle_manager);
	modules_to_disable.push_back(physics);
	modules_to_disable.push_back(animator);

	// Active all the necessary scene modules
	App->player->Active();
	App->physics->Active();
	App->entities_manager->Active();
	App->particle_manager->Active();
	App->endless->Active();
	App->animator->Active();

	//Enable active modules
	want_to_enable = true;
	load_scene_enabled = true;
	EnableActiveModules();

	//Start render & audio fade
	SDL_Color	color = { 0,0,0,255 };
	App->render->CallRenderEffect(RENDER_EF_TYPE::FADE_EFFECT, true, FADE_OUT_TIME, 0.0, 255.0, color, nullptr);
	App->render->CallRenderEffect(RENDER_EF_TYPE::LAYER_EFFECT, color);

	//Start render & audio fade
	current_scene = App->endless;
	
	//Set the correct app context
	app_context = IN_GAME_CONTEXT;
	pause = false;

	//Reset Item Selected
	App->gui->ItemSelected = current_scene->GetCorrectItemToSelect();
}
