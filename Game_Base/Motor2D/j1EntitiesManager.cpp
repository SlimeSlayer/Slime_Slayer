#include "j1EntitiesManager.h"

#include "j1App.h"
#include "j1Physics.h"
#include "j1Animator.h"

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

bool j1EntitiesManager::Awake(pugi::xml_node & node)
{
	return true;
}

bool j1EntitiesManager::Update(float dt)
{
	return true;
}

bool j1EntitiesManager::CleanUp()
{
	std::list<Creature*>::iterator cur_creatures_item = current_creatures.begin();
	while (cur_creatures_item != current_creatures.end())
	{
		delete cur_creatures_item._Ptr->_Myval;
		cur_creatures_item++;
	}
	current_creatures.clear();

	return true;
}

// Functionality ================================
Creature * j1EntitiesManager::GenerateCreature(CREATURE_TYPE creature_type)
{
	Creature* new_creature = nullptr;

	new_creature = new Creature();

	/*
	Here all the creature states are copied from the loaded ones
	*/

	new_creature->SetBody(App->physics->CreateRectangle(100, 800, 50, 70, collision_type::PLAYER_COLLISION, BODY_TYPE::PLAYER_BODY));


	current_creatures.push_back(new_creature);
	return new_creature;
}
