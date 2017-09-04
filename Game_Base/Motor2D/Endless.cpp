#define _CRT_SECURE_NO_WARNINGS

#include "Endless.h"

#include "j1App.h"
#include "j1Physics.h"
#include "j1Audio.h"
#include "j1InputManager.h"
#include "j1EntitiesManager.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1FileSystem.h"
#include "j1ParticleManager.h"

#include "Parallax.h"
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UI_Scroll_Bar.h"

// Constructor ==================================
Endless::Endless()
{
	name = "endless";
}

// Destructor ===================================
Endless::~Endless()
{

}

void Endless::Init()
{
	active = false;
	enabled = false;
}

// Game Loop ====================================
bool Endless::Enable()
{
	if (!base_enabled)
	{
		//UI Activation ---------------------------------------
		menu_branch->Activate();
		menu_branch->ActivateChilds();
		settings_menu->Desactivate();
		settings_menu->DesactivateChids();
		audio_menu->Desactivate();
		audio_menu->DesactivateChids();
		video_menu->Desactivate();
		video_menu->DesactivateChids();
		death_menu->Desactivate();
		death_menu->DesactivateChids();
		//Map build -------------------------------------------
		//Floor -----------------
		current_enable_node = data_doc.root().first_child().child("floor_collider");

		SDL_Rect rect = App->entities_manager->TokenStrToRect(current_enable_node.attribute("rect").as_string());
		floor_collider = App->physics->CreateRectangle(rect.x, rect.y, rect.w, rect.h, COLLISION_TYPE::MAP_COLLISION, BODY_TYPE::MAP_BODY);
		floor_collider->body->SetType(b2BodyType::b2_staticBody);
		floor_collider->body->GetFixtureList()->SetFriction(current_enable_node.attribute("friction").as_float());

		//Front Parallax --------
		current_enable_node = data_doc.root().first_child().child("front_pllx");

		front_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		front_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		front_parallax->SetTextureRect(App->entities_manager->TokenStrToRect(current_enable_node.attribute("rect").as_string()));
		front_parallax->SetPosition(0, current_enable_node.attribute("pos_y").as_int());

		//Mid Parallax ----------
		current_enable_node = data_doc.root().first_child().child("mid_pllx");

		mid_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		mid_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		mid_parallax->SetTextureRect(App->entities_manager->TokenStrToRect(current_enable_node.attribute("rect").as_string()));
		mid_parallax->SetPosition(0, current_enable_node.attribute("pos_y").as_int());

		//Back Parallax ---------
		current_enable_node = data_doc.root().first_child().child("back_pllx");

		back_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		back_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		back_parallax->SetTextureRect(App->entities_manager->TokenStrToRect(current_enable_node.attribute("rect").as_string()));
		back_parallax->SetPosition(0, current_enable_node.attribute("pos_y").as_int());

		//Prepare tutorial entities ---------------------------
		current_enable_node = data_doc.root().first_child().child("player_avatar");

		App->player->avatar->SetPosition(current_enable_node.attribute("pos_x").as_float(), current_enable_node.attribute("pos_y").as_float());

		//Set enable node root at null
		current_enable_node = current_enable_node.child("none");

		//Set boolean to check that base data has been loaded
		base_enabled = true;

		return false; /*This divide the first load step in one frame*/
	}
	else
	{
		j1Timer	enable_timer;

		//If the node is null focus it to the first scene entity
		if (current_enable_node.root() == NULL)
		{
			current_enable_node = data_doc.root().first_child().child("entities").first_child();
		}

		while (current_enable_node.root() != NULL)
		{

			ENTITY_TYPE entity_type = App->entities_manager->StrToEntityType(current_enable_node.attribute("entity_type").as_string());
			Entity* new_entity = nullptr;

			switch (entity_type)
			{
			case CREATURE:
			{
				CREATURE_TYPE creature_type = App->entities_manager->StrToCreatureType(current_enable_node.attribute("creature_type").as_string());
				new_entity = App->entities_manager->GenerateCreature(creature_type);

				break;
			}
			case ITEM:
				ITEM_TYPE item_type = App->entities_manager->StrToItemType(current_enable_node.attribute("item_type").as_string());
				new_entity = App->entities_manager->GenerateItem(item_type);

				break;
			}

			//Set the new entity scene data
			/*Position*/	new_entity->SetPosition(current_enable_node.attribute("pos_x").as_float(), current_enable_node.attribute("pos_y").as_float());

			//Focus the next entity node
			current_enable_node = current_enable_node.next_sibling();

			if (enable_timer.Read() > TIME_TO_ENABLE)return false;
		}
	}

	//Generate portals at spawn coordinates
	uint size = spawn_coordinates.size();
	for (uint k = 0; k < size; k++)
	{
		Particle* portal = App->particle_manager->GenerateAnimationParticle(PARTICLE_TYPE::PORTAL_PARTICLE);
		portal->SetPosition(spawn_coordinates[k].x, spawn_coordinates[k].y);
	}
	
	//Play scene music
	App->audio->PlayMusic(MUSIC_ID::MUSIC_IN_GAME);

	//Reset creatures spawn data
	current_defeat_creatures = current_alive_creatures = 0;
	wave = 0;
	next_wave_creatures = ceil(start_creatures * (wave_evolve * (wave + 1)));

	//Reset timers
	next_spawn_time = initial_spawn_time;
	spawn_timer.Start();
	
	enabled = true;
	active = false;

	return true;
}

void Endless::Disable()
{
	LOG("Disabling Endless");

	//UI Deactivation
	settings_exit_scene_button->UnBlock();
	death_end_button->UnBlock();
	death_reset_button->UnBlock();
	menu_branch->Desactivate();
	menu_branch->DesactivateChids();

	//Release parallax system
	if (front_parallax != nullptr) RELEASE(front_parallax);
	if (mid_parallax != nullptr)RELEASE(mid_parallax);
	if (back_parallax != nullptr)RELEASE(back_parallax);

	//Release tutorial physic system
	if (floor_collider != nullptr)RELEASE(floor_collider);

	//Delete all the current entities in the scene
	App->entities_manager->DeleteCurrentEntities();

	active = enabled = base_enabled = false;

	LOG("Endless Disabled");
}

void Endless::RestartScene()
{
	App->ActiveEndless();
}

bool Endless::Awake(pugi::xml_node & data_node)
{
	LOG("Loading %s Definition Doc", name.c_str());

	//Load the data document used to generate the scene when is enabled
	App->fs->LoadXML(data_node.child("data_folder").child_value(), &data_doc);
	if (data_doc.root() == NULL)
	{
		LOG("Error Loading %s Doc", name.c_str());
	}

	return true;
}

bool Endless::Start()
{
	SceneStart();

	//Load timing data ----------------
	pugi::xml_node timing_node = data_doc.root().first_child().child("spawn");
	initial_spawn_time = timing_node.attribute("initial_time").as_uint();
	spawn_rate = timing_node.attribute("spawn_rate").as_uint();
	wave_evolve = timing_node.attribute("wave_evolve").as_float();
	effect_waves = timing_node.attribute("boss_waves").as_uint();
	start_creatures = timing_node.attribute("start_creatures").as_uint();

	//Load Spawn coordinates ----------
	pugi::xml_node cord = timing_node.child("spawn_cords").first_child();
	while (cord.root() != NULL)
	{
		spawn_coordinates.push_back({ cord.attribute("x").as_int(),cord.attribute("y").as_int() });

		cord = cord.next_sibling();
	}

	//Load wave animation data --------
	pugi::xml_node ui_node = data_doc.root().first_child().child("ui");
	anim_duration = ui_node.attribute("wave_anim_duration").as_uint();
	bar_anim_add = ui_node.attribute("bar_anim_add").as_float();

	//Build endless UI ----------------
	wave_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	Font_Def* font = App->font->GetFontByID(FONT_ID::MENU_UI_FONT);
	wave_string->SetColor(font->font_color);
	wave_string->SetFont(font->font);
	wave_string->SetInputTarget(this);
	wave_string->SetVisualLayer(25);
	wave_string->SetLogicalLayer(0);
	wave_string->SetUseCamera(true);
	
	wave_percent_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	font = App->font->GetFontByID(FONT_ID::MENU_DIALOG_FONT);
	wave_percent_string->SetColor(font->font_color);
	wave_percent_string->SetFont(font->font);
	wave_percent_string->SetInputTarget(this);
	wave_percent_string->SetVisualLayer(26);
	wave_percent_string->SetLogicalLayer(0);
	wave_percent_string->SetUseCamera(true);
	wave_percent_string->SetString("0 %");

	wave_shield_icon = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	wave_shield_icon->SetInputTarget(this);
	wave_shield_icon->ChangeTextureId(TEXTURE_ID::WAVE_SHIELD_ICON);
	wave_shield_icon->SetTextureScale(0.5f);
	wave_shield_icon->AdjustBox();
	wave_shield_icon->SetBoxPosition(App->render->camera.w * 0.5 - wave_shield_icon->GetBox()->w, App->render->camera.h * 0.5 - wave_shield_icon->GetBox()->h * 0.5);
	wave_shield_icon->SetVisualLayer(24);

	wave_progress_bar = (UI_Progressive_Bar*)App->gui->GenerateUI_Element(UI_TYPE::PROGRESSIVE_BAR);
	SDL_Rect bar_rect = { App->render->camera.w * 0.5 - 75, App->render->camera.h * 0.5,400,40 };
	wave_progress_bar->SetBox(bar_rect);
	wave_progress_bar->SetFullColor({ 255,50,255,255 });
	wave_progress_bar->SetEmptyColor({ 70,70,70,255 });
	wave_progress_bar->SetToEmptyColor({ 255,255,255,255 });
	wave_progress_bar->SetVisualLayer(25);
	wave_progress_bar->SetToEmptyRestValue(1.0);
	wave_progress_bar->SetInputTarget(this);
	wave_progress_bar->SetMaxValue(effect_waves);
	wave_progress_bar->GenerateTexture();

	return true;
}

bool Endless::Update(float dt)
{
	// UI UPDATE --------------------------------
	App->gui->CalculateUpperElement(menu_branch);
	if (wave_animation)
	{
		//Update animation alpha
		if (anim_timer.Read() <= anim_duration)
		{
			anim_alpha = MIN(MAX(0.01,(anim_timer.Read() / (anim_duration*0.5)) * 255), 255);
			
		}
		else
		{
			float per_cent = 1-(((float)anim_timer.Read() - (float)anim_duration) / (float)anim_duration);
			anim_alpha = MAX(0.00, per_cent * 255);
		}
		
		//Update animation progressive bar
		if (anim_alpha == 255 && bar_anim_added < 1.00)
		{
			//Update bar
			float _add = bar_anim_add * App->GetDT();
			_add = MIN(1.00 - bar_anim_added, _add);
			bar_anim_added += _add;
			wave_progress_bar->AddValue(_add);
			wave_progress_bar->UpdateTexture();
			//Update percent string
			float cent_val = MAX(0, (wave_progress_bar->GetCurrentValue() / wave_progress_bar->GetMaxValue()) * 100);
			cent_val = (cent_val >(floor(cent_val) + 0.5f)) ? ceil(cent_val) : floor(cent_val);
			char buffer[6];
			_itoa(cent_val, buffer, 10);
			std::string w_str = buffer;
			w_str += " %";
			wave_percent_string->SetString(w_str.c_str());
			wave_percent_string->AdjustBox();
			wave_percent_string->SetBoxPosition(wave_progress_bar->GetBox()->x + wave_progress_bar->GetBox()->w * 0.5 - wave_percent_string->GetBox()->w * 0.5, wave_progress_bar->GetBox()->y + wave_progress_bar->GetBox()->h * 0.5 - wave_percent_string->GetBox()->h * 0.5);
		}

		//Animation UI blit
		App->render->CallBlit(wave_progress_bar->GetTexture(), wave_progress_bar->GetBox()->x, wave_progress_bar->GetBox()->y, NULL, true, false, false, 1.0f, wave_progress_bar->GetVisualLayer(), anim_alpha);
		App->render->CallBlit(wave_percent_string->GetTextTexture(), wave_percent_string->GetBox()->x, wave_percent_string->GetBox()->y, NULL, true, false, false, 1.0f, wave_percent_string->GetVisualLayer(), anim_alpha);
		App->render->CallBlit(wave_shield_icon->GetTexture(), wave_shield_icon->GetBox()->x, wave_shield_icon->GetBox()->y, NULL, true, false, false, wave_shield_icon->GetTextureScale(), wave_shield_icon->GetVisualLayer(), anim_alpha);
		App->render->CallBlit(wave_string->GetTextTexture(), wave_string->GetBox()->x, wave_string->GetBox()->y, NULL, true, false, false, 1.0f, wave_string->GetVisualLayer(), anim_alpha);
		
		//End case
		if (anim_alpha == 0.00)
		{
			wave_animation = false;
			anim_alpha = 0.01;
			bar_anim_added = 0.0;
			//Check if 5 waves have been completed
			if (wave % effect_waves == 0)
			{
				wave_progress_bar->SetCurrentValue(0.0f);
			}
		}
	}

	// ------------------------------------------

	// CREATURES FACTORY ------------------------
	if (spawn_timer.Read() > next_spawn_time && (current_defeat_creatures + current_alive_creatures < next_wave_creatures))
	{
		Creature* enemy = nullptr;
		if (wave % effect_waves == 0 && wave > 0)enemy = App->entities_manager->GenerateCreature(CREATURE_TYPE::BOSS_ENEMY_CREATURE);
		else enemy = App->entities_manager->GenerateCreature(CREATURE_TYPE::BASIC_ENEMY_CREATURE);
		enemy->worker.AddPriorizedAction(enemy->worker.GenerateAction(LG_ACTION_TYPE::LG_MAGIC_SPAWN_ACTION, enemy, 2000, 2, 0.1));
		int rand_index = rand() % spawn_coordinates.size();
		enemy->SetPosition(spawn_coordinates[0].x, spawn_coordinates[0].y);
		current_alive_creatures++;
		next_spawn_time = spawn_rate;
		spawn_timer.Start();
	}

	// ------------------------------------------

	//Blit scene Parallax -----------------------
	back_parallax->Draw(-30);
	mid_parallax->Draw(-20);
	front_parallax->Draw(-10);
	// ------------------------------------------

	//EXIT --------------------------------------
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_DOWN)
	{
		if (settings_menu->GetActiveState())
		{
			//Deactivate settings menu
			settings_menu->Desactivate();
			settings_menu->DesactivateChids();

			//Activate menu buttons
			settings_button->Activate();

			//Set the correct app context
			App->app_context = IN_GAME_CONTEXT;
			App->pause = false;

			//Set the correct input target in the new menu
			App->gui->ItemSelected = nullptr;
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
			App->gui->ItemSelected = settings_audio_button;
		}
		else if (!death_menu->GetActiveState())
		{
			//Deactivate settings button
			settings_button->Desactivate();
			//Activate settings menu
			settings_menu->Activate();
			settings_menu->ActivateChilds();

			//Set the correct app context
			App->app_context = PAUSE_CONTEXT;
			App->pause = true;

			//Set the correct input target in the new menu
			App->gui->ItemSelected = settings_audio_button;
		}
	}
	// ------------------------------------------

	//TEST ZONE ---------------------------------
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		Item* jar = App->entities_manager->GenerateItem(BASIC_BOX_ITEM);
		jar->GetBody()->SetPosition((float)App->input->GetMouseX() - App->render->camera.x, (float)App->input->GetMouseY() - App->render->camera.y);
	}
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		Creature* enemy = App->entities_manager->GenerateCreature(CREATURE_TYPE::BASIC_ENEMY_CREATURE);
		enemy->GetBody()->SetPosition(App->input->GetMouseX() - App->render->camera.x, App->input->GetMouseY() - App->render->camera.y);
	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		Creature* enemy = App->entities_manager->GenerateCreature(CREATURE_TYPE::BOSS_ENEMY_CREATURE);
		enemy->GetBody()->SetPosition(App->input->GetMouseX() - App->render->camera.x, App->input->GetMouseY() - App->render->camera.y);
	}
	// ------------------------------------------
	return true;
}

void Endless::CreaturesCount(uint defs)
{
	//Rest the defeated creatures to the current ones
	current_alive_creatures -= defs;
	current_defeat_creatures += defs;

	//If the wave has been passed new creatures are generated
	if (current_alive_creatures == 0 && current_defeat_creatures == next_wave_creatures)
	{
		wave++;
		next_wave_creatures = ceil(start_creatures * (wave_evolve * (wave + 1)));
		current_defeat_creatures = 0;
		//Active/Rest all the related UI
		anim_alpha = 0.01f;
		wave_animation = true;
		//Wave str
		char buffer [6];
		_itoa(wave, buffer, 10);
		std::string w_str = "Wave ";
		w_str += buffer;
		wave_string->SetString(w_str.c_str());
		wave_string->AdjustBox();
		wave_string->SetBoxPosition(App->render->viewport.w * 0.5 - wave_string->GetBox()->w * 0.5, App->render->viewport.h * 0.5 - wave_string->GetBox()->h * 0.5 - 50);
		//Percent str
		float cent_val = MAX(0, (wave_progress_bar->GetCurrentValue() / wave_progress_bar->GetMaxValue()) * 100);
		cent_val = (cent_val > (floor(cent_val) + 0.5f)) ? ceil(cent_val) : floor(cent_val);
		char n_buffer[6];
		_itoa(cent_val, n_buffer, 10);
		w_str = n_buffer;
		w_str += " %";
		wave_percent_string->SetString(w_str.c_str());
		//Bar
		wave_progress_bar->UpdateTexture();
		
		//Add a delay on the spawn to define the wave
		next_spawn_time = initial_spawn_time;
		spawn_timer.Start();
		anim_timer.Start();
	}

}
