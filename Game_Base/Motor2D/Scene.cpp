#include "Scene.h"

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
#include "j1Physics.h"
#include "j1EntitiesManager.h"

#include "Parallax.h"

// Constructor ==================================
Scene::Scene() : j1Module()
{

}

// Destructor ===================================
Scene::~Scene()
{

}

// Game Loop ====================================
void Scene::Init()
{
	active = false;
	enabled = false;
}

void Scene::Disable()
{
	//Release parallax system
	if (front_parallax != nullptr)RELEASE(front_parallax);
	if (mid_parallax != nullptr)RELEASE(mid_parallax);
	if (back_parallax != nullptr)RELEASE(back_parallax);

	//Release scene physic system
	if (floor_collider != nullptr)RELEASE(floor_collider);

	enabled = active = false;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	
	if (front_parallax != nullptr) delete front_parallax;
	if (mid_parallax != nullptr)delete mid_parallax;
	if (back_parallax != nullptr)delete back_parallax;
	
	if (floor_collider != nullptr)delete floor_collider;

	return true;
}

// Functionality ================================
void Scene::UpdateParallax(float disp)
{
	mid_parallax->Displace(disp * MID_PARALLAX_VAL * App->GetDT());
	back_parallax->Displace(disp * BACK_PARALLAX_VAL * App->GetDT());
}
