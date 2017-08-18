#define _CRT_SECURE_NO_WARNINGS

#include "j1ParticleManager.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1FileSystem.h"

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
	active = enabled = false;
}

bool j1ParticleManager::Enable()
{
	j1Timer process_timer;

	//If the current enable node is NULL it is focused to the first node
	if (current_enable_node.root() == NULL)
	{
		current_enable_node = definitions_doc.first_child().first_child();
	}

	// Iterate all the nodes to enable
	while (current_enable_node != NULL)
	{
		//Generate a particle definition with the data of the current node
		AddParticleDefinition(&current_enable_node);

		//Focus the next enable node
		current_enable_node = current_enable_node.next_sibling();

		// Let a game loop when enable time is reached
		if (process_timer.Read() > TIME_TO_ENABLE && current_enable_node != NULL)
		{
			return false;
		}
	}

	enabled = true;
	active = false;

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
	App->fs->LoadXML(node.child("particles_data").attribute("file").as_string(), &definitions_doc);
	if (definitions_doc.root() == NULL)
	{
		LOG("Error loading Particles Defs Doc");
		return false;
	}

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

	return true;
}

// Functionality ================================
void j1ParticleManager::AddParticleDefinition(const pugi::xml_node * data_node)
{
	//General pointer to the undefined particle
	Particle* new_particle = nullptr;
	
	//Get the new particle type
	PARTICLE_TYPE particle_type = StrToParticleType(data_node->attribute("particle_type").as_string());
	bool need_font = false;
	switch (particle_type)
	{
	case ALLY_HITPOINTS_PARTICLE:
	case ENEMY_HITPOINTS_PARTICLE:
	case EXPERIENCE_POINTS_PARTICLE:
	case LEVEL_UP_PARTICLE:
		new_particle = new Static_Text_Particle();
		need_font = true;
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
		
	//Add the built particle at the particles definitions vector
	particles_defs.push_back(new_particle);
}

// Enums Methods ================================
PARTICLE_TYPE j1ParticleManager::StrToParticleType(const char * str) const
{
	if (strcmp(str, "ally_hitpoints_particle") == 0)		return ALLY_HITPOINTS_PARTICLE;
	if (strcmp(str, "enemy_hitpoints_particle") == 0)		return ENEMY_HITPOINTS_PARTICLE;
	if (strcmp(str, "experience_points_particle") == 0)		return EXPERIENCE_POINTS_PARTICLE;
	if (strcmp(str, "level_up_particle") == 0)				return LEVEL_UP_PARTICLE;
	return NO_PARTICLE;
}

// Functionality ================================
void j1ParticleManager::DeleteParticle(Particle * target)
{
	particles_to_delete.push_back(target);
}

// Factory ======================================
Particle * j1ParticleManager::GenerateTextParticle(const Entity * target, PARTICLE_TYPE particle_type, uint value)
{
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
	int x = 0, y = 0, w = 0;
	target->GetBody()->GetPosition(x, y);
	SDL_QueryTexture(tex, NULL, NULL, &w, NULL);
	new_particle->SetPosition((float)x - (w * 0.5f), (float)y - target->GetBody()->height);

	if (new_particle->GetVolatile())
	{
		//Reset particle life timer
		new_particle->ResetLifeTimer();
	}

	//Add the generated particle at the current particle for update
	current_particles.push_back(new_particle);

	return new_particle;
}