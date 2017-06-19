#include "Items.h"

#include "j1App.h"
#include "j1EntitiesManager.h"

/// Items_Tank ----------------------------------
//Constructors ========================
Items_Tank::Items_Tank()
{

}

Items_Tank::Items_Tank(const Items_Tank & copy) :Item(copy)
{
	//Copy all the items of the list
	std::list<Item*>::const_iterator list_item = items.begin();
	while(list_item != items.end())
	{
		items.push_back(App->entities_manager->GenerateItem(list_item._Ptr->_Myval->GetItemType()));
		list_item++;
	}
}

//Destructors =========================
Items_Tank::~Items_Tank()
{
	std::list<Item*>::iterator list_item = items.begin();
	while(list_item!= items.end())
	{
		delete list_item._Ptr->_Myval;
		list_item++;
	}
	items.clear();
}

//Functionality =======================
void Items_Tank::AddItem(const Item * new_item)
{
	items.push_back((Item*)new_item);
}
/// ---------------------------------------------

/// Coin ----------------------------------------
//Constructors ========================
Coin::Coin()
{

}

Coin::Coin(const Coin & copy) :Item(copy), value(copy.value)
{

}

//Destructors =========================
Coin::~Coin()
{

}

//Set Methods =========================
void Coin::SetValue(uint new_value)
{
	value = new_value;
}

//Get Methods =========================
uint Coin::GetValue() const
{
	return value;
}
/// ---------------------------------------------


