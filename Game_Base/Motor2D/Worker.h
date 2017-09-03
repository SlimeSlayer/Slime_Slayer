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
	Action*			GetCurrentAction()const;
	LG_ACTION_TYPE	GetCurrentActionType()const;

	//Actions Factory -------
	/*
	- LG_DIALOG_ACTION: Entity* actor, Player* target
	- LG_SPAWN_DELAY_ACTION: Entity* actor, uint delay
	- LG_MOVE_ACTION: Entity* actor, iPoint destination
	- LG_MOVE_TO_TARGET_ACTION: Entity* actor, Creature* target
	- LG_BASIC_ATTACK_ACTION: Entity* actor, Creature* target
	- LG_SIMPLE_ATTACK_ACTION: Entity* actor, Creature* target
	- LG_DIE_ACTION: Entity* actor
	- LG_STUN_ACTION: Entity* actor, uint time, SDL_Color blit_color
	*/
	Action*	GenerateAction(LG_ACTION_TYPE type, ...);
	
	//Actions Management ----
	void	AddAction(const Action* new_act);
	void	AddPriorizedAction(const Action* new_act);

};
/// ---------------------------------------------
#endif // !_WORKER_H_
