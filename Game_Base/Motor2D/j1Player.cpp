#include "j1Player.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1InputManager.h"
#include "j1Physics.h"
#include "j1Render.h"
#include "Scene.h"
#include "Worker.h"
#include "j1Animator.h"

#include "p2Log.h"
#include "UI_Image.h"
#include "UI_Progressive_Bar.h"
#include "UI_String.h"
#include "j1Textures.h"

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
	App->animator->EntityPlay(avatar);

	//Activate Player UI 
	avatar_ui_branch->Activate();
	avatar_ui_branch->ActivateChilds();

	enabled = true;
	active = false;

	return true;
}

void j1Player::Disable()
{
	RELEASE(avatar);

	//Deactivate player UI
	avatar_ui_branch->Desactivate();
	avatar_ui_branch->DesactivateChids();

	active = enabled = false;
}

bool j1Player::Start()
{

	//Build avatar UI -------
	avatar_ui_branch = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	avatar_ui_branch->SetInputTarget(this);

	//Avatar Icon -------------------------------
	avatar_icon = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	avatar_icon->ChangeTextureId(TEXTURE_ID::AVATAR_ICON);
	avatar_icon->SetVisualLayer(10);
	avatar_icon->SetInputTarget(this);
	avatar_icon->SetBoxPosition(40, 40);
	avatar_icon->SetTextureScale(0.5f);
	avatar_icon->AdjustBox();
	avatar_ui_branch->AddChild(avatar_icon);
	// ------------------------------------------

	//Add the built branch at the GUI 
	App->gui->PushScreen(avatar_ui_branch);

	return true;
}

bool j1Player::Update(float dt)
{
	//When player is dead
	if (avatar == nullptr || avatar->GetBody() == nullptr)return true;


	//Update player avatar ----------------------
	avatar->Update();
	if (avatar == nullptr || avatar->GetBody() == nullptr)return true;
	avatar->Draw();	

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
	if (avatar->GetInputBlocked() || App->GetCurrentScene() == nullptr || App->pause)return true;

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
		if (B->entity_related->GetDiplomacy() == ENEMY)
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
