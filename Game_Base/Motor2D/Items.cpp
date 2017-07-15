#include "Items.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1Physics.h"

/// Items_Tank ----------------------------------
//Constructors ========================
Items_Tank::Items_Tank()
{

}

Items_Tank::Items_Tank(const Items_Tank & copy, bool generate_body) :Item(copy, generate_body), drop_impulse(copy.drop_impulse), drop_total_rad(copy.drop_total_rad)
{
	//Copy all the items of the list
	std::list<Item*>::const_iterator list_item = copy.items.begin();
	while(list_item != copy.items.end())
	{
		items.push_back(App->entities_manager->GenerateItem(list_item._Ptr->_Myval->GetItemType(),false));
		list_item++;
	}
}

//Destructors =========================
Items_Tank::~Items_Tank()
{
	std::list<Item*>::iterator list_item = items.begin();
	while(list_item!= items.end())
	{
		RELEASE(list_item._Ptr->_Myval);
		list_item++;
	}
	items.clear();
}

//Game Loop ===========================
bool Items_Tank::Update()
{
	if (ready_to_drop)
	{
		DropItems();
	}
	return true;
}

//Set Methods =========================
void Items_Tank::SetPosition(float x, float y)
{
	this->body->SetPosition(x, y);
	std::list<Item*>::const_iterator list_item = items.begin();
	while (list_item != items.end())
	{
		list_item._Ptr->_Myval->SetPosition(x, y);
		list_item++;
	}
}

void Items_Tank::SetDropImpulse(float new_val)
{
	drop_impulse = new_val;
}

void Items_Tank::SetDropTotalRad(float new_val)
{
	drop_total_rad = new_val;
}

void Items_Tank::SetReadyToDrop()
{
	ready_to_drop = true;
}

//Get Methods =========================
float Items_Tank::GetDropImpulse() const
{
	return drop_impulse;
}

float Items_Tank::GetDropTotalRad() const
{
	return drop_total_rad;
}

//Functionality =======================
void Items_Tank::AddItem(const Item * new_item)
{
	items.push_back((Item*)new_item);
}

void Items_Tank::DropItems()
{
	std::list<Item*>::const_iterator list_item = items.begin();
	while (list_item != items.end())
	{
		// Physbody that contains only the items bodydef
		PhysBody* old_physbody = list_item._Ptr->_Myval->GetBody();
		//Generate a new built body with the old definition
		list_item._Ptr->_Myval->SetBody(App->physics->TransformDefToBuilt(list_item._Ptr->_Myval->GetBody()));
		list_item._Ptr->_Myval->GetBody()->entity_related = list_item._Ptr->_Myval;

		//Delete the old physbody
		RELEASE(old_physbody);

		//Set the new body stats checking the items data
		//Position
		int x = 0, y = 0;
		this->body->GetPosition(x, y);
		list_item._Ptr->_Myval->GetBody()->SetPosition((float)x, (float)y);
		//Drop impulse
		float per_cent = (rand() % 100) * 0.01;
		list_item._Ptr->_Myval->GetBody()->body->SetLinearVelocity({ drop_total_rad * per_cent,-drop_impulse });
		
		//Items dropped have a ghost delay
		Action* act = list_item._Ptr->_Myval->worker.GenerateSpawnDelayAction(list_item._Ptr->_Myval);
		list_item._Ptr->_Myval->worker.AddAction(act);

		//Check dropped item volatility & reset delete timer
		if (list_item._Ptr->_Myval->GetIfVolatile())((Volatile_Item*)list_item._Ptr->_Myval)->ResetVolatileTimer();
		
		//Add the dropped item at the entities manager 
		App->entities_manager->AddEntity(list_item._Ptr->_Myval);
		
		list_item++;
	}
	//Clear the items to drop list
	items.clear();

	//Delete the "box" this is temporal
	App->entities_manager->DeleteEntity(this);
}
/// ---------------------------------------------

/// Volatile_Item -------------------------------
//Constructors ========================
Volatile_Item::Volatile_Item()
{

}

Volatile_Item::Volatile_Item(const Volatile_Item & copy, bool generate_body) :Item(copy, generate_body), time_to_delete(copy.time_to_delete)
{

}

//Destructors =========================
Volatile_Item::~Volatile_Item()
{

}

//Game Loop ===========================
bool Volatile_Item::Update()
{
	worker.Update();

	if (delete_timer.Read() > time_to_delete)
	{
		App->entities_manager->DeleteEntity(this);
	}

	return true;
}

void Volatile_Item::ResetVolatileTimer()
{
	delete_timer.Start();
}

//Set Methods =========================
void Volatile_Item::SetTimeToDelete(uint time)
{
	time_to_delete = time;
}

uint Volatile_Item::GetTimeToDelete() const
{
	return time_to_delete;
}
/// ---------------------------------------------

/// Coin ----------------------------------------
//Constructors ========================
Coin::Coin()
{

}

Coin::Coin(const Coin & copy, bool generate_body) :Volatile_Item(copy, generate_body), value(copy.value)
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
