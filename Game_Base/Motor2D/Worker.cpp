#include "Worker.h"

/// Worker --------------------------------------
// Constructors =======================
Worker::Worker()
{

}

// Destructors ========================
Worker::~Worker()
{
	std::list<Action*>::const_iterator act = actions.begin();
	while (act != actions.end())
	{
		RELEASE(act._Ptr->_Myval);
		act++;
	}
	actions.clear();

	if (current_action != nullptr)RELEASE(current_action);
}

// Game Loop ==========================
void Worker::Update()
{
	// Execute the current action
	if (current_action != nullptr)
	{
		if (current_action->Execute()) RELEASE(current_action);
	}

	// Start a new action
	else if (!actions.empty())
	{
		current_action = actions.front();
		actions.pop_front();

		// Check if the new action is started correctly 
		if (current_action != nullptr && !current_action->Init())
		{
			RELEASE(current_action);
		}
	}
}

Action * Worker::GetCurrentAction() const
{
	return current_action;
}

// Action Factory =====================
Dialog_Action* Worker::GenerateDialogAction(Entity * actor, Player * target, uint priority)
{
	//Generate dialog action
	Dialog_Action* new_act = new Dialog_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->target = target;
	new_act->priority = priority;
	//Return the generated action
	return new_act;
}

Spawn_Delay_Action * Worker::GenerateSpawnDelayAction(Entity * actor, uint delay)
{
	//Generate spawn delay action
	Spawn_Delay_Action* new_act = new Spawn_Delay_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->delay = delay;
	//Return the generated action
	return new_act;
}

Basic_Attack_Action * Worker::GenerateBasicAttackAction(Entity * actor, Creature* target)
{
	//Generate basic attack action
	Basic_Attack_Action* new_act = new Basic_Attack_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->target = target;
	//Return the generated action
	return new_act;
}

Simple_Attack_Action * Worker::GenerateSimpleAttackAction(Entity * actor, Creature * target)
{
	//Generate simple attack action
	Simple_Attack_Action* new_act = new Simple_Attack_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->target = target;
	//Return the generated action
	return new_act;
}

Move_Action * Worker::GenerateMoveAction(Entity * actor, const iPoint& destination)
{
	//Generate move action
	Move_Action* new_act = new Move_Action(destination);
	//Set action stats
	new_act->actor = actor;
	//Return the generated action
	return new_act;
}
Move_To_Target_Action * Worker::GenerateMoveToTargetAction(Entity * actor, Entity * target)
{
	//Generate move to target action
	Move_To_Target_Action* new_act = new Move_To_Target_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->target = (Creature*)target;
	//Return the generated action
	return new_act;
}

Die_Action * Worker::GenerateDieAction(Entity * actor)
{
	//Generate die action
	Die_Action* new_act = new Die_Action();
	//Set action stats
	new_act->actor = actor;
	//Return the generated action
	return new_act;
}

// Actions Management =================
void Worker::AddAction(const Action * new_act)
{
	actions.push_back((Action*)new_act);
}

void Worker::AddPriorizedAction(const Action * new_act)
{
	if (current_action != nullptr)
	{
		actions.push_front(current_action);
	}
	current_action = (Action*)new_act;
}
/// ---------------------------------------------
