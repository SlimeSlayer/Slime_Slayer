#include "Particles.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1ParticleManager.h"
#include "j1Textures.h"
#include "j1Animator.h"

/// Particle ------------------------------------
// Constructors =======================
Particle::Particle()
{

}

Particle::Particle(const Particle & copy) :particle_type(copy.particle_type), position(copy.position), velocity(copy.velocity), acceleration(copy.acceleration), volatile_(copy.volatile_), life_time(copy.life_time)
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

bool Particle::GetVolatile() const
{
	return volatile_;
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
	int alpha = MAX(0, (1 - ((float)life_timer.Read() / (float)life_time)) * 255);
	if (alpha > 0)
	{
		App->render->CallBlit(this->texture, this->position.x, this->position.y, NULL, false, false, false, 1.0f, 0, alpha);
	}
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
/// ---------------------------------------------

/// Static_Text_Particle ------------------------
// Constructors =======================
Static_Text_Particle::Static_Text_Particle()
{

}

Static_Text_Particle::Static_Text_Particle(const Static_Text_Particle & copy) :Static_Particle(copy), text_font(copy.text_font)
{

}

// Destructors ========================
Static_Text_Particle::~Static_Text_Particle()
{

}

//Set Methods =========================
void Static_Text_Particle::SetFont(const Font_Def * font)
{
	text_font = (Font_Def*)font;
}

//Get Methods =========================
Font_Def * Static_Text_Particle::GetFont() const
{
	return text_font;
}
/// ---------------------------------------------

/// Animated_Particle ---------------------------
// Constructors =======================
Animated_Particle::Animated_Particle()
{

}

Animated_Particle::Animated_Particle(const Animated_Particle & copy) :Particle(copy)
{
	if(copy.animation != nullptr)animation = new Animation(*copy.animation);
}

// Destructors ==================================
Animated_Particle::~Animated_Particle()
{
	RELEASE(animation);
}

// Game Loop ====================================
void Animated_Particle::Draw()
{
	if (animation != nullptr)
	{
		const Sprite* sprite = animation->GetCurrentSprite();
		App->render->CallBlit(animation->GetTexture(), position.x, position.y, sprite->GetFrame(), false, animation->GetSpritesHorizontalFlip(), animation->GetSpritesVerticalFlip(), animation->GetSpritesScale(), sprite->GetZ_cord(), sprite->GetOpacity(), -sprite->GetXpivot(), -sprite->GetYpivot(), animation->GetTexColor(),sprite->GetAngle());
	}
}

// Set Methods ==================================
void Animated_Particle::SetAnimation(const Animation * anim)
{
	animation = (Animation*)anim;
}

// Get Methods ==================================
Animation * Animated_Particle::GetAnimation() const
{
	return animation;
}
