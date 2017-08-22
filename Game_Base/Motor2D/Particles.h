#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "p2Point.h"
#include "j1Timer.h"

struct SDL_Texture;
struct Font_Def;
class Animation;

enum PARTICLE_TYPE
{
	NO_PARTICLE = 0,
	ALLY_HITPOINTS_PARTICLE,
	ENEMY_HITPOINTS_PARTICLE,
	EXPERIENCE_POINTS_PARTICLE,
	LEVEL_UP_PARTICLE,
	MAIN_MENU_SLIME_PARTICLE
};

enum PARTICLE_ANIMATION_ID
{
	NO_ANIM_ID = 0,
	IDLE_SLIME
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
	bool			GetVolatile()const;

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

/// Static_Text_Particle ------------------------
class Static_Text_Particle : public Static_Particle
{
public:

	Static_Text_Particle();
	Static_Text_Particle(const Static_Text_Particle& copy);
	~Static_Text_Particle();

protected:

	Font_Def* text_font = nullptr;

public:

	//Set Methods -----------
	void SetFont(const Font_Def* font);

	//Get Methods -----------
	Font_Def* GetFont()const;

};
/// ---------------------------------------------

/// Animated_Particle ---------------------------
class Animated_Particle : public Particle
{
public:

	Animated_Particle();
	Animated_Particle(const Animated_Particle& copy);
	~Animated_Particle();

protected:

	Animation* animation = nullptr;

public:

	//Set Methods -----------
	void SetAnimation(const Animation* anim);

	//Get Methods -----------
	Animation*	GetAnimation()const;
};
/// ---------------------------------------------

#endif // !_PARTICLES_H_
