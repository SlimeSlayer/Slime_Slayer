#include "MainMenu.h"

#include "j1App.h"
#include "j1Gui.h"
#include "j1InputManager.h"
#include "j1Audio.h"

#include "UI_Element.h"
#include "UI_Button.h"


// Constructors ===========================================
MainMenu::MainMenu()
{
	name = "menu";
}

// Destructors ============================================
MainMenu::~MainMenu()
{

}

// Game Loop ==============================================
void MainMenu::Init()
{
	active = true;
	enabled = true;
}

bool MainMenu::Start()
{
	//Build menu UI ---------

	// Main Menu Branch
	menu_branch = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	menu_branch->SetInputTarget(this);
	menu_branch->Activate();

	// Start Button
	start_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	start_button->SetBox({ 500,300,570,170 });
	start_button->SetTexOFF({ 0,0,564,165 }, ATLAS);
	start_button->SetTexOVER({ 0,393,564,165 }, ATLAS);
	start_button->SetTexON({ 0,393,564,165 }, ATLAS);
	start_button->SetInputTarget(this);
	start_button->SetParent(menu_branch);
	start_button->Activate();
	menu_branch->AddChild(start_button);

	//Settings Button

	//Add the built branch at the GUI 
	App->gui->PushScreen(menu_branch);

	return true;
}

bool MainMenu::Update(float dt)
{
	App->gui->CalculateUpperElement(menu_branch);

	//VOLUME ------------------------------------
	if (App->input_manager->GetEvent(VOLUME_UP) == INPUT_DOWN)
	{
		App->audio->VolumeUp();
	}
	else if (App->input_manager->GetEvent(VOLUME_DOWN) == INPUT_DOWN)
	{
		App->audio->VolumeDown();
	}
	// ------------------------------------------

	//EXIT --------------------------------------
	else if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN)
	{
		App->SetQuit();
	}
	// ------------------------------------------

	return true;
}

bool MainMenu::CleanUp()
{
	return true;
}

// GUI Functions ==========================================
void MainMenu::GUI_Input(UI_Element * target, GUI_INPUT input)
{
	if (input == GUI_INPUT::MOUSE_LEFT_BUTTON_DOWN && target == start_button)
	{
		App->ActiveScene();
	}
}
