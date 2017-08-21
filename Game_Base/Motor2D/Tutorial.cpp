#include "Tutorial.h"

#include "j1App.h"
#include "j1Physics.h"
#include "j1Audio.h"
#include "j1InputManager.h"
#include "j1EntitiesManager.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1FileSystem.h"

#include "Parallax.h"
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "UI_Scroll_Bar.h"

// Constructor ==================================
Tutorial::Tutorial()
{
	name = "tutorial";
}

// Destructor ===================================
Tutorial::~Tutorial()
{

}

// Game Loop ====================================
void Tutorial::Init()
{
	active = false;
	enabled = false;
}

bool Tutorial::Enable()
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

		floor_collider = App->physics->CreateRectangle(
			current_enable_node.attribute("x").as_int(),
			current_enable_node.attribute("y").as_int(),
			current_enable_node.attribute("w").as_int(),
			current_enable_node.attribute("h").as_int(),
			COLLISION_TYPE::MAP_COLLISION, BODY_TYPE::MAP_BODY
		);
		floor_collider->body->SetType(b2BodyType::b2_staticBody);
		floor_collider->body->GetFixtureList()->SetFriction(current_enable_node.attribute("friction").as_float());

		//End Trigger -----------
		current_enable_node = data_doc.root().first_child().child("end_trigger");

		end_trigger = App->physics->CreateRectangle(
			current_enable_node.attribute("x").as_int(),
			current_enable_node.attribute("y").as_int(),
			current_enable_node.attribute("w").as_int(),
			current_enable_node.attribute("h").as_int(),
			COLLISION_TYPE::MAP_COLLISION, BODY_TYPE::MAP_BODY
		);
		end_trigger->body->SetType(b2BodyType::b2_staticBody);
		end_trigger->body->GetFixtureList()->SetSensor(true);
		end_trigger->listener = this;

		//Front Parallax --------
		current_enable_node = data_doc.root().first_child().child("front_pllx");
		
		front_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		front_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		front_parallax->SetTextureRect({ 
			current_enable_node.attribute("tex_x").as_int(),
			current_enable_node.attribute("tex_y").as_int(),
			current_enable_node.attribute("tex_w").as_int(),
			current_enable_node.attribute("tex_h").as_int()
		});
		front_parallax->SetPosition(0, current_enable_node.attribute("pos_y").as_int());

		//Mid Parallax ----------
		current_enable_node = data_doc.root().first_child().child("mid_pllx");
		
		mid_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		mid_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		mid_parallax->SetTextureRect({
			current_enable_node.attribute("tex_x").as_int(),
			current_enable_node.attribute("tex_y").as_int(),
			current_enable_node.attribute("tex_w").as_int(),
			current_enable_node.attribute("tex_h").as_int()
		});
		mid_parallax->SetPosition(0, current_enable_node.attribute("pos_y").as_int());

		//Back Parallax ---------
		current_enable_node = data_doc.root().first_child().child("back_pllx");
		
		back_parallax = new Parallax(current_enable_node.attribute("cells").as_uint());
		back_parallax->SetTexture(App->tex->Load(current_enable_node.attribute("tex_folder").as_string()));
		back_parallax->SetTextureRect({
			current_enable_node.attribute("tex_x").as_int(),
			current_enable_node.attribute("tex_y").as_int(),
			current_enable_node.attribute("tex_w").as_int(),
			current_enable_node.attribute("tex_h").as_int()
		});
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
				if (item_type == BASIC_BOX_ITEM)
				{
					//Get the items in the items tank
					std::vector<ITEM_TYPE> items_ty_vec = App->entities_manager->TokenStrToItemTypes(current_enable_node.attribute("items_in").as_string());
					uint size = items_ty_vec.size();
					for (uint k = 0; k < size; k++)
					{
						Item* new_item = App->entities_manager->GenerateItem(items_ty_vec[k], false);
						((Items_Tank*)new_entity)->AddItem(new_item);
					}
				}
				break;
			}

			//Set the new entity scene data
			/*Position*/	new_entity->SetPosition(current_enable_node.attribute("pos_x").as_float(), current_enable_node.attribute("pos_y").as_float());

			//Focus the next entity node
			current_enable_node = current_enable_node.next_sibling();

			if (enable_timer.Read() > TIME_TO_ENABLE && current_enable_node.root() != NULL)return false;
		}
	}

	//Play scene music
	App->audio->PlayMusic(MUSIC_ID::MUSIC_IN_GAME);

	enabled = true;
	active = base_enabled = false;

	return true;
}

void Tutorial::Disable()
{
	LOG("Disabling Tutorial");

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
	if (end_trigger != nullptr)RELEASE(end_trigger);

	//Delete all the current entities in the scene
	App->entities_manager->DeleteCurrentEntities();

	active = enabled = base_enabled = false;
}

void Tutorial::RestartScene()
{
	App->ActiveTutorial();
}

bool Tutorial::Update(float dt)
{
	App->gui->CalculateUpperElement(menu_branch);

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
		else if(!death_menu->GetActiveState())
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
		jar->GetBody()->SetPosition(App->input->GetMouseX() - App->render->camera.x, App->input->GetMouseY() - App->render->camera.y);
	}
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		Creature* enemy = App->entities_manager->GenerateCreature(CREATURE_TYPE::BASIC_ENEMY_CREATURE);
		enemy->GetBody()->SetPosition(App->input->GetMouseX() - App->render->camera.x, App->input->GetMouseY() - App->render->camera.y);
	}
	// ------------------------------------------

	return true;
}

bool Tutorial::CleanUp()
{
	LOG("Freeing Tutorial");

	if (front_parallax != nullptr) delete front_parallax;
	if (mid_parallax != nullptr)delete mid_parallax;
	if (back_parallax != nullptr)delete back_parallax;

	if (floor_collider != nullptr)delete floor_collider;
	if (end_trigger != nullptr)delete end_trigger;

	data_doc.reset();

	return true;
}

bool Tutorial::GeneralSave(pugi::xml_node & node) const
{
	//Save if the tutorial is done
	pugi::xml_node tut_node = node.append_child("tutorial_completed");
	tut_node.append_attribute("val") = tutorial_completed;

	return true;
}

bool Tutorial::GeneralLoad(pugi::xml_node & node)
{
	tutorial_completed = node.child("tutorial_completed").attribute("val").as_bool();

	return true;
}

void Tutorial::BeginSensorCollision(PhysBody * A, PhysBody * B)
{
	if (A == end_trigger && B->entity_related == App->player->avatar)
	{
		tutorial_completed = true;
		App->SaveGeneralData(App->fs->general_save_file.c_str());
		App->ActiveEndless();
	}
}
