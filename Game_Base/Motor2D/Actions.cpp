#include "Actions.h"

#include "p2Log.h"

/// Action --------------------------------------
// Constructors =======================
Action::Action()
{

}

// Destructors ========================
Action::~Action()
{

}

// Game Loop ==========================
bool Action::Execute()
{
	return true;
}
/// ---------------------------------------------

/// Dialog_Action -------------------------------
// Constructors =======================
Dialog_Action::Dialog_Action()
{

}

// Destructors ========================
Dialog_Action::~Dialog_Action()
{

}

// Game Loop ==========================
bool Dialog_Action::Execute()
{
	LOG("EXECUTED!");
	return true;
}
/// ---------------------------------------------