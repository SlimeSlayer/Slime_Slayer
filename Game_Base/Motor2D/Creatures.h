#ifndef _CREATURES_H_
#define _CREATURES_H_

#ifndef _BASE_ENTITIES_H_
#include "BaseEntities.h"
#endif 

/// Intelligent_Creature ------------------------
class Intelligent_Creature : public Creature
{
public:

	Intelligent_Creature();
	Intelligent_Creature(const Intelligent_Creature& copy, bool generate_body = true);
	~Intelligent_Creature();

protected:

	uint money = 0;

public:

	//Set Methods -----------
	void SetMoney(uint money);
	void AddMoney(uint gained_money);

	//Get Methods -----------
	uint GetMoney()const;
};
/// ---------------------------------------------
#endif // !_CREATURES_H_
