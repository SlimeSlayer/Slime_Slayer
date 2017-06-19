#include "Creatures.h"

/// Intelligent_Creature ------------------------
// Constructors =======================
Intelligent_Creature::Intelligent_Creature()
{

}

Intelligent_Creature::Intelligent_Creature(const Intelligent_Creature & copy, bool generate_body) :Creature(copy, generate_body), money(copy.money)
{

}

// Destructors ========================
Intelligent_Creature::~Intelligent_Creature()
{

}

// Set Methods ========================
void Intelligent_Creature::SetMoney(uint money)
{
	this->money = money;
}

void Intelligent_Creature::AddMoney(uint gained_money)
{
	money += gained_money;
}

// Get Methods ========================
uint Intelligent_Creature::GetMoney() const
{
	return money;
}

/// ---------------------------------------------