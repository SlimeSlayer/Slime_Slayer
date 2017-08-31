#ifndef _BASE_ENTITIES_H_
#define _BASE_ENTITIES_H_

#define CREATURES_DROPS_IMPULSE -4
#define CREATURES_DROPS_RAD 1.5
	
#include "p2Point.h"
#include "p2Defs.h"
#include <string>
#include "Worker.h"
#include "UI_Progressive_Bar.h"

class PhysBody;
class Animation;

enum ENTITY_TYPE
{
	NO_ENTITY = 0,
	CREATURE,
	CREATURE_EVO_TEMPLATE,
	ITEM
};

enum DIPLOMACY
{
	NO_DIPLOMACY,
	NEUTRAL,
	ALLY,
	ENEMY
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
	BASIC_BOX_ITEM,
	LEFT_MAP_LIMIT_ITEM,
	RIGHT_MAP_LIMIT_ITEM
};

enum ACTION_TYPE
{
	NO_ACTION = 0,
	ATTACK_ACTION,
	WALK_ACTION,
	DISAPPEAR_ACTION,
	DIE_ACTION,
	IDLE_ACTION
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
	virtual void Clean();

public:

	//Game Loop -------------
	virtual bool Update();
	virtual void Draw();

protected:

	ENTITY_TYPE		entity_type = NO_ENTITY;
	DIPLOMACY		diplomacy = NO_DIPLOMACY;
	PhysBody*		body = nullptr;
	std::string		name = "";
	std::string		description = "";
	Animation*		current_animation = nullptr;

	ACTION_TYPE		action_type = NO_ACTION;
	DIRECTION		direction = NO_DIRECTION;

public:

	Worker worker;

public:

	//Set Methods -----------
	void			SetEntityType(ENTITY_TYPE type);
	void			SetDiplomacy(DIPLOMACY dip);
	void			SetName(std::string new_name);
	void			SetDescription(std::string new_description);
	void			SetBody(PhysBody* new_body);
	virtual void	SetPosition(float x, float y);
	void			SetAnimation(Animation* animation);
	void			SetActionType(ACTION_TYPE action_type);
	void			SetDirection(DIRECTION direction);

	//Get Methods -----------
	ENTITY_TYPE	GetEntityType()const;
	DIPLOMACY	GetDiplomacy()const;
	const char*	GetName()const;
	const char*	GetDescription()const;
	PhysBody*	GetBody()const;
	Animation*	GetAnimation()const;
	ACTION_TYPE GetActionType()const;
	DIRECTION	GetDirection()const;

	//Functionality ---------
	virtual void		UpdatePosition();
	virtual const char*	GenerateMouseInString()const;
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

public:

	//Game Loop
	bool Update();
	void Draw();

protected:

	CREATURE_TYPE creature_type = NO_CREATURE;

	uint				current_life = 0;
	uint				max_life = 0;
	UI_Progressive_Bar	life_bar;
	uint				attack_hitpoints = 0;
	uint				attack_rate = 0;
	float				mov_speed = 0.0f;
	float				jump_force = 0.0f;

	uint	money = 0;						/*Current money*/
	uint	reward_experience = 0;			/*Player get reward * lvl when beat the creature*/
	uint	level = 1;						/*Current lvl*/
	
public:

	//Set Methods -----------
	void SetCreatureType(CREATURE_TYPE new_creature_type);
	virtual void SetCurrentLife(uint new_life);
	void SetMaxLife(uint new_max_life);
	void SetAttackHitPoints(uint new_attack);
	void SetAttackRate(uint atk_rate);
	void SetMovSpeed(float new_mov_speed);
	void SetJumpForce(float new_jump_force);
	void SetMoney(uint money);
	void SetRewardExperience(uint experience);

	//Get Methods -----------
	CREATURE_TYPE		GetCreatureType()const;
	uint				GetCurrentLife()const;
	uint				GetMaxLife()const;
	UI_Progressive_Bar*	GetLifeBar();
	uint				GetAttackHitPoints()const;
	uint				GetAttackRate()const;
	float				GetMovSpeed()const;
	float				GetJumpForce()const;
	uint				GetMoney()const;
	uint				GetRewardExperience()const;

	//Functionality ---------
	void			AddMoney(uint gained_money);
	void			DropMoney();
	void			DropExperience();

};
/// ---------------------------------------------
#endif