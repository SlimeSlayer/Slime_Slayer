#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "p2Point.h"
#include "j1Timer.h"

struct SDL_Texture;

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

public:

	//Game Loop -------------
	virtual bool Update(float dt);
	virtual void Draw();

protected:

	PARTICLE_TYPE	particle_type = NO_PARTICLE;
	fPoint			position = { 0,0 };
	fPoint			velocity = { 0,0 };
	fPoint			acceleration = { 0,0 };
	
	bool			volatile_ = false;
	uint			life_time = 0;
	j1Timer			life_timer;

public:

	//Set Methods -----------
	void SetParticleType(PARTICLE_TYPE type);
	void SetPosition(float x, float y);
	void SetVelocity(float x, float y);
	void SetAcceleration(float x, float y);
	void SetVolatile(bool new_volatile_);
	void SetLifeTime(uint life_time);

	//Get Methods -----------
	PARTICLE_TYPE	GetParticleType()const;
	fPoint			GetPosition()const;
	fPoint			GetVelocity()const;
	fPoint			GetAcceleration()const;

	//Functionality ---------
	void ResetLifeTimer();

};
/// ---------------------------------------------

/// Static_Particle -----------------------------
class Static_Particle : public Particle
{
public:

	Static_Particle();
	Static_Particle(const Static_Particle& copy);
	~Static_Particle();

public:

	//Game Loop -------------
	void Draw();

protected:

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
