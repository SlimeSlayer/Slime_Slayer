#include "Actions.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1Gui.h"
#include "j1Player.h"
#include "j1InputManager.h"
#include "j1Physics.h"

#include "UI_Element.h"
#include "UI_String.h"

/// Action --------------------------------------
// Constructors =======================
Action::Action(ACTION_TYPE action_type): action_type(action_type)
{

}

// Destructors ========================
Action::~Action()
{

}

// Game Loop ==========================
bool Action::Init()
{
	return true;
}

bool Action::Execute()
{
	return true;
}
/// ---------------------------------------------

/// Dialog_Action -------------------------------
// Constructors =======================
Dialog_Action::Dialog_Action(): Action(DIALOG_ACTION)
{

}

// Destructors ========================
Dialog_Action::~Dialog_Action()
{

}

// Game Loop ==========================
bool Dialog_Action::Init()
{
	LOG("DIALOG_");

	//Stop target body when dialog starts & block input
	target->GetBody()->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	target->LockInput();

	return true;
}

bool Dialog_Action::Execute()
{
	//Get the first dialog string
	if (current_dialog == nullptr)
	{
		current_dialog = ((NPC*)actor)->GetDialogByIndex(0);
		current_dialog->SetBoxPosition(DEFAULT_DIALOG_X - current_dialog->GetBox()->w, DEFAULT_DIALOG_Y - current_dialog->GetBox()->h);
	}

	//Focus the next dialog string
	else if (App->input_manager->GetEvent(INPUT_EVENT::INTERACT) == INPUT_DOWN)
	{
		//Get the next dialog phrase
		current_dialog = ((NPC*)actor)->GetDialogByIndex(++dialog_index);
		
		//When dialog ends player input is unlocked
		if (current_dialog == nullptr)
		{
			LOG("DIALOG_ENDED_");
			target->UnlockInput();
			return true;
		}

		//Set the correct dialog box position
		else current_dialog->SetBoxPosition(DEFAULT_DIALOG_X - current_dialog->GetBox()->w, DEFAULT_DIALOG_Y - current_dialog->GetBox()->h);
	}

	//Draw the current string
	current_dialog->Draw(false);

	return false;
}
/// ---------------------------------------------