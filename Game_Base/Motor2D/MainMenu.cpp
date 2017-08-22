#include "MainMenu.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Gui.h"
#include "j1InputManager.h"
#include "j1Audio.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1Render.h"

#include "Endless.h"
#include "Tutorial.h"

#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Scroll_Bar.h"
#include "UI_String.h"
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
	active = false;
	enabled = true;
}

void MainMenu::Active()
{
	active = true;
}

bool MainMenu::Enable()
{

	menu_branch->Activate();
	menu_branch->ActivateChilds();
	settings_menu->Desactivate();
	settings_menu->DesactivateChids();
	audio_menu->Desactivate();
	audio_menu->DesactivateChids();
	video_menu->Desactivate();
	video_menu->DesactivateChids();
	quit_menu_base->Desactivate();
	quit_menu_base->DesactivateChids();

	if (App->tutorial->TutorialCompleted())
	{
		start_button->Desactivate();

		// Main Menu Links ------
		continue_button->SetNextInFocus(tutorial_button);
		continue_button->SetPrevInFocus(quit_button);
		tutorial_button->SetNextInFocus(settings_button);
		tutorial_button->SetPrevInFocus(continue_button);
		settings_button->SetNextInFocus(quit_button);
		settings_button->SetPrevInFocus(tutorial_button);
		quit_button->SetNextInFocus(continue_button);
		quit_button->SetPrevInFocus(settings_button);
		// ---------------------
	}
	else
	{
		continue_button->Desactivate();
		tutorial_button->Desactivate();

		// Main Menu Links ------
		start_button->SetNextInFocus(settings_button);
		start_button->SetPrevInFocus(quit_button);
		settings_button->SetNextInFocus(quit_button);
		settings_button->SetPrevInFocus(start_button);
		quit_button->SetNextInFocus(start_button);
		quit_button->SetPrevInFocus(settings_button);
		// ---------------------
	}

	App->audio->PlayMusic(MUSIC_ID::MUSIC_MENU);

	active = false;
	enabled = true;

	return true;
}

void MainMenu::Disable()
{
	active = false;
	enabled = false;
	start_button->UnBlock();
	continue_button->UnBlock();
	tutorial_button->UnBlock();
	menu_branch->Desactivate();
	menu_branch->DesactivateChids();
}

bool MainMenu::Start()
{
	//Build menu UI ---------
	Font_Def* menu_ui_font = App->font->GetFontByID(FONT_ID::MENU_UI_FONT);

	// Main Menu Branch -------------------------
	menu_branch = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	menu_branch->SetInputTarget(this);
	menu_branch->Activate();

	// Start Button
	start_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	start_button->SetBox({ 515,100,570,170 });
	start_button->SetTexOFF({ 0,0,564,165 }, ATLAS);
	start_button->SetTexOVER({ 0,393,564,165 }, ATLAS_TEST);
	start_button->SetTexON({ 0,393,564,165 }, ATLAS);
	start_button->SetInputTarget(this);
	start_button->SetParent(menu_branch);
	start_button->Activate();
	menu_branch->AddChild(start_button);

	// Start Button String
	UI_String* start_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	start_button_string->SetInputTarget(this);
	start_button_string->SetFont(menu_ui_font->font);
	start_button_string->SetColor(menu_ui_font->font_color);
	start_button_string->SetString("Start");
	start_button_string->AdjustBox();
	start_button_string->SetBoxPosition(start_button->GetBox()->w * 0.5f - start_button_string->GetBox()->w * 0.5f, start_button->GetBox()->h * 0.5f - start_button_string->GetBox()->h * 0.5f);
	start_button->AddChild(start_button_string);
	start_button_string->SetLogicalLayer(0);

	//Continue Button
	continue_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	continue_button->SetBox({ 515,100,570,170 });
	continue_button->SetTexOFF({ 0,0,564,165 }, ATLAS);
	continue_button->SetTexOVER({ 0,393,564,165 }, ATLAS_TEST);
	continue_button->SetTexON({ 0,393,564,165 }, ATLAS);
	continue_button->SetInputTarget(this);
	continue_button->SetParent(menu_branch);
	continue_button->Activate();
	menu_branch->AddChild(continue_button);

	//Continue Button String
	UI_String* continue_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	continue_button_string->SetInputTarget(this);
	continue_button_string->SetFont(menu_ui_font->font);
	continue_button_string->SetColor(menu_ui_font->font_color);
	continue_button_string->SetString("Continue");
	continue_button_string->AdjustBox();
	continue_button_string->SetBoxPosition(continue_button->GetBox()->w * 0.5f - continue_button_string->GetBox()->w * 0.5f, continue_button->GetBox()->h * 0.5f - continue_button_string->GetBox()->h * 0.5f);
	continue_button->AddChild(continue_button_string);
	continue_button_string->SetLogicalLayer(0);

	//Tutorial Button
	tutorial_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	tutorial_button->SetBox({ 515,300,570,170 });
	tutorial_button->SetTexOFF({ 0,0,564,165 }, ATLAS);
	tutorial_button->SetTexOVER({ 0,393,564,165 }, ATLAS_TEST);
	tutorial_button->SetTexON({ 0,393,564,165 }, ATLAS);
	tutorial_button->SetInputTarget(this);
	tutorial_button->SetParent(menu_branch);
	tutorial_button->Activate();
	menu_branch->AddChild(tutorial_button);

	//Tutorial Button String
	UI_String* tutorial_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	tutorial_button_string->SetInputTarget(this);
	tutorial_button_string->SetFont(menu_ui_font->font);
	tutorial_button_string->SetColor(menu_ui_font->font_color);
	tutorial_button_string->SetString("Tutorial");
	tutorial_button_string->AdjustBox();
	tutorial_button_string->SetBoxPosition(tutorial_button->GetBox()->w * 0.5f - tutorial_button_string->GetBox()->w * 0.5f, tutorial_button->GetBox()->h * 0.5f - tutorial_button_string->GetBox()->h * 0.5f);
	tutorial_button->AddChild(tutorial_button_string);
	tutorial_button_string->SetLogicalLayer(0);

	//Settings Button
	settings_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_button->SetBox({ 515,500,570,170 });
	settings_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	settings_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	settings_button->SetTexON({ 0,598,564,165 }, ATLAS);
	settings_button->SetInputTarget(this);
	settings_button->SetParent(menu_branch);
	settings_button->Activate();
	menu_branch->AddChild(settings_button);

	// Settings Button String
	UI_String* settings_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	settings_button_string->SetInputTarget(this);
	settings_button_string->SetFont(menu_ui_font->font);
	settings_button_string->SetColor(menu_ui_font->font_color);
	settings_button_string->SetString("Settings");
	settings_button_string->AdjustBox();
	settings_button_string->SetBoxPosition(settings_button->GetBox()->w * 0.5f - settings_button_string->GetBox()->w * 0.5f, settings_button->GetBox()->h * 0.5f - settings_button_string->GetBox()->h * 0.5f);
	settings_button->AddChild(settings_button_string);
	settings_button_string->SetLogicalLayer(0);

	//Quit Button
	quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	quit_button->SetBox({ 515,700,570,170 });
	quit_button->SetTexOFF({ 0,199,564,165 }, ATLAS_TEST);
	quit_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	quit_button->SetTexON({ 0,598,564,165 }, ATLAS_TEST);
	quit_button->SetInputTarget(this);
	quit_button->SetParent(menu_branch);
	quit_button->Activate();
	menu_branch->AddChild(quit_button);

	// Quit Button String
	UI_String* quit_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	quit_button_string->SetInputTarget(this);
	quit_button_string->SetFont(menu_ui_font->font);
	quit_button_string->SetColor(menu_ui_font->font_color);
	quit_button_string->SetString("Quit");
	quit_button_string->AdjustBox();
	quit_button_string->SetBoxPosition(quit_button->GetBox()->w * 0.5f - quit_button_string->GetBox()->w * 0.5f, quit_button->GetBox()->h * 0.5f - quit_button_string->GetBox()->h * 0.5f);
	quit_button->AddChild(quit_button_string);
	quit_button_string->SetLogicalLayer(0);

	// Github Link
	github_link = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	github_link->SetInputTarget(this);
	github_link->ChangeTextureId(TEXTURE_ID::GITHUB_ICON);
	github_link->AdjustBox();
	github_link->SetBoxPosition(1435, 750);
	menu_branch->AddChild(github_link);


	if (App->tutorial->TutorialCompleted())
	{
		start_button->Desactivate();
		start_button->DesactivateChids();

		// Main Menu Links ------
		continue_button->SetNextInFocus(tutorial_button);
		continue_button->SetPrevInFocus(quit_button);
		tutorial_button->SetNextInFocus(settings_button);
		tutorial_button->SetPrevInFocus(continue_button);
		settings_button->SetNextInFocus(quit_button);
		settings_button->SetPrevInFocus(tutorial_button);
		quit_button->SetNextInFocus(continue_button);
		quit_button->SetPrevInFocus(settings_button);
		// ---------------------
	}
	else
	{
		tutorial_button->Desactivate();
		tutorial_button->DesactivateChids();
		continue_button->Desactivate();
		continue_button->DesactivateChids();

		// Main Menu Links ------
		start_button->SetNextInFocus(settings_button);
		start_button->SetPrevInFocus(quit_button);
		settings_button->SetNextInFocus(quit_button);
		settings_button->SetPrevInFocus(start_button);
		quit_button->SetNextInFocus(start_button);
		quit_button->SetPrevInFocus(settings_button);
		// ---------------------
	}
	// ------------------------------------------

	//Build	Quit Menu ---------------------------
	//Base
	quit_menu_base = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	quit_menu_base->ChangeTextureId(TEXTURE_ID::ATLAS_TEST);
	quit_menu_base->ChangeTextureRect({0, 0, 100, 50});
	quit_menu_base->SetTextureScale(8.0f);
	quit_menu_base->AdjustBox();
	quit_menu_base->SetBoxPosition(400, 260);
	quit_menu_base->SetInputTarget(this);
	menu_branch->AddChild(quit_menu_base);
	quit_menu_base->SetVisualLayer(10);

	//Base string
	quit_menu_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	Font_Def* str_font = App->font->GetFontByID(FONT_ID::MENU_DIALOG_FONT);
	quit_menu_string->SetFont(str_font->font);
	quit_menu_string->SetColor(str_font->font_color);
	quit_menu_string->SetString("Do you want to quit?");
	quit_menu_string->AdjustBox();
	quit_menu_string->SetBoxPosition(quit_menu_base->GetBox()->w * 0.5f - quit_menu_string->GetBox()->w * 0.5f, 60);
	quit_menu_string->SetInputTarget(this);
	quit_menu_base->AddChild(quit_menu_string);
	
	//Accept button
	quit_menu_yes_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	quit_menu_yes_button->SetTexOFF({ 0,776,90,40 }, ATLAS);
	quit_menu_yes_button->SetTexON({ 0,266,90,40 }, ATLAS);
	quit_menu_yes_button->SetTexOVER({ 0,266,90,40 }, ATLAS);
	quit_menu_yes_button->SetInputTarget(this);
	quit_menu_yes_button->SetLogicalLayer(10);
	quit_menu_yes_button->SetTextureScale(1.5f);
	quit_menu_yes_button->AdjustBox();
	quit_menu_yes_button->SetBoxPosition(150, 260);
	quit_menu_base->AddChild(quit_menu_yes_button);

	//Accept button str
	UI_String* yes_btn_str = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	yes_btn_str->SetFont(str_font->font);
	yes_btn_str->SetColor(str_font->font_color);
	yes_btn_str->SetString("Accept");
	yes_btn_str->AdjustBox();
	yes_btn_str->SetBoxPosition(quit_menu_yes_button->GetBox()->w * 0.5f - yes_btn_str->GetBox()->w * 0.5f, quit_menu_yes_button->GetBox()->h * 0.5f - yes_btn_str->GetBox()->h * 0.5f);
	yes_btn_str->SetInputTarget(this);
	quit_menu_yes_button->AddChild(yes_btn_str);
	yes_btn_str->SetLogicalLayer(0);

	//Decline button
	quit_menu_no_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	quit_menu_no_button->SetTexOFF({ 0,776,90,40 }, ATLAS);
	quit_menu_no_button->SetTexON({ 0,266,90,40 }, ATLAS);
	quit_menu_no_button->SetTexOVER({ 0,266,90,40 }, ATLAS);
	quit_menu_no_button->SetTextureScale(1.5f);
	quit_menu_no_button->AdjustBox();
	quit_menu_no_button->SetBoxPosition(550, 260);
	quit_menu_no_button->SetInputTarget(this);
	quit_menu_no_button->SetLogicalLayer(10);
	quit_menu_base->AddChild(quit_menu_no_button);
	
	//Decline button str
	UI_String* no_btn_str = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	no_btn_str->SetFont(str_font->font);
	no_btn_str->SetColor(str_font->font_color);
	no_btn_str->SetString("Decline");
	no_btn_str->AdjustBox();
	no_btn_str->SetBoxPosition(quit_menu_no_button->GetBox()->w * 0.5f - no_btn_str->GetBox()->w * 0.5f, quit_menu_no_button->GetBox()->h * 0.5f - no_btn_str->GetBox()->h * 0.5f);
	no_btn_str->SetInputTarget(this);
	quit_menu_no_button->AddChild(no_btn_str);
	no_btn_str->SetLogicalLayer(0);
	
	// Quit Menu Links ------
	quit_menu_no_button->SetNextInFocus(quit_menu_yes_button);
	quit_menu_no_button->SetPrevInFocus(quit_menu_yes_button);
	quit_menu_yes_button->SetNextInFocus(quit_menu_no_button);
	quit_menu_yes_button->SetPrevInFocus(quit_menu_no_button);
	// ----------------------
	quit_menu_base->Desactivate();
	quit_menu_base->DesactivateChids();
	// ------------------------------------------

	//Build Setting Menu ------------------------
	
	//Settings menu base
	settings_menu = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	settings_menu->SetInputTarget(this);
	settings_menu->SetParent(settings_button);
	settings_menu->SetBox({ 0,0,0,0 });
	menu_branch->AddChild(settings_menu);

	//Quit button
	settings_quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_quit_button->SetInputTarget(this);
	settings_quit_button->SetParent(settings_menu);
	settings_quit_button->SetBox({ 1100,200,80,80 });
	settings_quit_button->SetTexOFF({ 0,776,80,80 }, ATLAS);
	settings_quit_button->SetTexON({ 0,266,80,80 }, ATLAS);
	settings_quit_button->SetTexOVER({ 0,266,80,80 }, ATLAS);
	settings_menu->AddChild(settings_quit_button);

	//Quit button string
	UI_String* set_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	set_button_string->SetInputTarget(this);
	set_button_string->SetFont(menu_ui_font->font);
	set_button_string->SetColor(menu_ui_font->font_color);
	set_button_string->SetString("X");
	set_button_string->AdjustBox();
	set_button_string->SetBoxPosition(settings_quit_button->GetBox()->w * 0.5f - set_button_string->GetBox()->w * 0.5f, settings_quit_button->GetBox()->h * 0.5f - set_button_string->GetBox()->h * 0.5f);
	settings_quit_button->AddChild(set_button_string);
	set_button_string->SetLogicalLayer(0);

	//Audio button
	settings_audio_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_audio_button->SetBox({ 515,300,570,170 });
	settings_audio_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	settings_audio_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	settings_audio_button->SetTexON({ 0,598,564,165 }, ATLAS);
	settings_audio_button->SetInputTarget(this);
	settings_audio_button->SetParent(menu_branch);
	settings_menu->AddChild(settings_audio_button);

	//Audio Button String
	UI_String* audio_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	audio_button_string->SetInputTarget(this);
	audio_button_string->SetFont(menu_ui_font->font);
	audio_button_string->SetColor(menu_ui_font->font_color);
	audio_button_string->SetString("Audio");
	audio_button_string->AdjustBox();
	audio_button_string->SetBoxPosition(settings_audio_button->GetBox()->w * 0.5f - audio_button_string->GetBox()->w * 0.5f, settings_audio_button->GetBox()->h * 0.5f - audio_button_string->GetBox()->h * 0.5f);
	settings_audio_button->AddChild(audio_button_string);
	audio_button_string->SetLogicalLayer(0);

	//Video button
	settings_video_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	settings_video_button->SetBox({ 515,500,570,170 });
	settings_video_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	settings_video_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	settings_video_button->SetTexON({ 0,598,564,165 }, ATLAS);
	settings_video_button->SetInputTarget(this);
	settings_video_button->SetParent(menu_branch);
	settings_menu->AddChild(settings_video_button);

	//Video Button String
	UI_String* video_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	video_button_string->SetInputTarget(this);
	video_button_string->SetFont(menu_ui_font->font);
	video_button_string->SetColor(menu_ui_font->font_color);
	video_button_string->SetString("Video");
	video_button_string->AdjustBox();
	video_button_string->SetBoxPosition(settings_video_button->GetBox()->w * 0.5f - video_button_string->GetBox()->w * 0.5f, settings_video_button->GetBox()->h * 0.5f - video_button_string->GetBox()->h * 0.5f);
	settings_video_button->AddChild(video_button_string);
	video_button_string->SetLogicalLayer(0);

	//Setting Menu Links ----
	settings_audio_button->SetNextInFocus(settings_video_button);
	settings_audio_button->SetPrevInFocus(settings_quit_button);
	settings_video_button->SetNextInFocus(settings_quit_button);
	settings_video_button->SetPrevInFocus(settings_audio_button);
	settings_quit_button->SetNextInFocus(settings_audio_button);
	settings_quit_button->SetPrevInFocus(settings_video_button);
	// ----------------------
	settings_menu->Desactivate();
	settings_menu->DesactivateChids();
	// ------------------------------------------

	//Build Audio Menu --------------------------
	//Audio menu base
	audio_menu = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	audio_menu->SetInputTarget(this);
	audio_menu->SetParent(settings_button);
	audio_menu->SetBox({ 100,0,0,0 });
	menu_branch->AddChild(audio_menu);
	audio_menu->SetLogicalLayer(0);

	//Quit button
	audio_quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	audio_quit_button->SetInputTarget(this);
	audio_quit_button->SetParent(settings_menu);
	audio_quit_button->SetBox({ 1000,200,80,80 });
	audio_quit_button->SetTexOFF({ 0,776,80,80 }, ATLAS);
	audio_quit_button->SetTexON({ 0,266,80,80 }, ATLAS);
	audio_quit_button->SetTexOVER({ 0,266,80,80 }, ATLAS);
	audio_menu->AddChild(audio_quit_button);

	//Quit button string
	UI_String* audio_quit_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	audio_quit_button_string->SetInputTarget(this);
	audio_quit_button_string->SetFont(menu_ui_font->font);
	audio_quit_button_string->SetColor(menu_ui_font->font_color);
	audio_quit_button_string->SetString("X");
	audio_quit_button_string->AdjustBox();
	audio_quit_button_string->SetBoxPosition(audio_quit_button->GetBox()->w * 0.5f - audio_quit_button_string->GetBox()->w * 0.5f, audio_quit_button->GetBox()->h * 0.5f - audio_quit_button_string->GetBox()->h * 0.5f);
	audio_quit_button->AddChild(audio_quit_button_string);
	audio_quit_button_string->SetLogicalLayer(0);

	//Master scroll
	master_audio_scroll = (UI_Scroll_Bar*)App->gui->GenerateUI_Element(UI_TYPE::SCROLL_BAR);
	master_audio_scroll->SetScrollBarType(SCROLL_BAR_TYPE::LATERAL_BAR);
	master_audio_scroll->SetBox({ 550,250,400,100 });
	master_audio_scroll->SetScrollMaxValue(MAX_VOLUME);
	master_audio_scroll->SetScrollableBack({ 40,50 }, { 0,0,350,50 }, ATLAS);
	master_audio_scroll->SetScrollableItem({ 50,40 }, { 0,500,30,70 }, ATLAS);
	master_audio_scroll->SetScrollValue(INITIAL_VOLUME);
	master_audio_scroll->SetInputTarget(this);
	audio_menu->AddChild(master_audio_scroll);

	//Master scroll string
	UI_String* master_scroll_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	master_scroll_string->SetInputTarget(this);
	master_scroll_string->SetFont(menu_ui_font->font);
	master_scroll_string->SetColor(menu_ui_font->font_color);
	master_scroll_string->SetString("Master");
	master_scroll_string->AdjustBox();
	master_scroll_string->SetBoxPosition(-master_audio_scroll->GetBox()->w * 0.20f - master_scroll_string->GetBox()->w * 0.5f, master_audio_scroll->GetBox()->h * 0.75f - master_scroll_string->GetBox()->h * 0.5f);
	master_audio_scroll->AddChild(master_scroll_string);
	master_scroll_string->SetLogicalLayer(0);

	//Music scroll
	music_audio_scroll = (UI_Scroll_Bar*)App->gui->GenerateUI_Element(UI_TYPE::SCROLL_BAR);
	music_audio_scroll->SetScrollBarType(SCROLL_BAR_TYPE::LATERAL_BAR);
	music_audio_scroll->SetBox({ 550,400,400,100 });
	music_audio_scroll->SetScrollMaxValue(INITIAL_VOLUME);
	music_audio_scroll->SetScrollableBack({ 40,50 }, { 0,0,350,50 }, ATLAS);
	music_audio_scroll->SetScrollableItem({ 10,40 }, { 0,500,30,70 }, ATLAS);
	music_audio_scroll->SetScrollValue(INITIAL_VOLUME);
	music_audio_scroll->SetInputTarget(this);
	audio_menu->AddChild(music_audio_scroll);

	//Music scroll string
	UI_String* music_scroll_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	music_scroll_string->SetInputTarget(this);
	music_scroll_string->SetFont(menu_ui_font->font);
	music_scroll_string->SetColor(menu_ui_font->font_color);
	music_scroll_string->SetString("Music");
	music_scroll_string->AdjustBox();
	music_scroll_string->SetBoxPosition(-music_audio_scroll->GetBox()->w * 0.20f - music_scroll_string->GetBox()->w * 0.5f, music_audio_scroll->GetBox()->h * 0.75f - music_scroll_string->GetBox()->h * 0.5f);
	music_audio_scroll->AddChild(music_scroll_string);
	music_scroll_string->SetLogicalLayer(0);

	//FX scroll
	fx_audio_scroll = (UI_Scroll_Bar*)App->gui->GenerateUI_Element(UI_TYPE::SCROLL_BAR);
	fx_audio_scroll->SetScrollBarType(SCROLL_BAR_TYPE::LATERAL_BAR);
	fx_audio_scroll->SetBox({ 550,550,400,100 });
	fx_audio_scroll->SetScrollMaxValue(INITIAL_VOLUME);
	fx_audio_scroll->SetScrollableBack({ 40,50 }, { 0,0,350,50 }, ATLAS);
	fx_audio_scroll->SetScrollableItem({ 10,40 }, { 0,500,30,70 }, ATLAS);
	fx_audio_scroll->SetScrollValue(INITIAL_VOLUME);
	fx_audio_scroll->SetInputTarget(this);
	audio_menu->AddChild(fx_audio_scroll);
	
	//Music scroll string
	UI_String* fx_scroll_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	fx_scroll_string->SetInputTarget(this);
	fx_scroll_string->SetFont(menu_ui_font->font);
	fx_scroll_string->SetColor(menu_ui_font->font_color);
	fx_scroll_string->SetString("Effects");
	fx_scroll_string->AdjustBox();
	fx_scroll_string->SetBoxPosition(-fx_audio_scroll->GetBox()->w * 0.20f - fx_scroll_string->GetBox()->w * 0.5f, fx_audio_scroll->GetBox()->h * 0.75f - fx_scroll_string->GetBox()->h * 0.5f);
	fx_audio_scroll->AddChild(fx_scroll_string);
	fx_scroll_string->SetLogicalLayer(0);

	// Audio Menu Links -----
	master_audio_scroll->SetNextInFocus(music_audio_scroll);
	master_audio_scroll->SetPrevInFocus(audio_quit_button);
	music_audio_scroll->SetNextInFocus(fx_audio_scroll);
	music_audio_scroll->SetPrevInFocus(master_audio_scroll);
	fx_audio_scroll->SetNextInFocus(audio_quit_button);
	fx_audio_scroll->SetPrevInFocus(music_audio_scroll);
	audio_quit_button->SetNextInFocus(master_audio_scroll);
	audio_quit_button->SetPrevInFocus(fx_audio_scroll);
	// ----------------------
	audio_menu->Desactivate();
	audio_menu->DesactivateChids();
	// ------------------------------------------

	//Build Video Menu --------------------------
	//Video menu base
	video_menu = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	video_menu->SetInputTarget(this);
	video_menu->SetParent(settings_button);
	video_menu->SetBox({ 0,0,0,0 });
	menu_branch->AddChild(video_menu);
	video_menu->SetLogicalLayer(0);

	//Quit button
	video_quit_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	video_quit_button->SetInputTarget(this);
	video_quit_button->SetParent(settings_menu);
	video_quit_button->SetBox({ 1100,200,80,80 });
	video_quit_button->SetTexOFF({ 0,776,80,80 }, ATLAS);
	video_quit_button->SetTexON({ 0,266,80,80 }, ATLAS);
	video_quit_button->SetTexOVER({ 0,266,80,80 }, ATLAS);
	video_menu->AddChild(video_quit_button);

	//Quit button string
	UI_String* video_quit_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	video_quit_button_string->SetInputTarget(this);
	video_quit_button_string->SetFont(menu_ui_font->font);
	video_quit_button_string->SetColor(menu_ui_font->font_color);
	video_quit_button_string->SetString("X");
	video_quit_button_string->AdjustBox();
	video_quit_button_string->SetBoxPosition(video_quit_button->GetBox()->w * 0.5f - video_quit_button_string->GetBox()->w * 0.5f, video_quit_button->GetBox()->h * 0.5f - video_quit_button_string->GetBox()->h * 0.5f);
	video_quit_button->AddChild(video_quit_button_string);
	video_quit_button_string->SetLogicalLayer(0);

	//Vsync button
	vsync_video_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	vsync_video_button->SetBox({ 515,300,570,170 });
	vsync_video_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	vsync_video_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	vsync_video_button->SetTexON({ 0,598,564,165 }, ATLAS);
	vsync_video_button->SetInputTarget(this);
	vsync_video_button->SetParent(menu_branch);
	video_menu->AddChild(vsync_video_button);

	//Vsync button string
	UI_String* vsync_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	vsync_button_string->SetInputTarget(this);
	vsync_button_string->SetFont(menu_ui_font->font);
	vsync_button_string->SetColor(menu_ui_font->font_color);
	vsync_button_string->SetString("VSync");
	vsync_button_string->AdjustBox();
	vsync_button_string->SetBoxPosition(vsync_video_button->GetBox()->w * 0.5f - vsync_button_string->GetBox()->w * 0.5f, vsync_video_button->GetBox()->h * 0.5f - vsync_button_string->GetBox()->h * 0.5f);
	vsync_video_button->AddChild(vsync_button_string);
	vsync_button_string->SetLogicalLayer(0);

	//Fullscreen button
	fullscreen_video_button = (UI_Button*)App->gui->GenerateUI_Element(UI_TYPE::BUTTON);
	fullscreen_video_button->SetBox({ 515,500,570,170 });
	fullscreen_video_button->SetTexOFF({ 0,199,564,165 }, ATLAS);
	fullscreen_video_button->SetTexOVER({ 0,598,564,165 }, ATLAS_TEST);
	fullscreen_video_button->SetTexON({ 0,598,564,165 }, ATLAS);
	fullscreen_video_button->SetInputTarget(this);
	fullscreen_video_button->SetParent(menu_branch);
	video_menu->AddChild(fullscreen_video_button);

	//Fullscreen button string
	UI_String* fullscreen_button_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	fullscreen_button_string->SetInputTarget(this);
	fullscreen_button_string->SetFont(menu_ui_font->font);
	fullscreen_button_string->SetColor(menu_ui_font->font_color);
	fullscreen_button_string->SetString("Full Screen");
	fullscreen_button_string->AdjustBox();
	fullscreen_button_string->SetBoxPosition(fullscreen_video_button->GetBox()->w * 0.5f - fullscreen_button_string->GetBox()->w * 0.5f, fullscreen_video_button->GetBox()->h * 0.5f - fullscreen_button_string->GetBox()->h * 0.5f);
	fullscreen_video_button->AddChild(fullscreen_button_string);
	fullscreen_button_string->SetLogicalLayer(0);

	// Video Menu Links -----
	vsync_video_button->SetNextInFocus(fullscreen_video_button);
	vsync_video_button->SetPrevInFocus(video_quit_button);
	fullscreen_video_button->SetNextInFocus(video_quit_button);
	fullscreen_video_button->SetPrevInFocus(vsync_video_button);
	video_quit_button->SetNextInFocus(vsync_video_button);
	video_quit_button->SetPrevInFocus(fullscreen_video_button);
	// ----------------------
	video_menu->Desactivate();
	video_menu->DesactivateChids();
	// ------------------------------------------

	//Add the built branch at the GUI 
	App->gui->PushScreen(menu_branch);

	App->audio->PlayMusic(MUSIC_ID::MUSIC_MENU);

	return true;
}

bool MainMenu::Update(float dt)
{
	App->gui->CalculateUpperElement(menu_branch);

	//EXIT --------------------------------------
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN)
	{
		if (settings_menu->GetActiveState())
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			
			//Activate menu buttons
			if (App->tutorial->TutorialCompleted())
			{
				continue_button->Activate();
				tutorial_button->Activate();
			}
			else
			{
				start_button->Activate();
			}
			settings_button->Activate();
			quit_button->Activate();
			
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)
			{
				if (App->tutorial->TutorialCompleted())App->gui->ItemSelected = continue_button;
				else App->gui->ItemSelected = start_button;
			}
		}
		else if (video_menu->GetActiveState() || audio_menu->GetActiveState())
		{
			//Deactivate settings sub menus
			video_menu->Desactivate();
			video_menu->DesactivateChids();
			audio_menu->Desactivate();
			audio_menu->DesactivateChids();
			
			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();
			
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = settings_audio_button;
		}
		else if (!quit_menu_base->GetActiveState())
		{
			quit_menu_base->Activate();
			quit_menu_base->ActivateChilds();
			quit_button->Block();
			continue_button->Block();
			tutorial_button->Block();
			settings_button->Block();
			start_button->Block();
			github_link->Block();
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = quit_menu_no_button;
		}
		else if (quit_menu_base->GetActiveState())
		{
			quit_menu_base->Desactivate();
			quit_menu_base->DesactivateChids();
			quit_button->UnBlock();
			continue_button->UnBlock();
			tutorial_button->UnBlock();
			settings_button->UnBlock();
			start_button->UnBlock();
			github_link->UnBlock();
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)
			{
				if (App->tutorial->TutorialCompleted())App->gui->ItemSelected = continue_button;
				else App->gui->ItemSelected = start_button;
			}
		}
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
	int x_vel, y_vel;
	App->input->GetMouseMotion(x_vel, y_vel);

	if (input == GUI_INPUT::MOUSE_LEFT_BUTTON_DOWN)
	{
		//Main Buttons ----------------
		if (target == start_button && !start_button->GetBlockState())
		{
			start_button->Block();
			App->ActiveTutorial();
		}
		else if (target == continue_button && !continue_button->GetBlockState())
		{
			continue_button->Block();
			App->ActiveEndless();
		}
		else if (target == tutorial_button && !tutorial_button->GetBlockState())
		{
			tutorial_button->Block();
			App->ActiveTutorial();
		}
		else if (target == settings_button)
		{
			//Deactivate start, settings & quit buttons
			start_button->Desactivate();
			continue_button->Desactivate();
			tutorial_button->Desactivate();
			settings_button->Desactivate();
			quit_button->Desactivate();
			
			//Activate settings menu and all the childs
			settings_menu->Activate();
			settings_menu->ActivateChilds();
			
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = settings_audio_button;
		}
		else if (target == quit_button)
		{
			quit_menu_base->Activate();
			quit_menu_base->ActivateChilds();
			quit_button->Block();
			continue_button->Block();
			tutorial_button->Block();
			settings_button->Block();
			start_button->Block();
			github_link->Block();
		}

		//Quit Buttons ----------------
		else if (target == quit_menu_yes_button)
		{
			App->SetQuit();
		}
		else if (target == quit_menu_no_button)
		{
			quit_menu_base->Desactivate();
			quit_menu_base->DesactivateChids();
			quit_button->UnBlock();
			continue_button->UnBlock();
			tutorial_button->UnBlock();
			settings_button->UnBlock();
			start_button->UnBlock();
			github_link->UnBlock();
		}

		//Settings Buttons ------------
		else if (target == settings_quit_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			
			//Activate menu buttons
			if (App->tutorial->TutorialCompleted())
			{
				continue_button->Activate();
				tutorial_button->Activate();
			}
			else
			{
				start_button->Activate();
			}
			settings_button->Activate();
			quit_button->Activate();
			
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)
			{
				if (App->tutorial->TutorialCompleted())App->gui->ItemSelected = continue_button;
				else App->gui->ItemSelected = start_button;
			}
		}
		else if (target == settings_audio_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			
			//Activate audio menu
			audio_menu->Activate();
			audio_menu->ActivateChilds();
			
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = master_audio_scroll;
		}
		else if (target == settings_video_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();
			
			//Activate audio menu
			video_menu->Activate();
			video_menu->ActivateChilds();
			
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = vsync_video_button;
		}
		//Audio Buttons ---------------
		else if (target == audio_quit_button)
		{
			//Deactivate audio menu
			audio_menu->Desactivate();
			audio_menu->DesactivateChids();
			
			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();
			
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = settings_audio_button;
		}
		//Video Buttons ---------------
		else if (target == video_quit_button)
		{
			//Deactivate audio menu
			video_menu->Desactivate();
			video_menu->DesactivateChids();
			
			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();
			
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = settings_audio_button;
		}
		else if (target == fullscreen_video_button)
		{
			App->win->ChangeFullscreen();
		}
		else if (target == vsync_video_button)
		{
			App->render->ChangeVSYNCstate(!App->render->vsync);
		}
		//Links Buttons ---------------
		else if (target == github_link)
		{
			ShellExecute(NULL, "open", "https://github.com/SlimeSlayer/Slime_Slayer", NULL, NULL, SW_SHOWMAXIMIZED);
		}
	}
	else if (input == GUI_INPUT::MOUSE_LEFT_BUTTON_REPEAT)
	{
		//Audio Scrolls ---------------
		if (target == master_audio_scroll)
		{
			//Master
			App->main_menu->master_audio_scroll->MoveScroll(y_vel, x_vel);
			App->endless->master_audio_scroll->MoveScroll(y_vel, x_vel);
			App->tutorial->master_audio_scroll->MoveScroll(y_vel, x_vel);
			App->audio->SetMasterVolume(master_audio_scroll->GetValue());
			//Music
			App->main_menu->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->endless->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->tutorial->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->main_menu->music_audio_scroll->RecalculateScrollValue();
			App->endless->music_audio_scroll->RecalculateScrollValue();
			App->tutorial->music_audio_scroll->RecalculateScrollValue();
			App->audio->SetMusicVolume(music_audio_scroll->GetValue());
			//FX
			App->main_menu->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->endless->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->tutorial->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->main_menu->fx_audio_scroll->RecalculateScrollValue();
			App->endless->fx_audio_scroll->RecalculateScrollValue();
			App->tutorial->fx_audio_scroll->RecalculateScrollValue();
			App->audio->SetFXVolume(fx_audio_scroll->GetValue());
		}
		else if (target == music_audio_scroll)
		{
			App->main_menu->music_audio_scroll->MoveScroll(y_vel, x_vel);
			App->endless->music_audio_scroll->MoveScroll(y_vel, x_vel);
			App->tutorial->music_audio_scroll->MoveScroll(y_vel, x_vel);
			App->audio->SetMusicVolume(music_audio_scroll->GetValue());
		}
		else if (target == fx_audio_scroll)
		{
			App->main_menu->fx_audio_scroll->MoveScroll(y_vel, x_vel);
			App->endless->fx_audio_scroll->MoveScroll(y_vel, x_vel);
			App->tutorial->fx_audio_scroll->MoveScroll(y_vel, x_vel);
			App->audio->SetFXVolume(fx_audio_scroll->GetValue());
		}
	}
}

void MainMenu::GUI_Controller_Input(INPUT_EVENT input_event)
{
	UI_Element* target = App->gui->ItemSelected;

	if (App->input_manager->GetEvent(INPUT_EVENT::FOCUS_NEXT) == INPUT_DOWN && target != nullptr)
	{
		App->gui->ItemSelected = target->GetNextInFocus();
	}

	else if (App->input_manager->GetEvent(INPUT_EVENT::FOCUS_PREV) == INPUT_DOWN && target != nullptr)
	{
		App->gui->ItemSelected = target->GetPrevInFocus();
	}

	else if (App->input_manager->GetEvent(ACCEPT) == INPUT_DOWN)
	{
		//Main Buttons ----------------
		if (target == start_button && !start_button->GetBlockState())
		{
			start_button->Block();
			App->ActiveTutorial();
		}
		else if (target == continue_button && !start_button->GetBlockState())
		{
			continue_button->Block();
			App->ActiveEndless();
		}
		else if (target == tutorial_button && !start_button->GetBlockState())
		{
			tutorial_button->Block();
			App->ActiveTutorial();
		}
		else if (target == settings_button)
		{
			//Deactivate start, settings & quit buttons
			start_button->Desactivate();
			continue_button->Desactivate();
			tutorial_button->Desactivate();
			settings_button->Desactivate();
			quit_button->Desactivate();

			//Activate settings menu and all the childs
			settings_menu->Activate();
			settings_menu->ActivateChilds();

			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = settings_audio_button;
		}
		else if (target == quit_button)
		{
			quit_menu_base->Activate();
			quit_menu_base->ActivateChilds();
			quit_button->Block();
			continue_button->Block();
			tutorial_button->Block();
			settings_button->Block();
			start_button->Block();
			github_link->Block();
			App->gui->ItemSelected = quit_menu_no_button;
		}

		//Quit Buttons ----------------
		else if (target == quit_menu_yes_button)
		{
			App->SetQuit();
		}
		else if (target == quit_menu_no_button)
		{
			quit_menu_base->Desactivate();
			quit_menu_base->DesactivateChids();
			quit_button->UnBlock();
			continue_button->UnBlock();
			tutorial_button->UnBlock();
			settings_button->UnBlock();
			start_button->UnBlock();
			github_link->UnBlock();
			//Set the correct input target in the new menu
			if (App->gui->controller_mode)
			{
				if (App->tutorial->TutorialCompleted())App->gui->ItemSelected = continue_button;
				else App->gui->ItemSelected = start_button;
			}
		}

		//Settings Buttons ------------
		else if (target == settings_quit_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();

			//Activate menu buttons
			if (App->tutorial->TutorialCompleted())
			{
				continue_button->Activate();
				tutorial_button->Activate();
			}
			else
			{
				start_button->Activate();
			}
			settings_button->Activate();
			quit_button->Activate();

			//Set the correct input target in the new menu
			if (App->gui->controller_mode)
			{
				if (App->tutorial->TutorialCompleted())App->gui->ItemSelected = continue_button;
				else App->gui->ItemSelected = start_button;
			}
		}
		else if (target == settings_audio_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();

			//Activate audio menu
			audio_menu->Activate();
			audio_menu->ActivateChilds();

			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = master_audio_scroll;
		}
		else if (target == settings_video_button)
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();

			//Activate audio menu
			video_menu->Activate();
			video_menu->ActivateChilds();

			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = vsync_video_button;
		}
		//Audio Buttons ---------------
		else if (target == audio_quit_button)
		{
			//Deactivate audio menu
			audio_menu->Desactivate();
			audio_menu->DesactivateChids();

			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();

			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = settings_audio_button;
		}
		//Video Buttons ---------------
		else if (target == video_quit_button)
		{
			//Deactivate audio menu
			video_menu->Desactivate();
			video_menu->DesactivateChids();

			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();

			//Set the correct input target in the new menu
			if (App->gui->controller_mode)App->gui->ItemSelected = settings_audio_button;
		}
		else if (target == fullscreen_video_button)
		{
			App->win->ChangeFullscreen();
		}
		else if (target == vsync_video_button)
		{
			App->render->ChangeVSYNCstate(!App->render->vsync);
		}
	}
	else if (App->input_manager->GetEvent(INPUT_EVENT::ADD_VALUE) != INPUT_NONE || App->input_manager->GetEvent(INPUT_EVENT::REST_VALUE) != INPUT_NONE)
	{
		uint value_to_add = 0;
		INPUT_STATE input_state = INPUT_NONE;
		if (App->input_manager->GetEvent(INPUT_EVENT::ADD_VALUE) != INPUT_NONE)
		{
			value_to_add = CONTROLLER_ON_ADD_VALUE * App->GetDT();
			input_state = App->input_manager->GetEvent(INPUT_EVENT::ADD_VALUE);
		}
		else
		{
			value_to_add = CONTROLLER_ON_REST_VALUE * App->GetDT();
			input_state = App->input_manager->GetEvent(INPUT_EVENT::REST_VALUE);
		}

		//Audio Scrolls ---------------
		if (target == master_audio_scroll)
		{
			//Master
			App->main_menu->master_audio_scroll->MoveScroll(0, value_to_add);
			App->endless->master_audio_scroll->MoveScroll(0, value_to_add);
			App->tutorial->master_audio_scroll->MoveScroll(0, value_to_add);
			App->audio->SetMasterVolume(master_audio_scroll->GetValue());
			//Music
			App->main_menu->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->endless->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->tutorial->music_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->main_menu->music_audio_scroll->RecalculateScrollValue();
			App->endless->music_audio_scroll->RecalculateScrollValue();
			App->tutorial->music_audio_scroll->RecalculateScrollValue();
			App->audio->SetMusicVolume(music_audio_scroll->GetValue());
			//FX
			App->main_menu->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->endless->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->tutorial->fx_audio_scroll->SetScrollMaxValue(master_audio_scroll->GetValue());
			App->main_menu->fx_audio_scroll->RecalculateScrollValue();
			App->endless->fx_audio_scroll->RecalculateScrollValue();
			App->tutorial->fx_audio_scroll->RecalculateScrollValue();
			App->audio->SetFXVolume(fx_audio_scroll->GetValue());
		}
		else if (target == music_audio_scroll)
		{
			App->main_menu->music_audio_scroll->MoveScroll(0, value_to_add);
			App->endless->music_audio_scroll->MoveScroll(0, value_to_add);
			App->tutorial->music_audio_scroll->MoveScroll(0, value_to_add);
			App->audio->SetMusicVolume(music_audio_scroll->GetValue());
		}
		else if (target == fx_audio_scroll)
		{
			App->main_menu->fx_audio_scroll->MoveScroll(0, value_to_add);
			App->endless->fx_audio_scroll->MoveScroll(0, value_to_add);
			App->tutorial->fx_audio_scroll->MoveScroll(0, value_to_add);
			App->audio->SetFXVolume(fx_audio_scroll->GetValue());
		}
		//Quit menu
		if (input_state == INPUT_DOWN)
		{
			if (target == quit_menu_no_button)
			{
				App->gui->ItemSelected = quit_menu_no_button->GetNextInFocus();
			}
			else if (target == quit_menu_yes_button)
			{
				App->gui->ItemSelected = quit_menu_yes_button->GetNextInFocus();
			}
		}
	}
}

UI_Element * MainMenu::GetCorrectItemToSelect() const
{
	UI_Element* item_to_select = nullptr;

	//Setting Menu Case -----
	if (settings_menu->GetActiveState())
	{
		item_to_select = settings_audio_button;
	}

	//Audio Menu Case -------
	else if (audio_menu->GetActiveState())
	{
		item_to_select = master_audio_scroll;
	}

	//Video Menu Case -------
	else if (video_menu->GetActiveState())
	{
		item_to_select = vsync_video_button;
	}

	//Quit Menu Case --------
	else if (quit_menu_base->GetActiveState())
	{
		item_to_select = quit_menu_no_button;
	}

	//Main Menu Menu Case ---
	else if(App->tutorial->TutorialCompleted())
	{
		item_to_select = continue_button;
	}
	else
	{
		item_to_select = start_button;
	}

	return item_to_select;
}
