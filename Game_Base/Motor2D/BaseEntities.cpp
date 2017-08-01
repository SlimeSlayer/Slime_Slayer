#include "BaseEntities.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Animator.h"
#include "j1Physics.h"
#include "j1EntitiesManager.h"
#include "j1ParticleManager.h"

/// Entity --------------------------------------
//Base class where the entity pillars are defined

//Constructors ========================
Entity::Entity()
{

}

Entity::Entity(const Entity & copy, bool generate_body) : entity_type(copy.entity_type), name(copy.name), description(copy.description)
{
	if (generate_body)
	{
		body = App->physics->TransformDefToBuilt(copy.body);
		body->entity_related = this;
	}
	else body = new PhysBody(*copy.body);
}

//Destructors =========================
Entity::~Entity()
{
	if (body != nullptr)RELEASE(body);
}

void Entity::Clean()
{
	RELEASE(body);
}

//Game Loop ===========================
bool Entity::Update()
{
	//Update worker
	worker.Update();

	return true;
}

//Set Methods =========================
void Entity::SetEntityType(ENTITY_TYPE type)
{
	entity_type = type;
}

void Entity::SetName(std::string new_name)
{
	name = new_name;
}

void Entity::SetDescription(std::string new_description)
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

void Entity::SetDirection(DIRECTION direction)
{
	this->direction = direction;
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

DIRECTION Entity::GetDirection() const
{
	return direction;
}

// Functionality ======================
void Entity::UpdatePosition()
{

}

/// Item ----------------------------------------
// Constructors =======================
Item::Item()
{

}

Item::Item(const Item & copy, bool generate_body) :Entity(copy, generate_body), item_type(copy.item_type), volatile_item(copy.volatile_item)
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

void Item::SetAsVolatile()
{
	volatile_item = true;
}

//Get Methods =========================
ITEM_TYPE Item::GetItemType() const
{
	return item_type;
}

bool Item::GetIfVolatile() const
{
	return volatile_item;
}
/// ---------------------------------------------

/// Creature ------------------------------------
// Constructors =======================
Creature::Creature()
{

}

Creature::Creature(const Creature & copy, bool generate_body) : Entity(copy, generate_body), creature_type(copy.creature_type), life(copy.life), attack_hitpoints(copy.attack_hitpoints), attack_rate(copy.attack_rate), mov_speed(copy.mov_speed), jump_force(copy.jump_force), money(copy.money), experience(copy.experience)
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

void Creature::SetAttackHitPoints(uint new_attack)
{
	attack_hitpoints = new_attack;
}

void Creature::SetAttackRate(uint atk_rate)
{
	attack_rate = atk_rate;
}

void Creature::SetMovSpeed(float new_mov_speed)
{
	mov_speed = new_mov_speed;
}

void Creature::SetJumpForce(float new_jump_force)
{
	jump_force = new_jump_force;
}

void Creature::SetMoney(uint money)
{
	this->money = money;
}

void Creature::SetExperience(uint experience)
{
	this->experience = experience;
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

uint Creature::GetAttackHitPoints() const
{
	return attack_hitpoints;
}

uint Creature::GetAttackRate() const
{
	return attack_rate;
}

float Creature::GetMovSpeed() const
{
	return mov_speed;
}

float Creature::GetJumpForce() const
{
	return jump_force;
}

uint Creature::GetMoney() const
{
	return money;
}

uint Creature::GetExperience() const
{
	return experience;
}

// Functionality ======================
void Creature::AddMoney(uint gained_money)
{
	money += gained_money;
}

void Creature::AddExperience(uint gained_xp)
{
	experience += gained_xp;
}

void Creature::DropMoney()
{
	//Drop the creature money
	int x = 0, y = 0;
	body->GetPosition(x, y);
	for (uint k = 0; k < money; k++)
	{
		//Generate all the items
		Item* coin = App->entities_manager->GenerateItem(COIN_ITEM);
		
		//Set items correct states
		coin->SetPosition(x, y);
		
		float per_cent = (rand() % 100) * 0.01;
		int dir = rand() % 10 > 5 ? -1 : 1;
		coin->GetBody()->body->SetLinearVelocity(b2Vec2(CREATURES_DROPS_RAD * per_cent * dir, CREATURES_DROPS_IMPULSE + CREATURES_DROPS_RAD / MAX(per_cent, 0.5)));
		
		//Coins dropped have a ghost delay
		coin->worker.AddAction(coin->worker.GenerateSpawnDelayAction(coin));

		//Rest volatile timer for correct timing
		((Volatile_Item*)coin)->ResetVolatileTimer();
	}

	//Reset money value
	money = 0;
}

void Creature::DropExperience(Creature* target)
{
	//Generate the experience particle
	App->particle_manager->GenerateExperiencePointsParticle(target, this->experience);
	
	//Add the experience to the target
	target->AddExperience(experience);
}
