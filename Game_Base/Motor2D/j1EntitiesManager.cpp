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
			case ITEM:	break;
			case CREATURE:	AddCreatureDefinition(&current_enble_node); break;
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

	// Clean all the current creatures ----------
	std::list<Creature*>::iterator cur_creatures_item = current_creatures.begin();
	while (cur_creatures_item != current_creatures.end())
	{
		delete cur_creatures_item._Ptr->_Myval;
		cur_creatures_item++;
	}
	current_creatures.clear();

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
	return true;
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

	// Clean all the current creatures ----------
	std::list<Creature*>::iterator cur_creatures_item = current_creatures.begin();
	while (cur_creatures_item != current_creatures.end())
	{
		delete cur_creatures_item._Ptr->_Myval;
		cur_creatures_item++;
	}
	current_creatures.clear();

	
	definitions_doc.reset();

	return true;
}

// Functionality ================================
void j1EntitiesManager::AddCreatureDefinition(const pugi::xml_node* data_node)
{
	//Allocate the new creature
	Creature* new_creature = new Creature();
	
	//Load the new creature body data
	PhysBody* new_body = nullptr;
	b2Shape::Type body_shape = App->physics->StrToBodyShape(data_node->attribute("body_shape").as_string());
	switch (body_shape)
	{
	case b2Shape::e_circle:
		break;

	case b2Shape::e_polygon:
		new_body = App->physics->CreateRectangle(	0,
													0,
													data_node->attribute("width").as_int(),
													data_node->attribute("height").as_int(),
													App->physics->StrToCollisionType(data_node->attribute("collision_type").as_string()),
													App->physics->StrToBodyType(data_node->attribute("body_type").as_string())
												);
		new_body->listener = App->GetModule(data_node->attribute("listener").as_string());
		new_creature->SetBody(new_body);
		break;

	case b2Shape::e_chain:
		break;
	}

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

Creature * j1EntitiesManager::GenerateCreature(CREATURE_TYPE creature_type)
{
	Creature* new_creature = nullptr;

	//Iterate the creatures definitions and copy the correct def
	uint size = creatures_defs.size();
	for (uint k = 0; k < size; k++)
	{
		if (creatures_defs[k]->GetCreatureType() == creature_type)
		{
			new_creature = new Creature(*creatures_defs[k]);
			break;
		}
	}

	// Add  the generated creature at the current creatures list
	current_creatures.push_back(new_creature);

	return new_creature;
}
