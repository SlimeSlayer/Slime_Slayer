#include "j1Player.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1InputManager.h"
#include "j1Physics.h"

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
bool j1Player::Start()
{
	avatar = App->entities_manager->GenerateCreature(PLAYER_CREATURE);

	return true;
}

bool j1Player::Update(float dt)
{
	INPUT_STATE go_left_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_LEFT);
	INPUT_STATE go_right_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_RIGHT);
	INPUT_STATE jump_input_state = App->input_manager->GetEvent(INPUT_EVENT::JUMP);
	INPUT_STATE crouch_input_state = App->input_manager->GetEvent(INPUT_EVENT::CROUCH);
	
	b2Vec2 linear_vel = avatar->GetBody()->body->GetLinearVelocity();

	if (!avatar->GetBody()->IsInContact())return true;

	if (go_left_input_state == INPUT_REPEAT)
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(-5.0f, linear_vel.y));
	}
	else if (go_right_input_state == INPUT_REPEAT)
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(5.0f, linear_vel.y));
	}
	else
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(0.0f, linear_vel.y));
	}
	if (jump_input_state == INPUT_DOWN)
	{
		avatar->GetBody()->body->ApplyForceToCenter(b2Vec2(0.0f, -30.0f), true);
	}

	return true;
}
