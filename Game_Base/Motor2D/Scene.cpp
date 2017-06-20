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

Scene::Scene() : j1Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{

}

void Scene::Init()
{
	active = false;
	enabled = false;
}

bool Scene::Enable()
{
	//Map build -------------------------------------------
	//Floor -----------------
	floor_collider = App->physics->CreateRectangle(2500, 955, 50000, 15, COLLISION_TYPE::MAP_COLLISION, BODY_TYPE::MAP_BODY);
	floor_collider->body->SetType(b2BodyType::b2_staticBody);
	floor_collider->body->GetFixtureList()->SetFriction(0.0f);

	//Front Parallax --------
	front_parallax = new Parallax(3);
	front_parallax->SetTexture(App->tex->Load("scene/ground_texture.jpg"));
	front_parallax->SetTextureRect({ 0,0,800,100 });
	front_parallax->SetPosition(0, 900);

	//Mid Parallax ----------
	mid_parallax = new Parallax(5);
	mid_parallax->SetTexture(App->tex->Load("scene/mid_texture.png"));
	mid_parallax->SetTextureRect({ 0,0,400,450 });
	mid_parallax->SetPosition(0, 450);

	//Back Parallax ---------
	back_parallax = new Parallax(2);
	back_parallax->SetTexture(App->tex->Load("scene/skyline_texture.jpg"));
	back_parallax->SetTextureRect({ 0,0,1600,900 });
	back_parallax->SetPosition(0, 0);

	//Play scene music
	App->audio->PlayMusic(MUSIC_ID::MUSIC_IN_GAME);

	active = enabled = true;

	return true;
}

void Scene::Disable()
{
	//Release parallax system
	RELEASE(front_parallax);
	RELEASE(mid_parallax);
	RELEASE(back_parallax);

	//Release scene physic system
	RELEASE(floor_collider);

	enabled = active = false;
}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	for (pugi::xml_node map_tmx = config.child("map_folder"); map_tmx; map_tmx = map_tmx.next_sibling("map_folder")) {

		map_folder.push_back(std::string(map_tmx.child_value()));

	}
	return true;
}

// Called before the first frame
bool Scene::Start()
{
	

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//Blit scene Parallax -----------------------
	back_parallax->Draw(-30);
	mid_parallax->Draw(-40);
	front_parallax->Draw(-30);
	// ------------------------------------------

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

	//EXIT --------------------------------------
	else if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN)
	{
		App->ActiveMainMenu();
	}
	// ------------------------------------------

	//TEST ZONE ---------------------------------
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		Item* jar = App->entities_manager->GenerateItem(JAR_ITEM);
		jar->GetBody()->SetPosition(App->input->GetMouseX() - App->render->camera.x, App->input->GetMouseY() - App->render->camera.y);
	}
	// ------------------------------------------
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	return true;
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
