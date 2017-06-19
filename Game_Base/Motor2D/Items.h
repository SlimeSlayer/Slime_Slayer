#ifndef _ITEMS_H_
#define _ITEMS_H_

#ifndef _BASE_ENTITIES_H_
#include "BaseEntities.h"
#endif 

#include <list>

/// Items_Tank ----------------------------------
class Items_Tank : public Item
{
public:

	Items_Tank();
	Items_Tank(const Items_Tank& copy);
	~Items_Tank();

protected:

	std::list<Item*> items;

public:

	//Functionality ---------
	void AddItem(const Item* new_item);

};
/// ---------------------------------------------

/// Coin ----------------------------------------
class Coin : public Item
{
public:

	Coin();
	Coin(const Coin& copy);
	~Coin();

protected:

	uint value;

public:

	//Set Methods -----------
	void SetValue(uint new_value);

	//Get Methods -----------
	uint GetValue()const;

};
/// ---------------------------------------------
#endif // _ITEMS_H_
