#include "BaseEntities.h"
#include "j1Animator.h"
#include "j1Collisions.h"

/// Entity --------------------------------------
//Base class where the entity pillars are defined

//Constructors ========================
Entity::Entity()
{

}

Entity::Entity(const Entity & copy) : entity_type(copy.entity_type), name(copy.name), description(copy.description), position(copy.position), speed(copy.speed)
{

}

//Destructors =========================
Entity::~Entity()
{

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

void Entity::SetPosition(fPoint new_position)
{
	position = new_position;
}

void Entity::SetSpeed(fPoint new_speed)
{
	speed = new_speed;
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

fPoint Entity::GetPosition() const
{
	return position;
}

fPoint Entity::GetSpeed() const
{
	return speed;
}


