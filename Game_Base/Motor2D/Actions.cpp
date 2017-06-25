#include "Actions.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1Gui.h"
#include "j1Player.h"
#include "j1InputManager.h"

#include "UI_Element.h"
#include "UI_String.h"

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
	if (current_dialog == nullptr)
	{
		current_dialog = ((NPC*)actor)->GetDialogByIndex(0);
		current_dialog->SetBoxPosition(DEFAULT_DIALOG_X - current_dialog->GetBox()->w, DEFAULT_DIALOG_Y - current_dialog->GetBox()->h);
	}
	else if (App->input_manager->GetEvent(INPUT_EVENT::CROUCH) == INPUT_DOWN)
	{
		current_dialog = ((NPC*)actor)->GetDialogByIndex(++dialog_index);
		if (current_dialog == nullptr)return true;
		else current_dialog->SetBoxPosition(DEFAULT_DIALOG_X - current_dialog->GetBox()->w, DEFAULT_DIALOG_Y - current_dialog->GetBox()->h);
	}
	current_dialog->Draw(false);

	LOG("EXECUTED!");
	return false;
}
/// ---------------------------------------------