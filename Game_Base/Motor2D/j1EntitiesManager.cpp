#include "j1EntitiesManager.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Physics.h"
#include "j1Animator.h"
#include "j1FileSystem.h"
#include "j1Fonts.h"
#include "j1Gui.h"

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
	active = false;
	enabled = false;
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
			case ITEM:		AddItemDefinition(&current_enble_node);			break;
			case CREATURE:	AddCreatureDefinition(&current_enble_node);		break;
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

	enabled = active = true;

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
	}

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
		list_item++;
	}

	//Iterate & delete all the entities ready to be deleted
	std::list<Entity*>::const_iterator del_list_item = entitites_to_delete.begin();
	while (del_list_item != entitites_to_delete.end())
	{
		current_entities.remove(del_list_item._Ptr->_Myval);
		RELEASE(del_list_item._Ptr->_Myval);
		del_list_item++;
	}
	entitites_to_delete.clear();
	
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

	
	definitions_doc.reset();

	return true;
}

void j1EntitiesManager::BeginCollision(PhysBody * A, PhysBody * B)
{
	if (A->entity_related == nullptr || B->entity_related == nullptr)return;
	
	if (strcmp(B->entity_related->GetName(), "Player") == 0)
	{
		if (strcmp(A->entity_related->GetName(), "Jar") == 0)
		{
			((Items_Tank*)A->entity_related)->SetReadyToDrop();
		}
		else if (strcmp(A->entity_related->GetName(), "Coin") == 0)
		{
			((Intelligent_Creature*)B->entity_related)->AddMoney(((Coin*)A->entity_related)->GetValue());
			App->entities_manager->DeleteEntity(A->entity_related);
			LOG("Player Money: %i", ((Intelligent_Creature*)B->entity_related)->GetMoney());
		}
	}
}

void j1EntitiesManager::BeginSensorCollision(PhysBody * A, PhysBody * B)
{
	switch (A->body_type)
	{
	case NPC_BODY:
		if (B->body_type == PLAYER_BODY)
		{
			if (((Creature*)A->entity_related)->GetCreatureType() == CREATURE_TYPE::LORE_NPC_CREATURE && A->entity_related->GetWorker()->GetCurrentAction() == nullptr)
			{
				((NPC*)A->entity_related)->StartDialog((Player*)B->entity_related);
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
	case COIN_ITEM:	new_item = new Coin();			break;
	case JAR_ITEM:	new_item = new Items_Tank();	break;
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
	/*Item Type*/		new_item->SetItemType(item_type);
	/*Name*/			new_item->SetName(data_node->attribute("name").as_string());
	/*Description*/		new_item->SetDescription(data_node->attribute("description").as_string());

	//Set new item specific stats
	if (item_type == COIN_ITEM)
	{
		/*Value*/		((Coin*)new_item)->SetValue(data_node->attribute("value").as_uint());
		/*Delete Time*/	((Coin*)new_item)->SetTimeToDelete(data_node->attribute("time_to_delete").as_uint());
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
	//Allocate the correct class checking the creature type
	switch (creature_type)
	{
	case PLAYER_CREATURE:
							new_creature = new Player();	break;
	case LORE_NPC_CREATURE:
	case LORE_NPC_B_CREATURE:
							new_creature = new NPC();					break;
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
	/*Creature Type*/	new_creature->SetCreatureType(creature_type);
	/*Name*/			new_creature->SetName(data_node->attribute("name").as_string());
	/*Description*/		new_creature->SetDescription(data_node->attribute("description").as_string());
	/*Life*/			new_creature->SetLife(data_node->attribute("life").as_uint());
	/*Attack*/			new_creature->SetAttack(data_node->attribute("attack").as_uint());
	/*Mov Speed*/		new_creature->SetMovSpeed(data_node->attribute("mov_speed").as_float());
	/*Jump Force*/		new_creature->SetJumpForce(data_node->attribute("jump_force").as_float());

	//Set new creature specific stats
	if (creature_type == PLAYER_CREATURE || creature_type == LORE_NPC_CREATURE || creature_type == LORE_NPC_B_CREATURE)
	{
		//Load the new creature vision area
		PhysBody* new_vision_area = new PhysBody();
		new_vision_area->body_def = new PhysBodyDef();
		
		/*Radius*/		new_vision_area->body_def->width = new_vision_area->body_def->height = data_node->attribute("vision_range").as_uint();
		/*Shape*/		new_vision_area->body_def->shape_type = b2Shape::Type::e_circle;
		/*Sensor*/		new_vision_area->body_def->is_sensor = true;
		/*Listener*/	new_vision_area->body_def->listener = App->GetModule(data_node->attribute("listener").as_string());
		/*Collision*/	new_vision_area->body_def->collision_type = App->physics->StrToCollisionType(data_node->attribute("vision_sensor_type").as_string());
		/*Body.Type*/	new_vision_area->body_def->body_type = App->physics->StrToBodyType(data_node->attribute("body_type").as_string());
		
		//Set the generated vision are at the new creature
		((Intelligent_Creature*)new_creature)->SetVisionArea(new_vision_area);

		/*Money*/	((Intelligent_Creature*)new_creature)->SetMoney(data_node->attribute("money").as_uint(0));
	}
	if (creature_type == LORE_NPC_CREATURE || creature_type == LORE_NPC_B_CREATURE)
	{
		//Load NPC dialogs
		/*Font/Size*/	_TTF_Font* d_font = App->font->GetFontByIndex(data_node->attribute("font").as_int(0));
		/*Color*/		SDL_Color d_color = { 0,0,0,255 };
		/*R*/			d_color.r = data_node->attribute("t_color_r").as_int();
		/*G*/			d_color.g = data_node->attribute("t_color_g").as_int();
		/*B*/			d_color.b = data_node->attribute("t_color_b").as_int();

		pugi::xml_attribute dialog_atr = data_node->attribute("d_0");
		while (dialog_atr != NULL)
		{
			//Build the string with the loaded data
			UI_String* new_str = new UI_String();
			new_str->SetFont(d_font);
			new_str->SetColor(d_color);
			new_str->SetString(dialog_atr.as_string("str_error"));
			
			//Add the generated ui_str
			((NPC*)new_creature)->AddDialogStr(new_str);
			
			//Focus the next attribute
			dialog_atr = dialog_atr.next_attribute();
		}
	}
	//Add the built creature at the definitions vector
	creatures_defs.push_back(new_creature);
}

//Enums Methods =================================
ENTITY_TYPE j1EntitiesManager::StrToEntityType(const char * str) const
{
	if (strcmp(str, "item") == 0)		return ITEM;
	if (strcmp(str, "creature") == 0)	return CREATURE;
	return NO_ENTITY;
}

CREATURE_TYPE j1EntitiesManager::StrToCreatureType(const char * str) const
{
	if (strcmp(str, "player_creature") == 0)		return PLAYER_CREATURE;
	if (strcmp(str, "standard_npc_creature") == 0)	return STANDARD_NPC_CREATURE;
	if (strcmp(str, "lore_npc_creature") == 0)		return LORE_NPC_CREATURE;
	if (strcmp(str, "lore_npc_b_creature") == 0)	return LORE_NPC_B_CREATURE;
	return NO_CREATURE;
}

ITEM_TYPE j1EntitiesManager::StrToItemType(const char * str) const
{
	if (strcmp(str, "jar_item") == 0)	return JAR_ITEM;
	if (strcmp(str, "coin_item") == 0)	return COIN_ITEM;
	return NO_ITEM;
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
		AddEntity(new_creature);
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
			case COIN_ITEM:
				new_item = new Coin(*(Coin*)items_defs[k], generate_body);
				break;
			case JAR_ITEM:
				new_item = new Items_Tank(*(Items_Tank*)items_defs[k], generate_body);
				((Items_Tank*)new_item)->AddItem(GenerateItem(COIN_ITEM, false));
				((Items_Tank*)new_item)->AddItem(GenerateItem(COIN_ITEM, false));
				((Items_Tank*)new_item)->AddItem(GenerateItem(COIN_ITEM, false));
				((Items_Tank*)new_item)->AddItem(GenerateItem(COIN_ITEM, false));
				((Items_Tank*)new_item)->AddItem(GenerateItem(COIN_ITEM, false));
				break;
			default:
				new_item = new Item(*items_defs[k], generate_body);
				break;
			}

			if(generate_body)new_item->GetBody()->entity_related = new_item;
			
			break;
		}
	}

	// Add  the generated creature at the current creatures list
	if (generate_body)
	{
		AddEntity(new_item);
	}

	return new_item;
}

void j1EntitiesManager::AddEntity(const Entity * target)
{
	entities_to_add.push_back((Entity*)target);
}

void j1EntitiesManager::DeleteEntity(Entity * target)
{
	current_entities.remove(target);
	entitites_to_delete.remove(target);
	entitites_to_delete.push_back(target);
}
