#include "Actions.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1ParticleManager.h"
#include "j1Gui.h"
#include "j1Player.h"
#include "j1InputManager.h"
#include "j1Physics.h"
#include "j1Render.h"
#include "Endless.h"
#include "j1Animator.h"

#include "UI_Element.h"
#include "UI_String.h"

/// Action --------------------------------------
// Constructors =======================
Action::Action(LG_ACTION_TYPE action_type): action_type(action_type)
{

}

// Destructors ========================
Action::~Action()
{

}

// Game Loop ==========================
bool Action::Init()
{
	return true;
}

bool Action::Execute()
{
	return true;
}
/// ---------------------------------------------

/// Dialog_Action -------------------------------
// Constructors =======================
Dialog_Action::Dialog_Action(): Action(LG_DIALOG_ACTION)
{

}

// Destructors ========================
Dialog_Action::~Dialog_Action()
{

}

// Game Loop ==========================
bool Dialog_Action::Init()
{
	LOG("DIALOG_");

	//Stop target body when dialog starts & block input
	target->GetBody()->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	target->LockInput();

	return true;
}

bool Dialog_Action::Execute()
{
	int x = 0, y = 0;
	actor->GetBody()->GetPosition(x, y);
	//Get the first dialog string
	if (current_dialog == nullptr)
	{
		current_dialog = ((NPC*)actor)->GetDialogByIndex(0);
	}

	//Focus the next dialog string
	else if (App->input_manager->GetEvent(INPUT_EVENT::INTERACT) == INPUT_DOWN)
	{
		//Get the next dialog phrase
		current_dialog = ((NPC*)actor)->GetDialogByIndex(++dialog_index);
		
		//When dialog ends player input is unlocked
		if (current_dialog == nullptr)
		{
			LOG("DIALOG_ENDED_");
			target->UnlockInput();
			return true;
		}
	}

	//Update dialog location checking camera position
	if(current_dialog != nullptr)current_dialog->SetBoxPosition(x - current_dialog->GetBox()->w * 0.5 + App->render->camera.x, y - current_dialog->GetBox()->h + App->render->camera.y - DEFAULT_DIALOG_Y_MARGIN);

	//Draw the current string
	current_dialog->Draw(false);

	return false;
}
/// ---------------------------------------------

/// Spawn_Delay_Action --------------------------
// Constructors =======================
Spawn_Delay_Action::Spawn_Delay_Action() : Action(LG_SPAWN_DELAY_ACTION)
{

}

// Destructors ========================
Spawn_Delay_Action::~Spawn_Delay_Action()
{

}

// Game Loop ==========================
bool Spawn_Delay_Action::Init()
{
	if (actor == nullptr || actor->GetBody() == nullptr)return false;

	//Get the previous fixture 
	prev_collision_type = actor->GetBody()->collision_type;
	
	//Change the current entity fixture to a ghost one
	b2Fixture* prev_fixture = actor->GetBody()->body->GetFixtureList();

	b2FixtureDef temp_fixture;
	temp_fixture.shape = prev_fixture->GetShape();
	temp_fixture.density = prev_fixture->GetDensity();
	temp_fixture.restitution = prev_fixture->GetRestitution();
	temp_fixture.friction = prev_fixture->GetFriction();
	temp_fixture.isSensor = false;
	temp_fixture.userData = prev_fixture->GetUserData();
	App->physics->SetFixture(temp_fixture, GHOST_COLLISION);
	actor->GetBody()->body->CreateFixture(&temp_fixture);
	actor->GetBody()->body->DestroyFixture(prev_fixture);

	delay_timer.Start();

	return true;
}

bool Spawn_Delay_Action::Execute()
{
	if (this->delay_timer.Read() > delay)
	{
		//Change the current ghost fixture the original
		b2Fixture* prev_fixture = actor->GetBody()->body->GetFixtureList();

		b2FixtureDef temp_fixture;
		temp_fixture.shape = prev_fixture->GetShape();
		temp_fixture.density = prev_fixture->GetDensity();
		temp_fixture.restitution = prev_fixture->GetRestitution();
		temp_fixture.friction = prev_fixture->GetFriction();
		temp_fixture.isSensor = false;
		temp_fixture.userData = prev_fixture->GetUserData();
		App->physics->SetFixture(temp_fixture, prev_collision_type);
		actor->GetBody()->body->CreateFixture(&temp_fixture);
		actor->GetBody()->body->DestroyFixture(prev_fixture);

		return true;
	}

	return false;
}
/// ---------------------------------------------

/// Move_Action ---------------------------------
// Constructors =======================
Move_Action::Move_Action() : Action(LG_MOVE_ACTION), destination(destination)
{

}

// Destructors ========================
Move_Action::~Move_Action()
{

}

// Game Loop ==========================
bool Move_Action::Init()
{
	//Get actor position
	int act_x = 0, act_y = 0;
	actor->GetBody()->GetPosition(act_x, act_y);

	//Check if the actor is already in the destination
	if (abs(act_x - destination.x) < LOCATION_ERROR_MARGIN)return false;

	//Set actor movement direction
	if (destination.x < act_x)
	{
		mov_direction = -1;
	}
	else
	{
		mov_direction = 1;
	}

	return true;
}

bool Move_Action::Execute()
{
	//Get actor position
	int act_x = 0, act_y = 0;
	actor->GetBody()->GetPosition(act_x, act_y);

	//Calculate the distance to the goal
	int dist_to_goal = act_x - destination.x;

	//Check if the actor has reached the goal
	while (abs(dist_to_goal) > LOCATION_ERROR_MARGIN)
	{
		actor->GetBody()->body->SetLinearVelocity(b2Vec2(((Creature*)actor)->GetMovSpeed() * mov_direction, 0));
		return false;
	}

	//Stop the actor body when is in the goal
	actor->GetBody()->body->SetLinearVelocity(b2Vec2(0, 0));

	return true;
}
/// ---------------------------------------------

/// Move_To_Target_Action -----------------------
// Constructors =======================
Move_To_Target_Action::Move_To_Target_Action() :Action(LG_MOVE_TO_TARGET_ACTION)
{

}

// Destructors ========================
Move_To_Target_Action::~Move_To_Target_Action()
{

}

// Game Loop ==========================
bool Move_To_Target_Action::Init()
{
	if (target->GetBody() == nullptr)
	{
		actor->GetBody()->body->SetLinearVelocity(b2Vec2(0, 0));
		return false;
	}

	//Calculate the goal
	int tar_x, tar_y;
	target->GetBody()->GetPosition(tar_x, tar_y);
	int act_x, act_y;
	actor->GetBody()->GetPosition(act_x, act_y);

	float mid_range = ((Intelligent_Creature*)actor)->GetVisionArea()->width * 0.5;
	
	//Check if the entity is inside goal area
	if (abs(tar_x - act_x) < mid_range)
	{
		return false;
	}
	
	if (tar_x < act_x)
	{
		current_goal.x = tar_x + mid_range;
		mov_direction = -1;
	}
	else
	{
		current_goal.x = tar_x - mid_range;
		mov_direction = 1;
	}

	//Initialize the update timer
	update_timer.Start();

	return true;
}

bool Move_To_Target_Action::Execute()
{
	if (target->GetBody() == nullptr)
	{
		actor->GetBody()->body->SetLinearVelocity(b2Vec2(0, 0));
		return true;
	}

	//Get actor position
	int act_x = 0, act_y = 0;
	actor->GetBody()->GetPosition(act_x, act_y);

	//Update the goal
	if (update_timer.Read() > ACTIONS_UPDATE_RATE)
	{
		this->Init();
	}
	
	//Calculate the distance to the goal
	int dist_to_goal = act_x - current_goal.x;

	//Check if the actor has reached the goal
	while (abs(dist_to_goal) > LOCATION_ERROR_MARGIN)
	{
		actor->GetBody()->body->SetLinearVelocity(b2Vec2(((Creature*)actor)->GetMovSpeed() * mov_direction, 0));
		return false;
	}

	//Stop the actor body when is in the goal
	actor->GetBody()->body->SetLinearVelocity(b2Vec2(0, 0));

	return true;
}
/// ---------------------------------------------

/// Basic_Attack Action -------------------------
// Constructors =======================
Basic_Attack_Action::Basic_Attack_Action() : Action(LG_BASIC_ATTACK_ACTION)
{

}

// Destructors ========================
Basic_Attack_Action::~Basic_Attack_Action()
{

}

bool Basic_Attack_Action::Init()
{
	//Check if the target is already dead
	if (target->GetCurrentLife() == 0)return false;

	//Start the attack rate timer
	attack_timer.Start();
	
	return true;
}

// Game Loop ==========================
bool Basic_Attack_Action::Execute()
{
	if (((Creature*)target)->GetCurrentLife() == 0)return true;
	if (!actor->GetBody()->IsInSpecificContact(1, MAP_COLLISION))return false;
	
	//Check if the target is in range
	int tar_x = 0, tar_y = 0;
	target->GetBody()->GetPosition(tar_x, tar_y);
	int act_x = 0, act_y = 0;
	actor->GetBody()->GetPosition(act_x, act_y);

	//If is not in range actor needs to move
	if (((Intelligent_Creature*)actor)->GetVisionArea()->width < abs(tar_x - act_x))
	{
		Action* act = actor->worker.GenerateAction(LG_ACTION_TYPE::LG_MOVE_TO_TARGET_ACTION, actor, target);
		actor->worker.AddPriorizedAction(act);
		return false;
	}

	//If is in range actor attacks
	if (attack_timer.Read() > ((Creature*)actor)->GetAttackRate())
	{
		//Apply damage
		int cur_life = target->GetCurrentLife();
		int atk_points = ((Creature*)actor)->GetAttackHitPoints();
		cur_life > atk_points ? cur_life -= atk_points : cur_life = 0;
		target->SetCurrentLife(cur_life);

		//Add stun effect at hit target
		SDL_Color hit_color = { 255,255,255,255 };
		target->worker.AddPriorizedAction(target->worker.GenerateAction(LG_ACTION_TYPE::LG_STUN_ACTION, target, 50, hit_color));

		//Reset attack timer
		attack_timer.Start();

		//Generate score particle
		App->particle_manager->GenerateTextParticle(target,PARTICLE_TYPE::ALLY_HITPOINTS_PARTICLE, ((Creature*)actor)->GetAttackHitPoints());
	}

	//When target is dead actor stops
	if (((Creature*)target)->GetCurrentLife() == 0)
	{
		target->worker.AddPriorizedAction(target->worker.GenerateAction(LG_ACTION_TYPE::LG_DIE_ACTION, target));
		return true;
	}

	return false;
}
/// ---------------------------------------------

/// Simple_Attack_Action ------------------------
// Constructors =======================
Simple_Attack_Action::Simple_Attack_Action() : Action(LG_SIMPLE_ATTACK_ACTION)
{

}

// Destructors ========================
Simple_Attack_Action::~Simple_Attack_Action()
{

}

// Game Loop ==========================
bool Simple_Attack_Action::Execute()
{
	if (target->GetCurrentLife() == 0)return true;

	//Apply damage to the target
	//Apply damage
	int cur_life = target->GetCurrentLife();
	int atk_points = ((Creature*)actor)->GetAttackHitPoints();
	cur_life > atk_points ? cur_life -= atk_points : cur_life = 0;
	target->SetCurrentLife(cur_life);

	//Generate a particle with the damage done
	App->particle_manager->GenerateTextParticle(target,PARTICLE_TYPE::ENEMY_HITPOINTS_PARTICLE, atk_points);

	//If the target is dead call die action
	if (target->GetCurrentLife() == 0)
	{
		target->worker.AddPriorizedAction(target->worker.GenerateAction(LG_ACTION_TYPE::LG_DIE_ACTION, target));
	}
	else
	{
		//Add stun effect at hit target
		SDL_Color hit_color = { 255,255,255,255 };
		target->worker.AddPriorizedAction(target->worker.GenerateAction(LG_ACTION_TYPE::LG_STUN_ACTION, target, 100, hit_color));
	}
	return true;
}
/// ---------------------------------------------

/// Die_Action ----------------------------------
// Constructors =======================
Die_Action::Die_Action() :Action(LG_DIE_ACTION)
{

}

// Destructors ========================
Die_Action::~Die_Action()
{

}

// Game Loop ==========================
bool Die_Action::Execute()
{
	//Drop the creature money
	if (((Creature*)actor)->GetMoney() > 0)((Creature*)actor)->DropMoney();
	//Drop the creature experience (if is not the player)
	if (actor != App->player->avatar)
	{
		((Creature*)actor)->DropExperience();
		if (App->GetCurrentScene() == (Scene*)App->endless)
		{
			App->endless->CreaturesCount(1);
		}
	}

	//Delete the entity
	App->entities_manager->ClearEntity(actor);

	//Player callback to end the current party
	if (((Creature*)actor)->GetCreatureType() == PLAYER_CREATURE)
	{
		App->player->PlayerDeath();
	}

	return true;
}
/// ---------------------------------------------

/// Stun_Action ---------------------------------
// Constructors =================================
Stun_Action::Stun_Action() :Action(LG_STUN_ACTION)
{

}

// Destructors ==================================
Stun_Action::~Stun_Action()
{

}

// Game Loop ====================================
bool Stun_Action::Init()
{
	stun_timer.Start();
	return true;
}

bool Stun_Action::Execute()
{
	return stun_timer.Read() > time;
}
/// ---------------------------------------------

/// Magic_Spawn_Action --------------------------
// Constructors =================================
Magic_Spawn_Action::Magic_Spawn_Action():Action(LG_MAGIC_SPAWN_ACTION)
{

}

// Destructors ==================================
Magic_Spawn_Action::~Magic_Spawn_Action()
{

}

// Game Loop ====================================
bool Magic_Spawn_Action::Init()
{
	//Get the previous fixture 
	prev_collision_type = actor->GetBody()->collision_type;

	//Change the current entity fixture to a ghost one
	b2Fixture* prev_fixture = actor->GetBody()->body->GetFixtureList();

	//Modify the fixture to ghost mode
	b2FixtureDef temp_fixture;
	temp_fixture.shape = prev_fixture->GetShape();
	temp_fixture.density = prev_fixture->GetDensity();
	temp_fixture.restitution = prev_fixture->GetRestitution();
	temp_fixture.friction = prev_fixture->GetFriction();
	temp_fixture.isSensor = false;
	temp_fixture.userData = prev_fixture->GetUserData();
	App->physics->SetFixture(temp_fixture, GHOST_COLLISION);
	actor->GetBody()->body->CreateFixture(&temp_fixture);
	actor->GetBody()->body->DestroyFixture(prev_fixture);

	//Get final scale checking current animation scale
	final_scale = actor->GetAnimation()->GetSpritesScale();
	
	//Set sprite angle
	actor->GetAnimation()->SetSpritesAngle(0.0);

	//Set sprite scale
	actor->GetAnimation()->SetSpritesScale(initial_scale);

	//Start timer
	spawn_timer.Start();

	return true;
}

bool Magic_Spawn_Action::Execute()
{
	per_cent = (float)spawn_timer.Read()/(float)total_time;
	
	//Update sprite angle
	current_angle = 360 * (float)flips * per_cent;
	actor->GetAnimation()->SetSpritesAngle(current_angle);

	//Update sprite scale
	current_scale = final_scale * per_cent;
	actor->GetAnimation()->SetSpritesScale(current_scale);

	bool ret = per_cent >= 1.00;

	actor->GetBody()->body->SetLinearVelocity(b2Vec2(0, 0));

	if (ret)
	{
		//Reset sprites angle 
		actor->GetAnimation()->SetSpritesAngle(0.0);
		//Reset sprites scale
		actor->GetAnimation()->SetSpritesScale(final_scale);

		//Change the current ghost fixture the original
		b2Fixture* prev_fixture = actor->GetBody()->body->GetFixtureList();

		//Reactivate collision modifying the fixture
		b2FixtureDef temp_fixture;
		temp_fixture.shape = prev_fixture->GetShape();
		temp_fixture.density = prev_fixture->GetDensity();
		temp_fixture.restitution = prev_fixture->GetRestitution();
		temp_fixture.friction = prev_fixture->GetFriction();
		temp_fixture.isSensor = false;
		temp_fixture.userData = prev_fixture->GetUserData();
		App->physics->SetFixture(temp_fixture, prev_collision_type);
		actor->GetBody()->body->CreateFixture(&temp_fixture);
		actor->GetBody()->body->DestroyFixture(prev_fixture);
	}

	return ret;
}

// Functionality ================================

// Set Methods ==================================

// Get Methods ==================================


