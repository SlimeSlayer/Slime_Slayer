#ifndef _PARTICLE_MANAGER_H_
#define _PARTICLE_MANAGER_H_

#define SCORE_PARTICLES_INITIAL_VEL -20
#define SCORE_PARTICLES_ACCELERATION_Y -150
#define SCORE_PARTICLES_LIFE 2500

#include "j1Module.h"
#include "j1Fonts.h"
#include "Particles.h"

class Entity;
class Creature;

class j1ParticleManager : public j1Module
{
public:

	j1ParticleManager();
	~j1ParticleManager();

public:

	// Game Loop ------------
	void Init();
	bool Enable();
	void Disable();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

private:

	//Data used during the segmented enable process
	pugi::xml_document	definitions_doc;
	pugi::xml_document	animations_doc;
	bool				animations_loaded = false;
	pugi::xml_node		current_enable_node;

	// Add definitions methods
	bool AddParticleDefinition(const pugi::xml_node* data_node);
	bool AddAnimationDefinition(const pugi::xml_node* data_node);

	// Vector with all the particles definitions
	std::vector<Particle*>	particles_defs;
	// Vector with all the animations that particles can use
	std::vector<Animation*> p_animations_defs;

	// List with the current particles in game
	std::list<Particle*>	current_particles;
	// Vector with all the dead particles ready to be deleted
	std::vector<Particle*>	particles_to_delete;

public:

	//Enums Methods ---------
	PARTICLE_TYPE			StrToParticleType(const char* str)const;
	PARTICLE_ANIMATION_ID	StrToParticleAnimationID(const char* str)const;

	// Functionality --------
	bool IsTextType(PARTICLE_TYPE type)const;
	bool IsAnimationType(PARTICLE_TYPE type)const;
	void DeleteParticle(Particle* target);

	// Factory --------------
	Particle*	GenerateTextParticle(const Entity* target, PARTICLE_TYPE particle_type, uint value);
	
	/*
	- MAIN_MENU_SLIME_PARTICLE: none
	- SLASH_PARTICLE: PARTICLE_ANIMATION_ID animation_id
	*/
	Particle*	GenerateAnimationParticle(PARTICLE_TYPE particle_type, ...);

	Animation*	GetParticleAnimationByID(PARTICLE_ANIMATION_ID id);
};
#endif // !_PARTICLE_MANAGER_H_
