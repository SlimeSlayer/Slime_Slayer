#ifndef _CREATURES_H_
#define _CREATURES_H_

#ifndef _BASE_ENTITIES_H_
#include "BaseEntities.h"
#endif 

class PhysBody;

/// Intelligent_Creature ------------------------
class Intelligent_Creature : public Creature
{
public:

	Intelligent_Creature();
	Intelligent_Creature(const Intelligent_Creature& copy, bool generate_body = true);
	~Intelligent_Creature();

public:

	//Game Loop -------------
	bool Update();

protected:

	uint money = 0;

	PhysBody*	vision_area = nullptr;

public:

	//Set Methods -----------
	void SetPosition(float x, float y);
	void SetMoney(uint money);
	void SetVisionArea(PhysBody* new_area);

	//Get Methods -----------
	uint		GetMoney()const;
	PhysBody*	GetVisionArea()const;

	//Functionality ---------
	void AddMoney(uint gained_money);
	void UpdatePosition();

	virtual void HandleInteraction(Entity* target);
};
/// ---------------------------------------------
#endif // !_CREATURES_H_
