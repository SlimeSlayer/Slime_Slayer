#include "j1EntitiesManager.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Physics.h"
#include "j1Animator.h"
#include "j1FileSystem.h"

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
		delete creatures_defs[k];
	}
	creatures_defs.clear();

	// Clean all the items definitions ----------
	size = items_defs.size();
	for (uint k = 0; k < size; k++)
	{
		delete items_defs[k];
	}
	items_defs.clear();

	// Clean all the current entities -----------
	std::list<Entity*>::iterator cur_entities_item = current_entities.begin();
	while (cur_entities_item != current_entities.end())
	{
		delete cur_entities_item._Ptr->_Myval;
		cur_entities_item++;
	}
	current_entities.clear();

	enabled = active = false;
}

bool j1EntitiesManager::Awake(pugi::xml_node & node)
{
	LOG("Loading Entities Definitions Doc");
	App->fs->LoadXML(node.child("entities_data").attribute("file").as_string(),&definitions_doc);
	if (definitions_doc == NULL)
	{
		LOG("Error Loading Entities Doc");
	}

	return true;
}

bool j1EntitiesManager::Update(float dt)
{
	bool ret = true;

	std::list<Entity*>::const_iterator list_item = current_entities.begin();
	while (list_item != current_entities.end())
	{
		ret = list_item._Ptr->_Myval->Update();
		list_item++;
	}

	uint size = entitites_to_delete.size();
	for (uint k = 0; k < size; k++)
	{
		delete entitites_to_delete[k];
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
		delete creatures_defs[k];
	}
	creatures_defs.clear();

	// Clean all the items definitions ------
	size = items_defs.size();
	for (uint k = 0; k < size; k++)
	{
		delete items_defs[k];
	}
	items_defs.clear();

	// Clean all the current entitites ----------
	std::list<Entity*>::iterator cur_entities_item = current_entities.begin();
	while (cur_entities_item != current_entities.end())
	{
		delete cur_entities_item._Ptr->_Myval;
		cur_entities_item++;
	}
	current_entities.clear();

	
	definitions_doc.reset();

	return true;
}

void j1EntitiesManager::OnCollision(PhysBody * A, PhysBody * B)
{
	if (A->entity_related == nullptr || B->entity_related == nullptr)return;
	
	if (strcmp(A->entity_related->GetName(),"Jar") == 0 && strcmp(B->entity_related->GetName(),"Player") == 0)
	{
		((Items_Tank*)A->entity_related)->SetReadyToDrop();
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
	PhysBody* new_body = nullptr;
	new_body = App->physics->CreateRectangleDef(
		data_node->attribute("width").as_int(),
		data_node->attribute("height").as_int(),
		App->physics->StrToBodyShape(data_node->attribute("body_shape").as_string()),
		App->physics->StrToInteractionType(data_node->attribute("interaction_type").as_string()),
		App->physics->StrToCollisionType(data_node->attribute("collision_type").as_string()),
		App->physics->StrToBodyType(data_node->attribute("body_type").as_string()),
		data_node->attribute("restitution").as_float(),
		App->GetModule(data_node->attribute("listener").as_string())
	);

	new_item->SetBody(new_body);

	//Set new item general stats
	/*Entity Type*/		new_item->SetEntityType(ITEM);
	/*Item Type*/		new_item->SetItemType(item_type);
	/*Name*/			new_item->SetName(data_node->attribute("name").as_string());
	/*Description*/		new_item->SetDescription(data_node->attribute("description").as_string());

	//Set new item specific stats
	if (item_type == COIN_ITEM)
	{
		/*Value*/	((Coin*)new_item)->SetValue(data_node->attribute("value").as_uint());
	}

	//Add the built item definition at the items definitions vector
	items_defs.push_back(new_item);
}

void j1EntitiesManager::AddCreatureDefinition(const pugi::xml_node* data_node)
{
	//Allocate the new creature
	Creature* new_creature = new Creature();
	
	//Load the new creature body data
	PhysBody* new_body = nullptr;
	new_body = App->physics->CreateRectangleDef(
		data_node->attribute("width").as_int(),
		data_node->attribute("height").as_int(),
		App->physics->StrToBodyShape(data_node->attribute("body_shape").as_string()),
		App->physics->StrToInteractionType(data_node->attribute("interaction_type").as_string()),
		App->physics->StrToCollisionType(data_node->attribute("collision_type").as_string()),
		App->physics->StrToBodyType(data_node->attribute("body_type").as_string()),
		data_node->attribute("restitution").as_float(),
		App->GetModule(data_node->attribute("listener").as_string())
												);

	new_creature->SetBody(new_body);

	//Load the new creature states
	/*Entity Type*/		new_creature->SetEntityType(CREATURE);
	/*Creature Type*/	new_creature->SetCreatureType(App->entities_manager->StrToCreatureType(data_node->attribute("creature_type").as_string()));
	/*Name*/			new_creature->SetName(data_node->attribute("name").as_string());
	/*Description*/		new_creature->SetDescription(data_node->attribute("description").as_string());
	/*Life*/			new_creature->SetLife(data_node->attribute("life").as_uint());
	/*Attack*/			new_creature->SetAttack(data_node->attribute("attack").as_uint());
	/*Mov Speed*/		new_creature->SetMovSpeed(data_node->attribute("mov_speed").as_float());
	/*Jump Force*/		new_creature->SetJumpForce(data_node->attribute("jump_force").as_float());

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
	if (strcmp(str, "player_creature") == 0)	return PLAYER_CREATURE;
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
			new_creature = new Creature(*creatures_defs[k], generate_body);
			new_creature->GetBody()->entity_related = new_creature;
			break;
		}
	}

	// Add  the generated creature at the current creatures list
	current_entities.push_back(new_creature);

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
			}
			if(generate_body)new_item->GetBody()->entity_related = new_item;
			
			break;
		}
	}

	// Add  the generated creature at the current creatures list
	if(generate_body)current_entities.push_back(new_item);

	return new_item;
}

void j1EntitiesManager::AddEntity(const Entity * target)
{
	current_entities.push_back((Entity*)target);
}

void j1EntitiesManager::DeleteEntity(Entity * target)
{
	current_entities.remove(target);
	entitites_to_delete.push_back(target);
}
