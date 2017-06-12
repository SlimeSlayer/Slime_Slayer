#include "p2Point.h"
#include "p2Defs.h"
#include <string>

using namespace std;
class Collider;
class Animation;

enum ENTITY_TYPE
{
	NO_ENTITY = 0,
	WALL,
	PLAYER
};

/// Entity --------------------------------------
//Base class where the entity pillars are defined
class Entity
{
public:

	Entity();
	Entity(const Entity& copy);
	~Entity();

private:

	ENTITY_TYPE	entity_type = NO_ENTITY;
	Collider*	collider = nullptr;
	string		name = "";
	string		description = "";
	Animation*	current_animation = nullptr;
	fPoint		position = { 0,0 };
	fPoint		speed = { 0,0 };

public:

	//Set Methods -----------
	void SetEntityType(ENTITY_TYPE type);
	void SetName(string new_name);
	void SetDescription(string new_description);
	void SetPosition(fPoint new_position);
	void SetSpeed(fPoint new_speed);

	//Get Methods -----------
	ENTITY_TYPE	GetEntityType()const;
	const char*	GetName()const;
	const char*	GetDescription()const;
	fPoint		GetPosition()const;
	fPoint		GetSpeed()const;

};
/// ---------------------------------------------