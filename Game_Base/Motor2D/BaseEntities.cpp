#include "BaseEntities.h"
#include "j1Animator.h"
#include "j1Physics.h"

/// Entity --------------------------------------
//Base class where the entity pillars are defined

//Constructors ========================
Entity::Entity()
{

}

Entity::Entity(const Entity & copy) : entity_type(copy.entity_type), name(copy.name), description(copy.description), body(copy.body)
{

}

//Destructors =========================
Entity::~Entity()
{
	delete body;
}

//Set Methods =========================
void Entity::SetEntityType(ENTITY_TYPE type)
{
	entity_type = type;
}

void Entity::SetName(string new_name)
{
	name = new_name;
}

void Entity::SetDescription(string new_description)
{
	description = new_description;
}

void Entity::SetBody(PhysBody * new_body)
{
	body = new_body;
}

//Get Methods =========================
ENTITY_TYPE Entity::GetEntityType() const
{
	return entity_type;
}

const char* Entity::GetName() const
{
	return name.c_str();
}

const char* Entity::GetDescription() const
{
	return description.c_str();
}

PhysBody * Entity::GetBody() const
{
	return body;
}

/// Creature ------------------------------------
// Constructors =======================
Creature::Creature()
{

}

Creature::Creature(const Creature & copy) : Entity(copy), life(copy.life), attack(copy.attack), mov_speed(copy.mov_speed)
{

}

// Destructors ========================
Creature::~Creature()
{

}

//Set Methods =========================
void Creature::SetCreatureType(CREATURE_TYPE new_creature_type)
{
	creature_type = new_creature_type;
}

void Creature::SetLife(uint new_life)
{
	life = new_life;
}

void Creature::SetAttack(uint new_attack)
{
	attack = new_attack;
}

void Creature::SetMovSpeed(float new_mov_speed)
{
	mov_speed = new_mov_speed;
}

//Get Methods =========================
CREATURE_TYPE Creature::GetCreatureType() const
{
	return creature_type;
}

uint Creature::GetLife() const
{
	return life;
}

uint Creature::GetAttack() const
{
	return attack;
}

float Creature::GetMovSpeed() const
{
	return mov_speed;
}
