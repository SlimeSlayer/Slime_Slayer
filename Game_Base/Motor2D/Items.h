#ifndef _ITEMS_H_
#define _ITEMS_H_

#ifndef _BASE_ENTITIES_H_
#include "BaseEntities.h"
#endif 

#include <list>
#include "j1Timer.h"

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
	float				drop_impulse = 0.0f;
	float				drop_total_rad = 0.0f;
	bool				ready_to_drop = false;

public:

	//Set Methods -----------
	void SetPosition(float x, float y);
	void SetDropImpulse(float new_val);
	void SetDropTotalRad(float new_val);
	void SetReadyToDrop();
	
	//Get Methods -----------
	float GetDropImpulse()const;
	float GetDropTotalRad()const;

	//Functionality ---------
	void AddItem(const Item* new_item);

	void DropItems();

};
/// ---------------------------------------------

/// Volatile_Item -------------------------------
class Volatile_Item : public Item
{
public:

	Volatile_Item();
	Volatile_Item(const Volatile_Item& copy, bool generate_body = true);
	~Volatile_Item();

public:

	//Game Loop -------------
	bool Update();

protected:

	uint	time_to_delete = 0;
	j1Timer	delete_timer;

public:

	//Functionality ---------
	void ResetVolatileTimer();

	// Set Methods ----------
	void SetTimeToDelete(uint time);

	// Get Methods ----------
	uint GetTimeToDelete()const;

};
/// ---------------------------------------------

/// Coin ----------------------------------------
class Coin : public Volatile_Item
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
