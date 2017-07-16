#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "p2Point.h"

class SDL_Texture;

enum PARTICLE_TYPE
{
	NO_PARTICLE = 0,
	SCORE_PARTICLE
};

/// Particle ------------------------------------
class Particle
{
public:

	Particle();
	Particle(const Particle& copy);
	~Particle();

private:

	PARTICLE_TYPE	particle_type = NO_PARTICLE;
	fPoint			position = { 0,0 };

public:

	//Set Methods -----------
	void SetPosition(float x, float y);

	//Get Methods -----------
	 fPoint GetPosition()const;
};
/// ---------------------------------------------

/// Static_Particle -----------------------------
class Static_Particle : public Particle
{
public:

	Static_Particle();
	Static_Particle(const Static_Particle& copy);
	~Static_Particle();

private:

	SDL_Texture* texture = nullptr;

public:

	//Set Methods -----------
	void SetTexture(const SDL_Texture* new_tex);

	//Get Methods -----------
	SDL_Texture* GetTexture()const;

};
/// ---------------------------------------------

/// Animated_Particle ---------------------------
//This will have an animation not texture
/// ---------------------------------------------

#endif // !_PARTICLES_H_
