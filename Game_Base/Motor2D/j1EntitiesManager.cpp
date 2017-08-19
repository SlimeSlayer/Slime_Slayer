#define _CRT_SECURE_NO_WARNINGS

#include "j1EntitiesManager.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Physics.h"
#include "j1Animator.h"
#include "j1FileSystem.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "Worker.h"
#include "Scene.h"
#include "j1Input.h"
#include "j1Render.h"

#include "UI_String.h"

// Constructors =================================
j1EntitiesManager::j1EntitiesManager()
{
	name = "entities_manager";
}

// Destructors ==================================
j1EntitiesManager::~j1EntitiesManager()
{

}


// Game Loop ====================================
void j1EntitiesManager::Init()
{
	active = enabled = false;
}

bool j1EntitiesManager::Enable()
{
	
	j1Timer process_timer;

	if (current_enable_group.root() == NULL && current_entity_type == NO_ENTITY)
	{
		current_enable_group = definitions_doc.first_child().first_child();
	}

	while (current_enable_group != NULL)
	{
		if (current_enble_node == nullptr)
		{
			current_enble_node = current_enable_group.first_child();
			current_entity_type = StrToEntityType(current_enable_group.name());
		}

		while (current_enble_node != NULL)
		{
			switch (current_entity_type)
			{
			case ITEM:					AddItemDefinition(&current_enble_node);			break;
			case CREATURE:				AddCreatureDefinition(&current_enble_node);		break;
			case CREATURE_EVO_TEMPLATE:	AddCreatureEvoTemplate(&current_enble_node);	break;
			}

			current_enble_node = current_enble_node.next_sibling();

			if (process_timer.Read() > TIME_TO_ENABLE && current_enble_node != NULL)
			{
				return false;
			}
		}
		//Focus the next group node & reset enable node
		current_enable_group = current_enable_group.next_sibling();
	}

	current_entity_type = NO_ENTITY;

	enabled = true;
	active = false;

	return true;
}

void j1EntitiesManager::Disable()
{
	// Clean all the creatures definitions ------
	uint size = creatures_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(creatures_defs[k]);
	}
	creatures_defs.clear();

	// Clean all the creatures evolutions temp --
	size = creatures_evo_templates.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(creatures_evo_templates[k]);
	}
	creatures_evo_templates.clear();

	// Clean all the items definitions ----------
	size = items_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(items_defs[k]);
	}
	items_defs.clear();

	// Clean all the entities to delete ---------
	std::list<Entity*>::iterator entities_to_del_item = entitites_to_delete.begin();
	while (entities_to_del_item != entitites_to_delete.end())
	{
		current_entities.remove(entities_to_del_item._Ptr->_Myval);
		RELEASE(entities_to_del_item._Ptr->_Myval);
		entities_to_del_item++;
	}
	entitites_to_delete.clear();

	// Clean all the current entities -----------
	std::list<Entity*>::iterator cur_entities_item = current_entities.begin();
	while (cur_entities_item != current_entities.end())
	{
		RELEASE(cur_entities_item._Ptr->_Myval);
		cur_entities_item++;
	}
	current_entities.clear();

	enabled = active = false;
}

bool j1EntitiesManager::Awake(pugi::xml_node & node)
{
	LOG("Loading Entities Definitions Doc");
	App->fs->LoadXML(node.child("entities_data").attribute("file").as_string(),&definitions_doc);
	if (definitions_doc.root() == NULL)
	{
		LOG("Error Loading Entities Doc");
		return false;
	}

	//Load entities UI data
	//Entities UI visual layer
	entities_ui_visual_layer = node.child("entities_ui").attribute("visual_layer").as_int();
	
	//Selected entity string data
	selected_str_margin = node.child("selected_string").attribute("margin").as_uint();
	selected_update_rate = node.child("selected_string").attribute("update_rate").as_uint();

	//Standard life bar sizes
	bar_life_margin = node.child("standard_life_bar").attribute("maring").as_uint();
	iPoint standard_bar_life_sizes = { node.child("standard_life_bar").attribute("w").as_int(), node.child("standard_life_bar").attribute("h").as_int() };
	ally_bar_life_template.SetBox({ 0,0,standard_bar_life_sizes.x,standard_bar_life_sizes.y });
	enemy_bar_life_template.SetBox({ 0,0,standard_bar_life_sizes.x,standard_bar_life_sizes.y });
	
	//Standard life bar to empty rest value
	float rest_val = node.child("standard_life_bar").attribute("to_empty_rest_val").as_float();
	ally_bar_life_template.SetToEmptyRestValue(rest_val);
	enemy_bar_life_template.SetToEmptyRestValue(rest_val);

	//Ally life bar colors
	ally_bar_life_template.SetFullColor(TokenStrToColor(node.child("ally_bar_life").attribute("full").as_string()));
	ally_bar_life_template.SetToEmptyColor(TokenStrToColor(node.child("ally_bar_life").attribute("to_empty").as_string()));
	ally_bar_life_template.SetEmptyColor(TokenStrToColor(node.child("ally_bar_life").attribute("empty").as_string()));
	//Enemy life bar colors
	enemy_bar_life_template.SetFullColor(TokenStrToColor(node.child("enemy_bar_life").attribute("full").as_string()));
	enemy_bar_life_template.SetToEmptyColor(TokenStrToColor(node.child("enemy_bar_life").attribute("to_empty").as_string()));
	enemy_bar_life_template.SetEmptyColor(TokenStrToColor(node.child("enemy_bar_life").attribute("empty").as_string()));


	return true;
}

bool j1EntitiesManager::Update(float dt)
{
	bool ret = true;
	//Add all the entities generated in the last frame
	uint size = entities_to_add.size();
	for (uint k = 0; k < size; k++)
	{
		current_entities.push_back(entities_to_add[k]);
		LOG("%s added(%i)", entities_to_add[k]->GetName(),current_entities.size());
	}
	entities_to_add.clear();

	//Iterate all the entities to update them
	std::list<Entity*>::const_iterator list_item = current_entities.begin();
	while (list_item != current_entities.end())
	{
		ret = list_item._Ptr->_Myval->Update();
		list_item._Ptr->_Myval->Draw();
		list_item++;
	}

	//Iterate & delete all the entities ready to be cleared
	std::list<Entity*>::const_iterator clr_list_item = entitites_to_clear.begin();
	while (clr_list_item != entitites_to_clear.end())
	{
		current_entities.remove(clr_list_item._Ptr->_Myval);
		clr_list_item._Ptr->_Myval->Clean();
		entitites_to_delete.push_back(clr_list_item._Ptr->_Myval);
		clr_list_item++;
	}
	entitites_to_clear.clear();
	
	//Check if mouse is over an entity
	bool still_in = false;
	//Iterate all the current entities to check body rects
	std::list<Entity*>::const_iterator enti_item = current_entities.begin();

	if (entity_sel_timer.Read() > selected_update_rate)
	{
		//Get mouse position
		int x = 0, y = 0;
		App->input->GetMousePosition(x, y);
		SDL_Point mouse_coords = { x - App->render->camera.x,y - App->render->camera.y };

		while (enti_item != current_entities.end())
		{
			//Build the current entity box
			int b_x = 0, b_y = 0;
			enti_item._Ptr->_Myval->GetBody()->GetPosition(b_x, b_y);
			SDL_Rect cur_enti_body =
			{
				b_x - enti_item._Ptr->_Myval->GetBody()->width ,
				b_y - enti_item._Ptr->_Myval->GetBody()->height,
				enti_item._Ptr->_Myval->GetBody()->width * 2,
				enti_item._Ptr->_Myval->GetBody()->height * 2
			};

			//Check if the built box contacts with the mouse 
			if (SDL_PointInRect(&mouse_coords, &cur_enti_body))
			{
				still_in = bool(entity_selected == enti_item._Ptr->_Myval);

				//Get the correct font
				Font_Def* font = nullptr;
				if (enti_item._Ptr->_Myval->GetDiplomacy() == ENEMY)font = App->font->GetFontByID(FONT_ID::ENEMY_INF_FONT);
				else font = App->font->GetFontByID(FONT_ID::ALLY_INF_FONT);
				//Set the font
				entity_sel_string.SetFont(font->font);
				//Set the color
				entity_sel_string.SetColor(font->font_color);
				//Set the string & generate the texture
				entity_sel_string.SetString(enti_item._Ptr->_Myval->GenerateMouseInString());
				entity_sel_string.AdjustBox();
				entity_sel_string.SetVisualLayer(entities_ui_visual_layer);

				//Set the current entity selected
				entity_selected = enti_item._Ptr->_Myval;

				break;
			}

			enti_item++;
		}

		entity_sel_timer.Start();
	}

	if (enti_item == current_entities.end() && !still_in)
	{
		entity_sel_string.SetString("none", false);
	}
	else if (entity_selected != nullptr && entity_selected->GetBody() != nullptr)
	{
		int x = 0, y = 0;
		entity_selected->GetBody()->GetPosition(x, y);
		
		entity_sel_string.DrawAt(x - entity_sel_string.GetBox()->w * 0.5f + App->render->camera.x, y - entity_selected->GetBody()->height * 2 + App->render->camera.y - selected_str_margin);
	}

	

	return ret;
}

bool j1EntitiesManager::CleanUp()
{
	// Clean all the creatures definitions ------
	uint size = creatures_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(creatures_defs[k]);
	}
	creatures_defs.clear();

	// Clean all the creatures evolutions temp --
	size = creatures_evo_templates.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(creatures_evo_templates[k]);
	}
	creatures_evo_templates.clear();

	// Clean all the items definitions ------
	size = items_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(items_defs[k]);
	}
	items_defs.clear();

	//Iterate & delete all the entities ready to be deleted
	std::list<Entity*>::const_iterator del_list_item = entitites_to_delete.begin();
	while (del_list_item != entitites_to_delete.end())
	{
		RELEASE(del_list_item._Ptr->_Myval);
		del_list_item++;
	}
	entitites_to_delete.clear();
	entitites_to_clear.clear();

	// Clean all the current entities ----------
	std::list<Entity*>::iterator cur_entities_item = current_entities.begin();
	while (cur_entities_item != current_entities.end())
	{
		RELEASE(cur_entities_item._Ptr->_Myval);
		cur_entities_item++;
	}
	current_entities.clear();

	//Reset definitions doc
	definitions_doc.reset();

	return true;
}

void j1EntitiesManager::BeginCollision(PhysBody * A, PhysBody * B)
{
	if (A->entity_related == nullptr || B->entity_related == nullptr)return;
	
	if (strcmp(B->entity_related->GetName(), "Player") == 0)
	{
		if (strcmp(A->entity_related->GetName(), "Chest") == 0)
		{
			((Items_Tank*)A->entity_related)->SetReadyToDrop();
		}
		else if (strcmp(A->entity_related->GetName(), "Coin") == 0)
		{
			((Intelligent_Creature*)B->entity_related)->AddMoney(((Coin*)A->entity_related)->GetValue());
			App->entities_manager->ClearEntity(A->entity_related);
			LOG("Player Money: %i", ((Intelligent_Creature*)B->entity_related)->GetMoney());
		}
	}
}

void j1EntitiesManager::BeginSensorCollision(PhysBody * A, PhysBody * B)
{
	switch (A->body_type)
	{
	// NEUTRAL Sensors ------------------------------------
	case NEUTRAL_SENSOR_BODY:
	{
		if (B->body_type == PLAYER_BODY)
		{
			//NPC dialog action -----------------
			if (((Creature*)A->entity_related)->GetCreatureType() == CREATURE_TYPE::LORE_NPC_CREATURE && B->entity_related->worker.GetCurrentAction() == nullptr)
			{
				((NPC*)A->entity_related)->StartDialog((Player*)B->entity_related);
			}
		}
	}
		break;

	// ENEMY Sensors --------------------------------------
	case BODY_TYPE::ENEMY_SENSOR_BODY:
	{
		//ENEMY ATK action ----------------------
		if (B->body_type == PLAYER_BODY)
		{
			Action* act = A->entity_related->worker.GenerateBasicAttackAction(A->entity_related, (Creature*)B->entity_related);
			A->entity_related->worker.AddAction(act);
		}
	}
		break;
	}
}

// Functionality ================================
void j1EntitiesManager::AddItemDefinition(const pugi::xml_node * data_node)
{
	//General pointer to the new undefined item
	Item* new_item = nullptr; 
	//Get the new item type
	ITEM_TYPE item_type = StrToItemType(data_node->attribute("item_type").as_string());
	//Allocate the correct class checking the item type
	switch (item_type)
	{
	case COIN_ITEM:			new_item = new Coin();			break;
	case BASIC_BOX_ITEM:	new_item = new Items_Tank();	break;
	}
	
	//Load the new item body data
	PhysBody* new_body = new PhysBody();
	new_body->body_def = new PhysBodyDef();

	/*Width*/			new_body->body_def->width = data_node->attribute("width").as_int();
	/*Height*/			new_body->body_def->height = data_node->attribute("height").as_int();
	/*Shape*/			new_body->body_def->shape_type = App->physics->StrToBodyShape(data_node->attribute("body_shape").as_string());
	/*Interact.Type*/	new_body->body_def->body_interaction_type = App->physics->StrToInteractionType(data_node->attribute("interaction_type").as_string());
	/*Collision.Type*/	new_body->body_def->collision_type = App->physics->StrToCollisionType(data_node->attribute("collision_type").as_string());
	/*Body.Type*/		new_body->body_def->body_type = App->physics->StrToBodyType(data_node->attribute("body_type").as_string());
	/*Restitution*/		new_body->body_def->restitution = data_node->attribute("restitution").as_float();
	/*Listener*/		new_body->body_def->listener = App->GetModule(data_node->attribute("listener").as_string());
	/*Friction*/		new_body->body_def->friction = data_node->attribute("friction").as_float();
	/*Mass*/			new_body->body_def->density = data_node->attribute("density").as_float();
	/*Fixed Rotation*/	new_body->body_def->fixed_rotation = data_node->attribute("fixed_rotation").as_bool();

	//Set the generated body at the new item
	new_item->SetBody(new_body);

	//Set new item general stats
	/*Entity Type*/		new_item->SetEntityType(ITEM);
	/*Diplomacy*/		new_item->SetDiplomacy(StrToDiplomacy(data_node->attribute("diplomacy").as_string()));
	/*Item Type*/		new_item->SetItemType(item_type);
	/*Volatile*/		if (data_node->attribute("volatile").as_bool())new_item->SetAsVolatile();
	/*Name*/			new_item->SetName(data_node->attribute("name").as_string());
	/*Description*/		new_item->SetDescription(data_node->attribute("description").as_string());

	//Set new item specific stats
	switch (item_type)
	{
	case COIN_ITEM:

		/*Value*/		((Coin*)new_item)->SetValue(data_node->attribute("value").as_uint());
		/*Delete Time*/	((Coin*)new_item)->SetTimeToDelete(data_node->attribute("time_to_delete").as_uint());

		break;

	case BASIC_BOX_ITEM:

		/*Drop Imp*/	((Items_Tank*)new_item)->SetDropImpulse(data_node->attribute("drop_impulse").as_float());
		/*Drop T Rad*/	((Items_Tank*)new_item)->SetDropTotalRad(data_node->attribute("drop_total_rad").as_float());
		
		break;

	}

	//Add the built item definition at the items definitions vector
	items_defs.push_back(new_item);
}

void j1EntitiesManager::AddCreatureDefinition(const pugi::xml_node* data_node)
{
	//Allocate the new creature
	Creature* new_creature = nullptr;
	//Get the new item type
	CREATURE_TYPE creature_type = StrToCreatureType(data_node->attribute("creature_type").as_string());
	
	//Error alert
	if (creature_type == NO_CREATURE)
	{
		LOG("ERROR: Creature Gen NO_CREATURE");
		return;
	}

	//Allocate the correct class checking the creature type
	switch (creature_type)
	{
	case PLAYER_CREATURE:
							new_creature = new Player();				break;
	case LORE_NPC_CREATURE:
	case LORE_NPC_B_CREATURE:
							new_creature = new NPC();					break;
	case BASIC_ENEMY_CREATURE:
							new_creature = new Intelligent_Creature();	break;
	case STANDARD_NPC_CREATURE:
	case NO_CREATURE:		
							new_creature = new Creature();				break;
	}

	//Load the new creature body data
	PhysBody* new_body = new PhysBody();
	new_body->body_def = new PhysBodyDef();

	/*Width*/			new_body->body_def->width = data_node->attribute("width").as_int();
	/*Height*/			new_body->body_def->height = data_node->attribute("height").as_int();
	/*Shape*/			new_body->body_def->shape_type = App->physics->StrToBodyShape(data_node->attribute("body_shape").as_string());
	/*Interact.Type*/	new_body->body_def->body_interaction_type = App->physics->StrToInteractionType(data_node->attribute("interaction_type").as_string());
	/*Collision.Type*/	new_body->body_def->collision_type = App->physics->StrToCollisionType(data_node->attribute("collision_type").as_string());
	/*Body.Type*/		new_body->body_def->body_type = App->physics->StrToBodyType(data_node->attribute("body_type").as_string());
	/*Restitution*/		new_body->body_def->restitution = data_node->attribute("restitution").as_float();
	/*Listener*/		new_body->body_def->listener = App->GetModule(data_node->attribute("listener").as_string());
	/*Friction*/		new_body->body_def->friction = data_node->attribute("friction").as_float();
	/*Mass*/			new_body->body_def->density = data_node->attribute("density").as_float();
	/*Fixed Rotation*/	new_body->body_def->fixed_rotation = data_node->attribute("fixed_rotation").as_bool();

	//Set the generated body at the new creature
	new_creature->SetBody(new_body);

	//Set the new creature general states
	/*Entity Type*/		new_creature->SetEntityType(CREATURE);
	/*Diplomacy*/		new_creature->SetDiplomacy(StrToDiplomacy(data_node->attribute("diplomacy").as_string()));
	/*Creature Type*/	new_creature->SetCreatureType(creature_type);
	/*Name*/			new_creature->SetName(data_node->attribute("name").as_string());
	/*Description*/		new_creature->SetDescription(data_node->attribute("description").as_string());
	/*Life*/			new_creature->SetMaxLife(data_node->attribute("life").as_uint());
						new_creature->SetCurrentLife(new_creature->GetMaxLife());
	/*AttackHitPoints*/	new_creature->SetAttackHitPoints(data_node->attribute("attack_hitpoints").as_uint());
	/*AttackRate*/		new_creature->SetAttackRate(data_node->attribute("attack_rate").as_uint());
	/*Mov Speed*/		new_creature->SetMovSpeed(data_node->attribute("mov_speed").as_float());
	/*Jump Force*/		new_creature->SetJumpForce(data_node->attribute("jump_force").as_float());
	/*Money*/			new_creature->SetMoney(data_node->attribute("money").as_uint(0));
	/*Reward Exp*/		new_creature->SetRewardExperience(data_node->attribute("reward_experience").as_uint(0));
	
	//Build life bar
	UI_Progressive_Bar* life_bar = new_creature->GetLifeBar();

	//Get the correct template
	if (new_creature->GetDiplomacy() == ENEMY)
	{
		*life_bar = enemy_bar_life_template;
	}
	else
	{
		*life_bar = ally_bar_life_template;
	}
	/*Values*/			life_bar->SetMaxValue(new_creature->GetMaxLife());
						life_bar->SetCurrentValue(new_creature->GetMaxLife());
	/*Visual Layer*/	life_bar->SetVisualLayer(entities_ui_visual_layer);
	/*No Use Cam*/		life_bar->SetUseCamera(false);

	//Set new creature specific stats
	switch (creature_type)
	{
	case PLAYER_CREATURE:
	case LORE_NPC_CREATURE:
	case LORE_NPC_B_CREATURE:
	case BASIC_ENEMY_CREATURE:
	{
		//Load the new creature vision area
		PhysBody* new_vision_area = new PhysBody();
		new_vision_area->body_def = new PhysBodyDef();

		/*Radius*/		new_vision_area->body_def->width = new_vision_area->body_def->height = data_node->attribute("vision_range").as_uint();
		/*Shape*/		new_vision_area->body_def->shape_type = b2Shape::Type::e_circle;
		/*Sensor*/		new_vision_area->body_def->is_sensor = true;
		/*Listener*/	new_vision_area->body_def->listener = App->GetModule(data_node->attribute("listener").as_string());
		/*Collision*/	new_vision_area->body_def->collision_type = App->physics->StrToCollisionType(data_node->attribute("vision_sensor_type").as_string());
		/*Body.Type*/	new_vision_area->body_def->body_type = App->physics->StrToBodyType(data_node->attribute("vision_sensor_body").as_string());

		//Set the generated vision are at the new creature
		((Intelligent_Creature*)new_creature)->SetVisionArea(new_vision_area);

	}
		break;

	case STANDARD_NPC_CREATURE:
		break;
	}

	if (creature_type == LORE_NPC_CREATURE || creature_type == LORE_NPC_B_CREATURE)
	{
		//Load NPC dialogs
		/*Font/Size*/	Font_Def* d_font = App->font->GetFontByID(App->font->StrToFontID(data_node->attribute("font").as_string()));
		/*Font Color*/	SDL_Color f_color = App->entities_manager->TokenStrToColor(data_node->attribute("f_color").as_string());
		/*Back*/		bool back = data_node->attribute("back").as_bool();
		/*Back Color*/	SDL_Color back_color = App->entities_manager->TokenStrToColor(data_node->attribute("b_color").as_string("0/0/0/0"));
		
		pugi::xml_attribute dialog_atr = data_node->attribute("d_0");
		while (dialog_atr != NULL)
		{
			//Build the string with the loaded data
			UI_String* new_str = new UI_String();
			new_str->SetFont(d_font->font);
			new_str->SetColor(f_color);
			new_str->SetBack(back);
			new_str->SetBackColor(back_color);
			new_str->SetBackMargins({ data_node->attribute("margin_x").as_int(),data_node->attribute("margin_y").as_int() });
			new_str->SetString(dialog_atr.as_string("str_error"));
			new_str->AdjustBox();

			//Add the generated ui_str
			((NPC*)new_creature)->AddDialogStr(new_str);
			
			//Focus the next attribute
			dialog_atr = dialog_atr.next_attribute();
		}
	}

	if (creature_type == PLAYER_CREATURE)
	{
		/*Attack range*/	((Player*)new_creature)->SetAttackRange(data_node->attribute("attack_range").as_uint());
		
		//Load the player attack area
		/*Attack Area*/		PhysBody* atk_area = new PhysBody();
							atk_area->body_def = new PhysBodyDef();

		/*Width*/			atk_area->body_def->width = data_node->attribute("attack_range").as_uint();
		/*Height*/			atk_area->body_def->height = atk_area->body_def->width * 2;
		/*Shape*/			atk_area->body_def->shape_type = b2Shape::Type::e_polygon;
		/*Sensor*/			atk_area->body_def->is_sensor = true;
		/*Listener*/		atk_area->body_def->listener = App->GetModule(data_node->attribute("listener").as_string());
		/*Collision*/		atk_area->body_def->collision_type = App->physics->StrToCollisionType(data_node->attribute("attack_sensor_type").as_string());
		/*Body.Type*/		atk_area->body_def->body_type = App->physics->StrToBodyType(data_node->attribute("attack_sensor_body").as_string());

		//Set the generated attack area at the new player
		((Player*)new_creature)->SetCurrentAttackArea(atk_area);

		/*Exp Scale*/		((Player*)new_creature)->SetExperienceScale(data_node->attribute("experience_scale").as_float());
		/*Next Lvl Exp*/	((Player*)new_creature)->SetNextLevelExperience(data_node->attribute("next_lvl_experience").as_uint());
	}

	//Add the built creature at the definitions vector
	creatures_defs.push_back(new_creature);
}

void j1EntitiesManager::AddCreatureEvoTemplate(const pugi::xml_node * data_node)
{
	//Allocate the new evolution template
	Evolution_Template* new_evo_tmp = new Evolution_Template();

	//Set all the evolution stats from the data node
	/*Creature type*/	new_evo_tmp->creature_type = StrToCreatureType(data_node->attribute("creature_type").as_string());
	/*Life*/			new_evo_tmp->life = data_node->attribute("life").as_uint();
	/*Attack Points*/	new_evo_tmp->attack_hitpoints = data_node->attribute("attack_hitpoints").as_uint();
	/*Money*/			new_evo_tmp->money = data_node->attribute("money").as_uint();

	//Add the generated template to the template vector
	creatures_evo_templates.push_back(new_evo_tmp);
}

//Enums Methods =================================
ENTITY_TYPE j1EntitiesManager::StrToEntityType(const char * str) const
{
	if (strcmp(str, "item") == 0)					return ITEM;
	if (strcmp(str, "creature") == 0)				return CREATURE;
	if (strcmp(str, "creature_evo_template") == 0)	return CREATURE_EVO_TEMPLATE;
	return NO_ENTITY;
}

DIPLOMACY j1EntitiesManager::StrToDiplomacy(const char * str) const
{
	if (strcmp(str, "neutral") == 0)	return NEUTRAL;
	if (strcmp(str, "ally") == 0)		return ALLY;
	if (strcmp(str, "enemy") == 0)		return ENEMY;
	return NO_DIPLOMACY;
}

CREATURE_TYPE j1EntitiesManager::StrToCreatureType(const char * str) const
{
	if (strcmp(str, "player_creature") == 0)		return PLAYER_CREATURE;
	if (strcmp(str, "standard_npc_creature") == 0)	return STANDARD_NPC_CREATURE;
	if (strcmp(str, "lore_npc_creature") == 0)		return LORE_NPC_CREATURE;
	if (strcmp(str, "lore_npc_b_creature") == 0)	return LORE_NPC_B_CREATURE;
	if (strcmp(str, "basic_enemy_creature") == 0)	return BASIC_ENEMY_CREATURE;
	return NO_CREATURE;
}

ITEM_TYPE j1EntitiesManager::StrToItemType(const char * str) const
{
	if (strcmp(str, "basic_box_item") == 0)	return BASIC_BOX_ITEM;
	if (strcmp(str, "coin_item") == 0)		return COIN_ITEM;
	return NO_ITEM;
}

DIRECTION j1EntitiesManager::StrToDirection(const char * str) const
{
	if (strcmp(str, "left") == 0)		return LEFT;
	if (strcmp(str, "right") == 0)		return RIGHT;
	return NO_DIRECTION;
}

ACTION_TYPE j1EntitiesManager::StrToActionType(const char * str) const
{
	if (strcmp(str, "attack_action") == 0)		return ATTACK_ACTION;
	if (strcmp(str, "walk_action") == 0)		return WALK_ACTION;
	if (strcmp(str, "disappear_action") == 0)	return DISAPPEAR_ACTION;
	if (strcmp(str, "die_action") == 0)			return DIE_ACTION;
	if (strcmp(str, "idle_action") == 0)		return IDLE_ACTION;
	return NO_ACTION;
}

std::vector<ITEM_TYPE> j1EntitiesManager::TokenStrToItemTypes(const char * str) const
{
	std::vector<ITEM_TYPE> types_vec;

	std::string copy = str;
	char* token = strtok((char*)copy.c_str(), "/");

	while (token != NULL)
	{
		types_vec.push_back(App->entities_manager->StrToItemType(token));
		token = strtok(NULL, "/");
	}

	return types_vec;
}

SDL_Color j1EntitiesManager::TokenStrToColor(const char * str) const
{
	SDL_Color res = { 255,255,255,255 };

	std::vector<Uint8> values_vec;

	std::string copy = str;
	char* token = strtok((char*)copy.c_str(), "/");

	while (token != NULL)
	{
		values_vec.push_back(atoi(token));
		token = strtok(NULL, "/");
	}
	
	if(values_vec.size() == 4)res = { values_vec[0],values_vec[1],values_vec[2],values_vec[3] };

	return res;
}

//Functionality =================================
Creature * j1EntitiesManager::GenerateCreature(CREATURE_TYPE creature_type, bool generate_body)
{
	Creature* new_creature = nullptr;

	//Iterate the creatures definitions and copy the correct def
	uint size = creatures_defs.size();
	for (uint k = 0; k < size; k++)
	{
		if (creatures_defs[k]->GetCreatureType() == creature_type)
		{
			switch (creature_type)
			{
			case LORE_NPC_CREATURE:
			case LORE_NPC_B_CREATURE:
				new_creature = new NPC(*(NPC*)creatures_defs[k], generate_body);
				break;
			case PLAYER_CREATURE:
				new_creature = new Player(*(Player*)creatures_defs[k], generate_body);
				break;
			case BASIC_ENEMY_CREATURE:
				new_creature = new Intelligent_Creature(*(Intelligent_Creature*)creatures_defs[k], generate_body);
				break;
			default:
				new_creature = new Creature(*creatures_defs[k], generate_body);
				break;
			}			
			break;
		}
	}

	// Add  the generated creature at the current creatures list
	if (generate_body)
	{
		//Add the entity to the entity manager
		AddEntity(new_creature);
		//Play entity animation
		App->animator->EntityPlay(new_creature);
		//Active life bar if is an enemy
		if (new_creature->GetDiplomacy() == ENEMY)
		{
			new_creature->GetLifeBar()->GenerateTexture();
		}
	}

	return new_creature;
}

Item* j1EntitiesManager::GenerateItem(ITEM_TYPE item_type, bool generate_body)
{
	Item* new_item = nullptr;

	//Iterate the creatures definitions and copy the correct def
	uint size = items_defs.size();
	for (uint k = 0; k < size; k++)
	{
		if (items_defs[k]->GetItemType() == item_type)
		{
			switch (item_type)
			{
			case COIN_ITEM:			new_item = new Coin(*(Coin*)items_defs[k], generate_body);						break;
			case BASIC_BOX_ITEM:	new_item = new Items_Tank(*(Items_Tank*)items_defs[k], generate_body);			break;
			default:				new_item = new Item(*items_defs[k], generate_body);								break;
			}

			if(generate_body)new_item->GetBody()->entity_related = new_item;
			
			break;
		}
	}

	// Add  the generated creature at the current creatures list
	if (generate_body)
	{
		AddEntity(new_item);
		App->animator->EntityPlay(new_item);
	}

	return new_item;
}

bool j1EntitiesManager::LevelUpCreature(Creature * target)
{
	//Evolution template pointer
	Evolution_Template* evo_template = nullptr;

	//Search the correct evolution template
	uint size = creatures_evo_templates.size();
	for (uint k = 0; k < size; k++)
	{
		if (creatures_evo_templates[k]->creature_type == target->GetCreatureType())
		{
			evo_template = creatures_evo_templates[k];
			break;
		}
	}

	//If the template is found we can apply it
	if (evo_template != nullptr)
	{
		/*Life*/			target->SetMaxLife(target->GetMaxLife() + evo_template->life);
							target->SetCurrentLife(target->GetCurrentLife());
		/*Attack Points*/	target->SetAttackHitPoints(target->GetAttackHitPoints() + evo_template->attack_hitpoints);
		/*Money*/			target->AddMoney(evo_template->money);
	}

	return bool(evo_template != nullptr);
}

void j1EntitiesManager::AddEntity(const Entity * target)
{
	entities_to_add.push_back((Entity*)target);
}

void j1EntitiesManager::ClearEntity(Entity * target)
{
	entitites_to_clear.remove(target);
	entitites_to_clear.push_back(target);
}

void j1EntitiesManager::DeleteEntity(Entity* target)
{
	entitites_to_delete.remove(target);
	entitites_to_delete.push_back(target);
}

void j1EntitiesManager::DeleteCurrentEntities()
{
	//Iterate & delete all the entities ready to be deleted
	std::list<Entity*>::const_iterator del_list_item = entitites_to_delete.begin();
	while (del_list_item != entitites_to_delete.end())
	{
		current_entities.remove(del_list_item._Ptr->_Myval);
		RELEASE(del_list_item._Ptr->_Myval);
		del_list_item++;
	}
	entitites_to_delete.clear();

	// Clean all the current entities ----------
	std::list<Entity*>::iterator cur_entities_item = current_entities.begin();
	while (cur_entities_item != current_entities.end())
	{
		RELEASE(cur_entities_item._Ptr->_Myval);
		cur_entities_item++;
	}
	current_entities.clear();
}
