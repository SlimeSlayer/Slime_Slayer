#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "p2Defs.h"

class Entity;
class Player;
class UI_String;

enum ACTION_TYPE
{
	NO_ACTION = 0,
	DIALOG_ACTION
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
#endif // !_ACTIONS_H_
