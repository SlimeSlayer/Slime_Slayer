#include "BaseEntities.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Animator.h"
#include "j1Physics.h"

/// Entity --------------------------------------
//Base class where the entity pillars are defined

//Constructors ========================
Entity::Entity()
{

}

Entity::Entity(const Entity & copy, bool generate_body) : entity_type(copy.entity_type), name(copy.name), description(copy.description)
{
	if(generate_body)body = App->physics->TransformDefToBuilt(copy.body);
	else body = copy.body;
}

//Destructors =========================
Entity::~Entity()
{
	if (body != nullptr)App->physics->DeleteBody(body);
}

//Game Loop ===========================
bool Entity::Update()
{
	return true;
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

void Entity::SetPosition(float x, float y)
{
	body->SetPosition(x, y);
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

/// Item ----------------------------------------
// Constructors =======================
Item::Item()
{

}

Item::Item(const Item & copy, bool generate_body) :Entity(copy, generate_body), item_type(copy.item_type)
{

}

// Destructors ========================
Item::~Item()
{

}

//Set Methods =========================
void Item::SetItemType(ITEM_TYPE new_item_type)
{
	item_type = new_item_type;
}

//Get Methods =========================
ITEM_TYPE Item::GetItemType() const
{
	return item_type;
}
/// ---------------------------------------------

/// Creature ------------------------------------
// Constructors =======================
Creature::Creature()
{

}

Creature::Creature(const Creature & copy, bool generate_body) : Entity(copy, generate_body), creature_type(copy.creature_type), life(copy.life), attack(copy.attack), mov_speed(copy.mov_speed), jump_force(copy.jump_force)
{
	entity_type = CREATURE;
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

void Creature::SetJumpForce(float new_jump_force)
{
	jump_force = new_jump_force;
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

float Creature::GetJumpForce() const
{
	return jump_force;
}
