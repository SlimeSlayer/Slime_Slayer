#include "j1Scene.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "j1Animator.h"
#include "j1FileSystem.h"
#include "j1InputManager.h"
#include "j1Audio.h"

j1Scene::j1Scene() : j1Module()
{
	name = "scene";
}

// Destructor
j1Scene::~j1Scene()
{

}

void j1Scene::Init()
{
	active = true;
	enabled = true;
}

bool j1Scene::Enable()
{
	active = true;
	return true;
}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	for (pugi::xml_node map_tmx = config.child("map_folder"); map_tmx; map_tmx = map_tmx.next_sibling("map_folder")) {

		map_folder.push_back(std::string(map_tmx.child_value()));

	}

	bool ret = true;
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	//Map build -------------------------------------------
	//Load_Current_Map();

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	//VOLUME ------------------------------------
	if (App->input_manager->GetEvent(VOLUME_UP) == INPUT_DOWN)
	{
		App->audio->VolumeUp();
	}
	else if (App->input_manager->GetEvent(VOLUME_DOWN) == INPUT_DOWN)
	{
		App->audio->VolumeDown();
	}
	// ------------------------------------------
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input_manager->GetEvent(FULL_SCREEN) == INPUT_DOWN)
	{
		if (App->win->fullscreen)
		{
			SDL_SetWindowFullscreen(App->win->window, NULL);
			App->win->fullscreen = false;
		}
		else
		{
			SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_FULLSCREEN);
			App->win->fullscreen = true;
		}
	}

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}