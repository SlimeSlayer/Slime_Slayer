#ifndef _PARTICLE_MANAGER_H_
#define _PARTICLE_MANAGER_H_

#define SCORE_PARTICLES_INITIAL_VEL -20
#define SCORE_PARTICLES_ACCELERATION_Y -150
#define SCORE_PARTICLES_LIFE 2500

#include "j1Module.h"
#include "j1Fonts.h"
#include "Particles.h"

class Entity;

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

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

private:

	std::list<Particle*>	current_particles;
	std::vector<Particle*>	particles_to_delete;

public:

	// Functionality --------
	void DeleteParticle(Particle* target);

	// Factory --------------
	Particle* GenerateDamagePointsParticle(const Entity* target, uint value);
	Particle* GenerateExperiencePointsParticle(const Entity* target, uint value);

};
#endif // !_PARTICLE_MANAGER_H_
