#ifndef _BASE_ENTITIES_H_
#define _BASE_ENTITIES_H_

#include "p2Point.h"
#include "p2Defs.h"
#include <string>
#include "Worker.h"

using namespace std;
class PhysBody;
class Animation;

enum ENTITY_TYPE
{
	NO_ENTITY = 0,
	CREATURE,
	ITEM
};

enum CREATURE_TYPE
{
	NO_CREATURE = 0,
	PLAYER_CREATURE,
	STANDARD_NPC_CREATURE,
	LORE_NPC_CREATURE,
	LORE_NPC_B_CREATURE,
	BASIC_ENEMY_CREATURE
};

enum ITEM_TYPE
{
	NO_ITEM = 0,
	COIN_ITEM,
	JAR_ITEM
};

enum DIRECTION
{
	NO_DIRECTION = 0,
	LEFT,
	RIGHT
};

/// Entity --------------------------------------
//Base class where the entity pillars are defined
class Entity
{
public:

	Entity();
	Entity(const Entity& copy, bool generate_body = true);
	virtual ~Entity();

public:

	//Game Loop -------------
	virtual bool Update();

protected:

	ENTITY_TYPE	entity_type = NO_ENTITY;
	PhysBody*	body = nullptr;
	string		name = "";
	string		description = "";
	//Animation*	current_animation = nullptr;

	DIRECTION	direction = NO_DIRECTION;

public:

	Worker worker;

public:

	//Set Methods -----------
	void			SetEntityType(ENTITY_TYPE type);
	void			SetName(string new_name);
	void			SetDescription(string new_description);
	void			SetBody(PhysBody* new_body);
	virtual void	SetPosition(float x, float y);
	void			SetDirection(DIRECTION direction);

	//Get Methods -----------
	ENTITY_TYPE	GetEntityType()const;
	const char*	GetName()const;
	const char*	GetDescription()const;
	PhysBody*	GetBody()const;
	DIRECTION	GetDirection()const;

	//Functionality ---------
	virtual void UpdatePosition();

};
/// ---------------------------------------------

/// Item ----------------------------------------
class Item : public Entity
{
public:

	Item();
	Item(const Item& copy, bool generate_body = true);
	~Item();

protected:

	ITEM_TYPE	item_type = NO_ITEM;
	bool		volatile_item = false;

public:

	//Set Methods -----------
	void SetItemType(ITEM_TYPE new_item_type);
	void SetAsVolatile();

	//Get Methods -----------
	ITEM_TYPE	GetItemType()const;
	bool		GetIfVolatile()const;

};
/// ---------------------------------------------

/// Creature ------------------------------------
class Creature : public Entity
{
public:

	Creature();
	Creature(const Creature& copy, bool generate_body = true);
	~Creature();

protected:

	CREATURE_TYPE creature_type = NO_CREATURE;

	uint	life = 0;
	uint	attack_hitpoints = 0;
	uint	attack_rate = 0;
	float	mov_speed = 0.0f;
	float	jump_force = 0.0f;

public:

	//Set Methods -----------
	void SetCreatureType(CREATURE_TYPE new_creature_type);
	void SetLife(uint new_life);
	void SetAttackHitPoints(uint new_attack);
	void SetAttackRate(uint atk_rate);
	void SetMovSpeed(float new_mov_speed);
	void SetJumpForce(float new_jump_force);

	//Get Methods -----------
	CREATURE_TYPE	GetCreatureType()const;
	uint			GetLife()const;
	uint			GetAttackHitPoints()const;
	uint			GetAttackRate()const;
	float			GetMovSpeed()const;
	float			GetJumpForce()const;

};
/// ---------------------------------------------
#endif