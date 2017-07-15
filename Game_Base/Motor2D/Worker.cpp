#include "Worker.h"

/// Worker --------------------------------------
// Constructors =======================
Worker::Worker()
{

}

// Destructors ========================
Worker::~Worker()
{
	uint size = actions.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE((Action*)actions.top());
		actions.pop();
	}
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
		current_action = actions.top();
		actions.pop();

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

Dialog_Action* Worker::AddDialogAction(Entity * actor, Player * target, uint priority)
{
	//Generate dialog action
	Dialog_Action* new_act = new Dialog_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->target = target;
	new_act->priority = priority;
	//Push the new action at the queue
	actions.emplace(new_act);
	//Return the generated action
	return new_act;
}

Spawn_Delay_Action * Worker::AddSpawnDelayAction(Entity * actor, uint delay)
{
	//Generate spawn delay action
	Spawn_Delay_Action* new_act = new Spawn_Delay_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->delay = delay;
	//Push the new action at the queue
	actions.emplace(new_act);
	//Return the generated action
	return new_act;
}

Basic_Attack_Action * Worker::AddBasicAttackAction(Entity * actor, Entity* target)
{
	//Generate basic attack action
	Basic_Attack_Action* new_act = new Basic_Attack_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->target = target;
	//Push the new action at the queue
	actions.emplace(new_act);
	//Return the generated action
	return new_act;
}

Move_Action * Worker::AddMoveAction(Entity * actor, const iPoint& destination)
{
	//Generate move action
	Move_Action* new_act = new Move_Action(destination);
	//Set action stats
	new_act->actor = actor;
	//Push the new action at the queue
	actions.emplace(new_act);
	//Return the generated action
	return new_act;
}
Move_To_Target_Action * Worker::AddMoveToTargetAction(Entity * actor, Entity * target)
{
	//Generate move to target action
	Move_To_Target_Action* new_act = new Move_To_Target_Action();
	//Set action stats
	new_act->actor = actor;
	new_act->target = (Creature*)target;
	//Push the new action at the queue
	actions.emplace(new_act);
	//Return the generated action
	return new_act;
}
/// ---------------------------------------------
