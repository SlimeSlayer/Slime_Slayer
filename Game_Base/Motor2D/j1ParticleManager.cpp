#define _CRT_SECURE_NO_WARNINGS

#include "j1ParticleManager.h"

#include "j1App.h"
#include "j1EntitiesManager.h"

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
	//Update all the particles
	std::list<Particle*>::const_iterator particle = current_particles.begin();
	while (particle != current_particles.end())
	{
		particle._Ptr->_Myval->Update(dt);

		particle++;
	}

	//Draw all the particles
	std::list<Particle*>::const_iterator d_particle = current_particles.begin();
	while (d_particle != current_particles.end())
	{
		d_particle._Ptr->_Myval->Draw();

		d_particle++;
	}

	//Release all the particles ready to be deleted
	uint size = particles_to_delete.size();
	for (uint k = 0; k < size; k++)
	{
		current_particles.remove(particles_to_delete[k]);
		RELEASE(particles_to_delete[k]);
	}
	particles_to_delete.clear();
	

	return true;
}

bool j1ParticleManager::CleanUp()
{
	//Release all the particles
	std::list<Particle*>::const_iterator particle = current_particles.begin();
	while (particle != current_particles.end())
	{
		RELEASE(particle._Ptr->_Myval);

		particle++;
	}
	current_particles.clear();

	//Release ready to be deleted vector
	particles_to_delete.clear();

	return true;
}

// Functionality ================================
void j1ParticleManager::DeleteParticle(Particle * target)
{
	particles_to_delete.push_back(target);
}

// Factory ======================================
Particle* j1ParticleManager::GenerateDamagePointsParticle(const Entity * target, uint value)
{
	//Get the correct font
	Font_Def* font = App->font->GetFontByID(FONT_ID::HITPOINTS_FONT);
	
	//Transform the value to a string
	char buffer[9];
	_itoa((int)value, buffer, 10);
	const char* str = buffer;
	
	//Generate a texture of the string
	SDL_Texture* tex = App->font->Print(str, font->font_color, font->font);
	
	//Generate the particle
	Static_Particle* new_particle = new Static_Particle();
	
	//Set all the particle data
	new_particle->SetParticleType(PARTICLE_TYPE::SCORE_PARTICLE);
	new_particle->SetTexture(tex);
	new_particle->SetVolatile(true);
	new_particle->SetLifeTime(SCORE_PARTICLES_LIFE);
	new_particle->SetVelocity(0, SCORE_PARTICLES_INITIAL_VEL);
	new_particle->SetAcceleration(0, SCORE_PARTICLES_ACCELERATION_Y);

	//Set position checking all the current stats
	int x = 0, y = 0, w = 0;
	target->GetBody()->GetPosition(x, y);
	SDL_QueryTexture(tex, NULL, NULL, &w, NULL);
	new_particle->SetPosition((float)x + target->GetBody()->width - w * 0.5, (float)y);
		
	//Add the built particle at the particles list
	current_particles.push_back(new_particle);
	
	//Reset particle life timer
	new_particle->ResetLifeTimer();

	return new_particle;
}