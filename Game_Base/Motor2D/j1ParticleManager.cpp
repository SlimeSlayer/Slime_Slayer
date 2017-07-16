#include "j1ParticleManager.h"

// Constructors =================================
j1ParticleManager::j1ParticleManager()
{
	name = "particle_manager";
}

// Destructors ==================================
j1ParticleManager::~j1ParticleManager()
{

}

// Game Loop ====================================
void j1ParticleManager::Init()
{
	active = enabled = false;
}

bool j1ParticleManager::Enable()
{
	return true;
}

void j1ParticleManager::Disable()
{

}

bool j1ParticleManager::Awake(pugi::xml_node & node)
{
	return true;
}

bool j1ParticleManager::Update(float dt)
{
	return true;
}

bool j1ParticleManager::CleanUp()
{
	return true;
}
