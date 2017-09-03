#define _CRT_SECURE_NO_WARNINGS

#include "j1ParticleManager.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1FileSystem.h"
#include "j1Animator.h"
#include "j1Textures.h"

// Constructors =================================
j1ParticleManager::j1ParticleManager()
{
	name = "particles_manager";
}

// Destructors ==================================
j1ParticleManager::~j1ParticleManager()
{

}

// Game Loop ====================================
void j1ParticleManager::Init()
{
	active = true;
	enabled = false;
}

bool j1ParticleManager::Enable()
{
	j1Timer process_timer;

	//If the current enable node is NULL it is focused to the first node
	if (current_enable_node.root() == NULL)
	{
		if(animations_loaded)current_enable_node = definitions_doc.first_child().first_child();
		else current_enable_node = animations_doc.first_child().first_child();
	}

	// Iterate all the nodes to enable
	while (current_enable_node != NULL)
	{
		//Generate a particle definition or animation definition with the data of the current node
		if(animations_loaded)AddParticleDefinition(&current_enable_node);
		else AddAnimationDefinition(&current_enable_node);

		//Focus the next enable node
		current_enable_node = current_enable_node.next_sibling();

		// Let a game loop when enable time is reached
		if (process_timer.Read() > TIME_TO_ENABLE && current_enable_node != NULL)
		{
			return false;
		}
	}

	if (!animations_loaded)
	{
		animations_loaded = true;
		return false;
	}

	enabled = true;
	active = false;
	animations_loaded = false;

	return true;
}

void j1ParticleManager::Disable()
{
	// Clean all the particles definitions
	uint size = particles_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(particles_defs[k]);
	}
	particles_defs.clear();

	// Clean all the particles animations definitions
	size = p_animations_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(p_animations_defs[k]);
	}
	p_animations_defs.clear();


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

	active = enabled = false;
}

bool j1ParticleManager::Awake(pugi::xml_node & node)
{
	LOG("Loading Particles Definitions Doc");
	App->fs->LoadXML(node.child("particles_data").attribute("defs_file").as_string(), &definitions_doc);
	if (definitions_doc.root() == NULL)
	{
		LOG("Error loading Particles Defs Doc");
		return false;
	}
	LOG("Loading Particles Animations Doc");
	App->fs->LoadXML(node.child("particles_data").attribute("animations_file").as_string(), &animations_doc);
	if (animations_doc.root() == NULL)
	{
		LOG("Error loading Particles Animations Doc");
		return false;
	}

	return true;
}

bool j1ParticleManager::Start()
{
	while (!Enable()) {}

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
	// Clean all the particles definitions
	uint size = particles_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(particles_defs[k]);
	}
	particles_defs.clear();

	// Clean all the particles animations definitions
	size = p_animations_defs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(p_animations_defs[k]);
	}
	p_animations_defs.clear();

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

	//Reset definitions doc
	definitions_doc.reset();
	animations_doc.reset();

	return true;
}

// Functionality ================================
bool j1ParticleManager::AddParticleDefinition(const pugi::xml_node * data_node)
{
	//General pointer to the undefined particle
	Particle* new_particle = nullptr;
	
	//Get the new particle type
	PARTICLE_TYPE particle_type = StrToParticleType(data_node->attribute("particle_type").as_string());
	if (particle_type == NO_PARTICLE)return false;

	bool need_font = false;
	bool need_animation = false;
	switch (particle_type)
	{
	case ALLY_HITPOINTS_PARTICLE:
	case ENEMY_HITPOINTS_PARTICLE:
	case EXPERIENCE_POINTS_PARTICLE:
	case LEVEL_UP_PARTICLE:
		new_particle = new Static_Text_Particle();
		need_font = true;
		break;
	case MAIN_MENU_SLIME_PARTICLE:
	case SLASH_PARTICLE:
		new_particle = new Animated_Particle();
		need_animation = true;
		break;
	}

	// Set all the particle definitions stats
	/*Particle Type*/		new_particle->SetParticleType(particle_type);
	/*Initial Velocity*/	new_particle->SetVelocity(data_node->attribute("initial_vel_x").as_float(), data_node->attribute("initial_vel_y").as_float());
	/*Acceleration*/		new_particle->SetAcceleration(data_node->attribute("acceleration_x").as_float(), data_node->attribute("acceleration_y").as_float());
	/*Volatile*/			new_particle->SetVolatile(data_node->attribute("volatile").as_bool());
	if (new_particle->GetVolatile())
	{
		/*Life Time*/		new_particle->SetLifeTime(data_node->attribute("life_time").as_uint());
	}

	// Set specific particles stats
	if (need_font)
	{
		/*Text Font*/ ((Static_Text_Particle*)new_particle)->SetFont(App->font->GetFontByID(App->font->StrToFontID(data_node->attribute("font_id").as_string())));
	}
	if (need_animation)
	{
		/*Particle Animation*/	((Animated_Particle*)new_particle)->SetAnimation(GetParticleAnimationByID(StrToParticleAnimationID(data_node->attribute("animation_id").as_string())));
	}
		
	//Add the built particle at the particles definitions vector
	particles_defs.push_back(new_particle);

	return true;
}

bool j1ParticleManager::AddAnimationDefinition(const pugi::xml_node * data_node)
{
	//Allocate the new animation
	Animation* new_animation = new Animation();

	//Load all animation data form the node
	//Animation ID
	PARTICLE_ANIMATION_ID animation_id = StrToParticleAnimationID(data_node->attribute("id").as_string());
	if (animation_id == NO_ANIM_ID)
	{
		RELEASE(new_animation);
		LOG("Error loading particle animation: Animation ID undefined");
		return false;
	}
	new_animation->SetId(animation_id);
	//Texture
	std::string tex_folder = name + "/" + data_node->attribute("texture").as_string();
	SDL_Texture* texture = App->tex->Load(tex_folder.c_str());
	if (texture == nullptr)
	{
		RELEASE(new_animation);
		LOG("Error loading %s texture", tex_folder.c_str());
		return false;
	}
	new_animation->SetTexture(texture);
	//Loop
	new_animation->SetLoop(data_node->attribute("loop").as_bool());
	//Speed
	new_animation->SetSpeed(data_node->attribute("speed").as_uint());
	//Scale
	new_animation->SetSpritesScale(data_node->attribute("scale").as_float());
	//Animation flip
	new_animation->SetSpritesHorizontalFlip(data_node->attribute("h_flip_sprites").as_bool(false));
	new_animation->SetSpritesVerticalFlip(data_node->attribute("v_flip_sprites").as_bool(false));
	//Color Texture 
	new_animation->SetTexColor(App->entities_manager->TokenStrToColor(data_node->attribute("tex_color").as_string()));

	pugi::xml_node sprite_node = data_node->child("sprite");
	while (sprite_node != NULL)
	{
		//Load current sprite node data

		//Load sprite rect
		SDL_Rect rect = { sprite_node.attribute("x").as_int(),sprite_node.attribute("y").as_int(),sprite_node.attribute("w").as_int(),sprite_node.attribute("h").as_int() };

		//Load sprite pivot
		float pX = sprite_node.attribute("pX").as_float() * rect.w;
		pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
		float pY = sprite_node.attribute("pY").as_float() * rect.h;
		pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);

		//Load sprite opacity
		uint opacity = sprite_node.attribute("opacity").as_uint(255);

		//Add sprite at animation
		new_animation->AddSprite(rect, iPoint(pX, pY), sprite_node.attribute("z").as_int(), opacity);


		//Focus next sprite node
		sprite_node = sprite_node.next_sibling();
	}

	//Add the built animation to the definitions vector
	p_animations_defs.push_back(new_animation);

	return true;
}

// Enums Methods ================================
PARTICLE_TYPE j1ParticleManager::StrToParticleType(const char * str) const
{
	if (strcmp(str, "ally_hitpoints_particle") == 0)		return ALLY_HITPOINTS_PARTICLE;
	if (strcmp(str, "enemy_hitpoints_particle") == 0)		return ENEMY_HITPOINTS_PARTICLE;
	if (strcmp(str, "experience_points_particle") == 0)		return EXPERIENCE_POINTS_PARTICLE;
	if (strcmp(str, "level_up_particle") == 0)				return LEVEL_UP_PARTICLE;
	if (strcmp(str, "main_menu_slime_particle") == 0)		return MAIN_MENU_SLIME_PARTICLE;
	if (strcmp(str, "slash_particle") == 0)					return SLASH_PARTICLE;
	return NO_PARTICLE;
}

PARTICLE_ANIMATION_ID j1ParticleManager::StrToParticleAnimationID(const char * str) const
{
	if (strcmp(str, "idle_slime") == 0)			return IDLE_SLIME;
	if (strcmp(str, "slash_left_down") == 0)	return SLASH_LEFT_DOWN;
	if (strcmp(str, "slash_left_up") == 0)		return SLASH_LEFT_UP;
	if (strcmp(str, "slash_right_down") == 0)	return SLASH_RIGHT_DOWN;
	if (strcmp(str, "slash_right_up") == 0)		return SLASH_RIGHT_UP;
	return NO_ANIM_ID;
}

bool j1ParticleManager::IsTextType(PARTICLE_TYPE type) const
{
	return (type == ALLY_HITPOINTS_PARTICLE || 
			type == ENEMY_HITPOINTS_PARTICLE || 
			type == EXPERIENCE_POINTS_PARTICLE ||
			type == LEVEL_UP_PARTICLE);
}

bool j1ParticleManager::IsAnimationType(PARTICLE_TYPE type) const
{
	return (type == MAIN_MENU_SLIME_PARTICLE ||
			type == SLASH_PARTICLE);
}

// Functionality ================================
void j1ParticleManager::DeleteParticle(Particle * target)
{
	particles_to_delete.push_back(target);
}

// Factory ======================================
Particle * j1ParticleManager::GenerateTextParticle(const Entity * target, PARTICLE_TYPE particle_type, uint value)
{
	if (!IsTextType(particle_type))
	{
		LOG("Error generating text particle: Invalid type");
		return nullptr;
	}

	//First we search the particle
	Particle* particle_template = nullptr;
	uint size = particles_defs.size();
	for (uint k = 0; k < size; k++)
	{
		if (particles_defs[k]->GetParticleType() == particle_type)
		{
			particle_template = particles_defs[k];
			break;
		}
	}
	if (particle_template == nullptr)
	{
		LOG("Error generating particle(template not found)");
		return nullptr;
	}

	//Allocate the correct class
	Particle* new_particle = new Static_Text_Particle(*(Static_Text_Particle*)particle_template);

	//String of the text particle
	const char* str;

	//Transform the value to a string
	char buffer[9];
	_itoa((int)value, buffer, 10);
	str = buffer;

	//Depending of the particle type different text is build
	switch (particle_type)
	{
	case EXPERIENCE_POINTS_PARTICLE:
	{
		//Add + XP on the value string to clarify the output
		uint str_size = strlen(buffer);
		for (uint k = str_size; k > 0; k--)
		{
			buffer[k] = buffer[k - 1];
		}
		buffer[0] = '+';
		buffer[str_size + 1] = 'X';
		buffer[str_size + 2] = 'P';
		buffer[str_size + 3] = '\0';

		//The resulting constant string
		str = buffer;
	}
	break;

	case LEVEL_UP_PARTICLE:
	{
		str = "Level UP!";
	}
	break;
	}

	//Generate a texture of the string
	SDL_Texture* tex = App->font->Print(str, ((Static_Text_Particle*)new_particle)->GetFont()->font_color, ((Static_Text_Particle*)new_particle)->GetFont()->font);
	((Static_Text_Particle*)new_particle)->SetTexture(tex);

	//Set position checking all the current stats
	if (target != nullptr)
	{
		int x = 0, y = 0, w = 0;
		target->GetBody()->GetPosition(x, y);
		SDL_QueryTexture(tex, NULL, NULL, &w, NULL);
		new_particle->SetPosition((float)x - (w * 0.5f), (float)y - target->GetBody()->height);
	}

	if (new_particle->GetVolatile())
	{
		//Reset particle life timer
		new_particle->ResetLifeTimer();
	}

	//Add the generated particle at the current particle for update
	current_particles.push_back(new_particle);

	return new_particle;
}

Particle * j1ParticleManager::GenerateAnimationParticle(PARTICLE_TYPE particle_type, ...)
{
	//Check particle type
	if (!IsAnimationType(particle_type))
	{
		LOG("Error generating animation particle: Invalid type");
		return nullptr;
	}

	//General animated particle pointer
	Animated_Particle* new_particle = nullptr;

	//First we search the particle
	Particle* particle_template = nullptr;
	uint size = particles_defs.size();
	for (uint k = 0; k < size; k++)
	{
		if (particles_defs[k]->GetParticleType() == particle_type)
		{
			particle_template = particles_defs[k];
			break;
		}
	}
	if (particle_template == nullptr)
	{
		LOG("Error generating particle(template not found)");
		return nullptr;
	}

	//Get specific variables
	va_list variables;
	va_start(variables, particle_type);

	//Allocate the correct particle class
	switch (particle_type)
	{
	case MAIN_MENU_SLIME_PARTICLE:
		new_particle = new Animated_Particle(*(Animated_Particle*)particle_template);
		break;
	case SLASH_PARTICLE:
		new_particle = new Animated_Particle(*(Animated_Particle*)particle_template);
		new_particle->SetAnimation(GetParticleAnimationByID(va_arg(variables, PARTICLE_ANIMATION_ID)));
		break;
	}





	//Add the generated particle at the current particle for update
	current_particles.push_back(new_particle);

	return new_particle;
}

Animation * j1ParticleManager::GetParticleAnimationByID(PARTICLE_ANIMATION_ID id)
{
	uint size = p_animations_defs.size();
	for (uint k = 0; k < size; k++)
	{
		if (p_animations_defs[k]->GetId() == id)
		{
			return new Animation(*p_animations_defs[k]);
		}
	}

	return nullptr;
}
