#ifndef _WORKER_H_
#define _WORKER_H_

#include "p2Defs.h"
#include "Actions.h"

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

	std::list<Action*>				actions;
	Action*							current_action = nullptr;

public:

	//Get Methods -----------
	Action*	GetCurrentAction()const;

	//Actions Factory -------
	Dialog_Action*			GenerateDialogAction(Entity* actor, Player* target, uint priority = 0);
	Spawn_Delay_Action*		GenerateSpawnDelayAction(Entity* actor, uint delay = 500);
	Basic_Attack_Action*	GenerateBasicAttackAction(Entity* actor, Creature* target);
	Move_Action*			GenerateMoveAction(Entity* actor, const iPoint& destination);
	Move_To_Target_Action*	GenerateMoveToTargetAction(Entity* actor, Entity* target);

	//Actions Management ----
	void	AddAction(const Action* new_act);
	void	AddPriorizedAction(const Action* new_act);

};
/// ---------------------------------------------
#endif // !_WORKER_H_
