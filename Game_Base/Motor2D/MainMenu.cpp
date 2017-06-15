#include "MainMenu.h"

#include "j1App.h"
#include "j1Gui.h"
#include "j1InputManager.h"
#include "j1Audio.h"

#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"

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

void MainMenu::Active()
{
	active = true;
	App->audio->PlayMusic(MUSIC_ID::MUSIC_MENU);
}

bool MainMenu::Enable()
{
	active = true;
	enabled = true;
	menu_branch->Activate();
	menu_branch->ActivateChilds();
	settings_menu->Desactivate();
	settings_menu->DesactivateChids();

	return true;
}

void MainMenu::Disable()
{
	active = false;
	enabled = false;
	menu_branch->Desactivate();
	menu_branch->DesactivateChids();
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
	start_button->SetTexOVER({ 0,393,564,165 }, ATLAS_TEST);
	start_button->SetTexON({ 0,393,564,165 }, ATLAS);
	start_button->SetInputTarget(this);
	start_button->SetParent(menu_branch);
	start_button->Activate();
	menu_branch->AddChild(start_button);

	//Settings Button
	settings_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_button->SetBox({ 500,600,570,170 });
	settings_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	settings_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	settings_button->SetTexON({ 0,598,564,165 }, ATLAS);
	settings_button->SetInputTarget(this);
	settings_button->SetParent(menu_branch);
	settings_button->Activate();
	menu_branch->AddChild(settings_button);

	//Quit Button
	quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	quit_button->SetBox({ 500,800,570,170 });
	quit_button->SetTexOFF({ 0,199,564,165 }, ATLAS_TEST);
	quit_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	quit_button->SetTexON({ 0,598,564,165 }, ATLAS_TEST);
	quit_button->SetInputTarget(this);
	quit_button->SetParent(menu_branch);
	quit_button->Activate();
	menu_branch->AddChild(quit_button);

	//Build Setting Menu ----
	
	//Settings menu base
	settings_menu = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	settings_menu->SetInputTarget(this);
	settings_menu->SetParent(settings_button);
	settings_menu->SetBox({ 100,100,500,700 });
	settings_menu->ChangeTextureId(ATLAS);
	settings_menu->ChangeTextureRect({ 100,100,500,700 });
	settings_menu->Desactivate();
	settings_menu->DesactivateChids();
	menu_branch->AddChild(settings_menu);

	//Quit button
	settings_quit = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_quit->SetInputTarget(this);
	settings_quit->SetParent(settings_menu);
	settings_quit->SetBox({ 450,20,130,95 });
	settings_quit->SetTexOFF({ 0,766,126,93 }, ATLAS);
	settings_quit->SetTexON({ 0,266,126,93 }, ATLAS);
	settings_quit->SetTexOVER({ 0,266,126,93 }, ATLAS);
	settings_quit->Desactivate();
	settings_menu->AddChild(settings_quit);

	//Add the built branch at the GUI 
	App->gui->PushScreen(menu_branch);

	App->audio->PlayMusic(MUSIC_ID::MUSIC_MENU);

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
		if (settings_menu->GetActiveState())
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			//Activate menu buttons
			start_button->Activate();
			settings_button->Activate();
		}
		else App->SetQuit();
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
	if (input == GUI_INPUT::MOUSE_LEFT_BUTTON_DOWN)
	{
		//Main Buttons ----------------
		if(target == start_button)App->ActiveScene();
		else if (target == settings_button)
		{
			//Deactivate start, settings & quit buttons
			start_button->Desactivate();
			settings_button->Desactivate();
			quit_button->Desactivate();

			//Activate settings menu and all the childs
			settings_menu->Activate();
			settings_menu->ActivateChilds();
		}
		else if (target == quit_button)
		{
			App->SetQuit();
		}

		//Settings Buttons ------------
		else if (target == settings_quit)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			//Activate menu buttons
			start_button->Activate();
			settings_button->Activate();
			quit_button->Activate();
		}
	}
}
