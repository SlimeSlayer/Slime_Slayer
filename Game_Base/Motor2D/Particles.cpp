#include "Particles.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1ParticleManager.h"
#include "j1Textures.h"

/// Particle ------------------------------------
// Constructors =======================
Particle::Particle()
{

}

Particle::Particle(const Particle & copy) :particle_type(copy.particle_type), position(copy.position), velocity(copy.velocity), acceleration(copy.acceleration)
{

}

// Destructors ========================
Particle::~Particle()
{

}

// Game Loop ==========================
bool Particle::Update(float dt)
{
	//If the particles life end it is deleted
	if (volatile_ && life_timer.Read() > life_time)
	{
		App->particle_manager->DeleteParticle(this);
		LOG("Particle dead");
		return true;
	}

	//Update position & velocity
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
	velocity.x += acceleration.x * dt;
	velocity.y += acceleration.y * dt;

	return true;
}

void Particle::Draw()
{

}

//Set Methods =========================
void Particle::SetParticleType(PARTICLE_TYPE type)
{
	particle_type = type;
}

void Particle::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
}

void Particle::SetVelocity(float x, float y)
{
	velocity.x = x;
	velocity.y = y;
}

void Particle::SetAcceleration(float x, float y)
{
	acceleration.x = x;
	acceleration.y = y;
}

void Particle::SetVolatile(bool new_volatile_)
{
	volatile_ = new_volatile_;
}

void Particle::SetLifeTime(uint life_time)
{
	this->life_time = life_time;
}

//Get Methods =========================
PARTICLE_TYPE Particle::GetParticleType() const
{
	return particle_type;
}

fPoint Particle::GetPosition() const
{
	return position;
}

fPoint Particle::GetVelocity() const
{
	return velocity;
}

fPoint Particle::GetAcceleration() const
{
	return acceleration;
}

void Particle::ResetLifeTimer()
{
	life_timer.Start();
}
/// ---------------------------------------------

/// Static_Particle -----------------------------
// Constructors =======================
Static_Particle::Static_Particle()
{

}

Static_Particle::Static_Particle(const Static_Particle & copy) : Particle(copy), texture(copy.texture)
{

}

// Destructors ========================
Static_Particle::~Static_Particle()
{
	if (texture != nullptr)App->tex->UnLoad(texture);
}

// Game Loop ========================== 
void Static_Particle::Draw()
{
	SDL_SetTextureAlphaMod(this->texture, (1 - ((float)life_timer.Read() / (float)life_time)) * 255);
	App->render->CallBlit(this->texture, this->position.x, this->position.y);
}

//Set Methods =========================
void Static_Particle::SetTexture(const SDL_Texture * new_tex)
{
	texture = (SDL_Texture*)new_tex;
}

//Get Methods =========================
SDL_Texture * Static_Particle::GetTexture() const
{
	return texture;
}
