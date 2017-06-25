#ifndef _ACTIONS_H_
#define _ACTIONS_H_

#include "p2Defs.h"

class Entity;
class Player;
class UI_String;

enum ACTION_TYPE
{
	NO_ACTION = 0,
	DIALOG,
};

/// Action --------------------------------------
class Action
{
public:

	Action();
	~Action();

public:

	//Game Loop -------------
	virtual bool Execute();

public:

	Entity* actor = nullptr;
	uint	priority = 0;

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
	bool Execute();

public:

	Player* target = nullptr;
	
	UI_String*	current_dialog = nullptr;
	uint		dialog_index = 0;

};
/// ---------------------------------------------
#endif // !_ACTIONS_H_
