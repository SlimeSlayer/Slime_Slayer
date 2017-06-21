#include "Tutorial.h"

#include "j1App.h"
#include "j1Physics.h"
#include "Parallax.h"
#include "j1Audio.h"
#include "j1InputManager.h"
#include "j1EntitiesManager.h"
#include "j1Input.h"
#include "j1Player.h"

// Constructor ==================================
Tutorial::Tutorial()
{
	name = "tutorial";
}

// Destructor ===================================
Tutorial::~Tutorial()
{

}

// Game Loop ====================================
bool Tutorial::Enable()
{
	//Map build -------------------------------------------
	//Floor -----------------
	floor_collider = App->physics->CreateRectangle(2500, 955, 5000, 15, COLLISION_TYPE::MAP_COLLISION, BODY_TYPE::MAP_BODY);
	floor_collider->body->SetType(b2BodyType::b2_staticBody);
	floor_collider->body->GetFixtureList()->SetFriction(0.0f);

	//End Trigger -----------
	end_trigger = App->physics->CreateRectangle(1700, 900, 10, 100, COLLISION_TYPE::MAP_COLLISION, BODY_TYPE::MAP_BODY);
	end_trigger->body->SetType(b2BodyType::b2_staticBody);
	end_trigger->listener = this;

	//Front Parallax --------
	front_parallax = new Parallax(3);
	front_parallax->SetTexture(App->tex->Load("scenes/ground_texture.jpg"));
	front_parallax->SetTextureRect({ 0,0,800,100 });
	front_parallax->SetPosition(0, 900);

	//Mid Parallax ----------
	mid_parallax = new Parallax(5);
	mid_parallax->SetTexture(App->tex->Load("scenes/mid_texture.png"));
	mid_parallax->SetTextureRect({ 0,0,400,450 });
	mid_parallax->SetPosition(0, 450);

	//Back Parallax ---------
	back_parallax = new Parallax(2);
	back_parallax->SetTexture(App->tex->Load("scenes/skyline_texture.jpg"));
	back_parallax->SetTextureRect({ 0,0,1600,900 });
	back_parallax->SetPosition(0, 0);

	//Play scene music
	App->audio->PlayMusic(MUSIC_ID::MUSIC_IN_GAME);

	//Prepare tutorial entities ---------------------------
	App->player->avatar->SetPosition(1500, 900);

	active = enabled = true;

	return true;
}

void Tutorial::Disable()
{
	LOG("Disabling Tutorial");

	if (front_parallax != nullptr) RELEASE(front_parallax);
	if (mid_parallax != nullptr)RELEASE(mid_parallax);
	if (back_parallax != nullptr)RELEASE(back_parallax);

	if (floor_collider != nullptr)RELEASE(floor_collider);
	if (end_trigger != nullptr)RELEASE(end_trigger);

	active = enabled = false;
}

bool Tutorial::Update(float dt)
{
	//Blit scene Parallax -----------------------
	back_parallax->Draw(-30);
	mid_parallax->Draw(-40);
	front_parallax->Draw(-30);
	// ------------------------------------------

	//EXIT --------------------------------------
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN)
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

bool Tutorial::CleanUp()
{
	LOG("Freeing Tutorial");

	if (front_parallax != nullptr) delete front_parallax;
	if (mid_parallax != nullptr)delete mid_parallax;
	if (back_parallax != nullptr)delete back_parallax;

	if (floor_collider != nullptr)delete floor_collider;
	if (end_trigger != nullptr)delete end_trigger;

	return true;
}

void Tutorial::OnCollision(PhysBody * A, PhysBody * B)
{
	if (A == end_trigger)
	{
		App->ActiveEndless();
	}
}
