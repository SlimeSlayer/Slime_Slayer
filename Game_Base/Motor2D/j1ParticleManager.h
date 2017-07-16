#ifndef _PARTICLE_MANAGER_H_
#define _PARTICLE_MANAGER_H_

#include "j1Module.h"
#include "Particles.h"

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

	std::list<Particle*> current_particles;

};
#endif // !_PARTICLE_MANAGER_H_
