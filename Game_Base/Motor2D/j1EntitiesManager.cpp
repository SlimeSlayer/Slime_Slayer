#include "j1EntitiesManager.h"

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
	return true;
}
