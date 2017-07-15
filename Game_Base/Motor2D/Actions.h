#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#define ACTIONS_UPDATE_RATE 250
#define LOCATION_ERROR_MARGIN 5 /*In Pixels*/

#include "p2Defs.h"
#include "p2Point.h"
#include "j1Timer.h"
#include "j1Physics.h"

class Entity;
class Creature;
class Player;
class UI_String;
class b2Fixture;

enum ACTION_TYPE
{
	NO_ACTION = 0,
	DIALOG_ACTION,
	SPAWN_DELAY_ACTION,
	MOVE_ACTION,
	BASIC_ATTACK_ACTION
};

/// Action --------------------------------------
class Action
{
public:

	Action(ACTION_TYPE action_type = NO_ACTION);
	~Action();

public:

	//Game Loop -------------
	virtual bool Init();
	virtual bool Execute();

public:

	ACTION_TYPE action_type = NO_ACTION;
	Entity*		actor = nullptr;
	uint		priority = 0;

public:

	//Methods

};
/// ---------------------------------------------

/// Dialog_Action -------------------------------
class Dialog_Action : public Action
{
public:

	Dialog_Action();
	~Dialog_Action();

public:

	//Game Loop -------------
	bool Init();
	bool Execute();

public:

	Player* target = nullptr;
	
	UI_String*	current_dialog = nullptr;
	uint		dialog_index = 0;

};
/// ---------------------------------------------

/// Spawn_Delay_Action --------------------------
class Spawn_Delay_Action : public Action
{
public:

	Spawn_Delay_Action();
	~Spawn_Delay_Action();

public:

	//Game Loop -------------
	bool Init();
	bool Execute();

public:

	COLLISION_TYPE prev_collision_type = COLLISION_TYPE::GHOST_COLLISION;

	j1Timer delay_timer;
	uint	delay = 0;
};
/// ---------------------------------------------

/// Move_Action ---------------------------------
class Move_Action : public Action
{
public:

	Move_Action(const iPoint& destination);
	~Move_Action();

public:

	//Game Loop -------------
	bool Init();
	bool Execute();

public:
	
	iPoint	destination = { 0,0 };
	int		mov_direction = 0;
};
/// ---------------------------------------------

/// Move_To_Target_Action -----------------------
class Move_To_Target_Action : public Action
{
public:

	Move_To_Target_Action();
	~Move_To_Target_Action();

public:

	//Game Loop -------------
	bool Init();
	bool Execute();

public:

	Creature*	target = nullptr;
	j1Timer		update_timer;
	iPoint		current_goal = { 0,0 };
	int			mov_direction = 0;
};
/// ---------------------------------------------

/// Basic_Attack_Action -------------------------
class Basic_Attack_Action : public Action
{
public:
	
	Basic_Attack_Action();
	~Basic_Attack_Action();

public:

	//Game Loop -------------
	bool Init();
	bool Execute();

public:

	Creature*	target = nullptr;
	j1Timer		attack_timer;

};
/// ---------------------------------------------
#endif // !_ACTIONS_H_
