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
	Items_Tank(const Items_Tank& copy, bool generate_body = true);
	~Items_Tank();

public:

	//Game Loop -------------
	bool Update();

protected:

	std::list<Item*>	items;
	bool				ready_to_drop = false;

public:

	//Set Methods -----------
	void SetPosition(float x, float y);
	void SetReadyToDrop();

	//Functionality ---------
	void AddItem(const Item* new_item);

	void DropItems();

};
/// ---------------------------------------------

/// Coin ----------------------------------------
class Coin : public Item
{
public:

	Coin();
	Coin(const Coin& copy, bool generate_body = true);
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
