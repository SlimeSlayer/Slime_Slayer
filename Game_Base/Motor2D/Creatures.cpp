#include "Creatures.h"

#include "j1App.h"
#include "j1Physics.h"

/// Intelligent_Creature ------------------------
// Constructors =======================
Intelligent_Creature::Intelligent_Creature()
{

}

Intelligent_Creature::Intelligent_Creature(const Intelligent_Creature & copy, bool generate_body) :Creature(copy, generate_body), money(copy.money)
{
	if (generate_body)
	{
		vision_area = App->physics->TransformDefToBuilt(copy.GetVisionArea());
		vision_area->entity_related = this;
	}
	else vision_area = new PhysBody(*copy.GetVisionArea());
}

// Destructors ========================
Intelligent_Creature::~Intelligent_Creature()
{

}

// Game Loop ==========================
bool Intelligent_Creature::Update()
{
	this->UpdatePosition();

	return true;
}

// Set Methods ========================
void Intelligent_Creature::SetPosition(float x, float y)
{
	body->SetPosition(x, y);
	vision_area->SetPosition(x, y);
}

void Intelligent_Creature::SetMoney(uint money)
{
	this->money = money;
}

void Intelligent_Creature::SetVisionArea(PhysBody * new_area)
{
	vision_area = new_area;
}

// Get Methods ========================
uint Intelligent_Creature::GetMoney() const
{
	return money;
}

PhysBody * Intelligent_Creature::GetVisionArea() const
{
	return vision_area;
}

// Functionality ======================
void Intelligent_Creature::AddMoney(uint gained_money)
{
	money += gained_money;
}

void Intelligent_Creature::UpdatePosition()
{
	int x = 0,y = 0;
	body->GetPosition(x, y);
	vision_area->SetPosition((float)x + body->width, (float)y + body->height);
}

void Intelligent_Creature::HandleInteraction(Entity * target)
{

}
/// ---------------------------------------------