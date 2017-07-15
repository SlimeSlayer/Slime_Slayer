#ifndef _WORKER_H_
#define _WORKER_H_

#include "p2Defs.h"
#include "Actions.h"
#include <queue>

/// Worker --------------------------------------
class Worker
{
public:

	Worker();
	~Worker();

public:

	//Game Loop -------------
	void Update();

private:

	std::priority_queue<Action*>	actions;
	Action*							current_action = nullptr;

public:

	//Get Methods -----------
	Action*	GetCurrentAction()const;

	//Actions Factory -------
	Dialog_Action*			AddDialogAction(Entity* actor, Player* target, uint priority = 0);
	Spawn_Delay_Action*		AddSpawnDelayAction(Entity* actor, uint delay = 500);
	Basic_Attack_Action*	AddBasicAttackAction(Entity* actor);
	Move_Action*			AddMoveAction(Entity* actor, const iPoint& destination);

};
/// ---------------------------------------------
#endif // !_WORKER_H_
