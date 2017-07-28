#include "j1Player.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1InputManager.h"
#include "j1Physics.h"
#include "j1Render.h"
#include "Scene.h"
#include "Worker.h"
#include "p2Log.h"

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
	//Generate player avatar
	avatar = (Player*)App->entities_manager->GenerateCreature(PLAYER_CREATURE, false);
	PhysBody* bdy_def = avatar->GetBody();
	avatar->SetBody(App->physics->TransformDefToBuilt(avatar->GetBody()));
	RELEASE(bdy_def);
	avatar->GetBody()->body->GetFixtureList()->SetFriction(0.0f);
	avatar->GetBody()->entity_related = avatar;

	active = enabled = true;

	return true;
}

void j1Player::Disable()
{
	RELEASE(avatar);

	active = enabled = false;
}

bool j1Player::Start()
{


	return true;
}

bool j1Player::Update(float dt)
{
	//When player is dead
	if (avatar == nullptr)return true;


	//Update player avatar ----------------------
	avatar->Update();
	if (avatar == nullptr)return true;

	// Read all player inputs states ------------
	INPUT_STATE go_left_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_LEFT);
	INPUT_STATE go_right_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_RIGHT);
	INPUT_STATE jump_input_state = App->input_manager->GetEvent(INPUT_EVENT::JUMP);
	INPUT_STATE crouch_input_state = App->input_manager->GetEvent(INPUT_EVENT::CROUCH);
	INPUT_STATE attack_input_state = App->input_manager->GetEvent(INPUT_EVENT::ATTACK);
	
	
	// Get player necessary data ----------------
	int x_pos = 0, y_pos = 0;
	avatar->GetBody()->GetPosition(x_pos, y_pos);
	b2Vec2 linear_vel = avatar->GetBody()->body->GetLinearVelocity();


	// Center camera at player position ---------
	App->render->camera.x = -x_pos + PLAYER_CAMERA_X;
	App->render->camera.y = -y_pos + PLAYER_CAMERA_Y;

	// Check if player input is blocked ---------
	if (avatar->GetInputBlocked() || App->GetCurrentScene() == nullptr)return true;

	// ATTACK INPUT -----------------------------
	if (attack_input_state == INPUT_DOWN || (attack_input_state == INPUT_REPEAT && avatar->ReadyToAttack()))
	{
		if (avatar->GetDirection() == RIGHT)
		{
			avatar->AttackRight();
		}
		else if (avatar->GetDirection() == LEFT)
		{
			avatar->AttackLeft();
		}
	}

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
		avatar->SetDirection(LEFT);
	}
	else if (go_right_input_state == INPUT_REPEAT)
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(avatar->GetMovSpeed(), linear_vel.y));
		App->GetCurrentScene()->UpdateParallax(avatar->GetMovSpeed());
		avatar->SetDirection(RIGHT);
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

bool j1Player::CleanUp()
{
	if (avatar != nullptr)RELEASE(avatar);

	return true;
}

// Collisions functions =========================
void j1Player::OnSensorCollision(PhysBody * A, PhysBody * B)
{
	/*A is the sensor*/

	if (avatar == nullptr) return;

	// Attack sensor
	INPUT_STATE atk_input_state = App->input_manager->GetEvent(INPUT_EVENT::ATTACK);
	//Check if player is ready to attack
	if ((atk_input_state == INPUT_DOWN || (atk_input_state == INPUT_REPEAT && avatar->ReadyToAttack())) && A == avatar->GetCurrentAttackArea())
	{
		//Check if B is an enemy
		if (B->entity_related->GetEntityType() == CREATURE && ((Creature*)B->entity_related)->GetCreatureType() == CREATURE_TYPE::BASIC_ENEMY_CREATURE)
		{
			A->entity_related->worker.AddAction(A->entity_related->worker.GenerateSimpleAttackAction(A->entity_related, (Creature*)B->entity_related));
		}
	}
}

//Functionality =================================
void j1Player::PlayerDeath()
{
	//Clean avatar
	avatar = nullptr;

	//Start scene death mode
	App->GetCurrentScene()->PlayerDeathMode();
}
