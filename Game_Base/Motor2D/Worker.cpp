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
	if (current_action != nullptr)
	{
		if (current_action->Execute())
		{
			RELEASE(current_action);
		}
	}
	else if (!actions.empty())
	{
		current_action = actions.top();
		actions.pop();
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
/// ---------------------------------------------
