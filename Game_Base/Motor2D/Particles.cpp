#include "Particles.h"

/// Particle ------------------------------------
// Constructors =======================
Particle::Particle()
{

}

Particle::Particle(const Particle & copy) :particle_type(copy.particle_type), position(copy.position)
{

}

// Destructors ========================
Particle::~Particle()
{

}

//Set Methods =========================
void Particle::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
}

//Get Methods =========================
fPoint Particle::GetPosition() const
{
	return position;
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
