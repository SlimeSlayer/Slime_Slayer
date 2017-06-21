#include "j1Player.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1InputManager.h"
#include "j1Physics.h"
#include "j1Render.h"
#include "Scene.h"

// Constructors =================================
j1Player::j1Player()
{
	name = "player";
}

// Destructors ==================================
j1Player::~j1Player()
{
	
}

// Game Loop ====================================
void j1Player::Init()
{
	enabled = false;
	active = false;
}

bool j1Player::Enable()
{
	avatar = (Intelligent_Creature*)App->entities_manager->GenerateCreature(PLAYER_CREATURE);
	avatar->GetBody()->SetPosition(1500, 900);
	avatar->GetBody()->body->GetFixtureList()->SetFriction(0.0f);
	return true;
}

bool j1Player::Start()
{


	return true;
}

bool j1Player::Update(float dt)
{
	// Read all player inputs states ------------
	INPUT_STATE go_left_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_LEFT);
	INPUT_STATE go_right_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_RIGHT);
	INPUT_STATE jump_input_state = App->input_manager->GetEvent(INPUT_EVENT::JUMP);
	INPUT_STATE crouch_input_state = App->input_manager->GetEvent(INPUT_EVENT::CROUCH);
	
	
	// Get player necessary data ----------------
	int x_pos = 0, y_pos = 0;
	avatar->GetBody()->GetPosition(x_pos, y_pos);
	b2Vec2 linear_vel = avatar->GetBody()->body->GetLinearVelocity();


	// Center camera at player position ---------
	App->render->camera.x = -x_pos + PLAYER_CAMERA_X;
	App->render->camera.y = -y_pos + PLAYER_CAMERA_Y;

	// AIR INPUT --------------------------------
	if (!avatar->GetBody()->IsInContact())
	{
		if (linear_vel.x < 0)
		{
			App->GetCurrentScene()->UpdateParallax(-avatar->GetMovSpeed());
		}
		else if (linear_vel.x > 0)
		{
			App->GetCurrentScene()->UpdateParallax(avatar->GetMovSpeed());
		}
		return true;
	}

	// ARROWS INPUT -----------------------------
	if (go_left_input_state == INPUT_REPEAT)
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(-avatar->GetMovSpeed(), linear_vel.y));
		App->GetCurrentScene()->UpdateParallax(-avatar->GetMovSpeed());
	}
	else if (go_right_input_state == INPUT_REPEAT)
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(avatar->GetMovSpeed(), linear_vel.y));
		App->GetCurrentScene()->UpdateParallax(avatar->GetMovSpeed());
	}
	else
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(0.0f, linear_vel.y));
	}

	// JUMP INPUT -------------------------------
	if (jump_input_state == INPUT_DOWN)
	{
		avatar->GetBody()->body->ApplyForceToCenter(b2Vec2(0.0f, -avatar->GetJumpForce()), true);
	}

	return true;
}
