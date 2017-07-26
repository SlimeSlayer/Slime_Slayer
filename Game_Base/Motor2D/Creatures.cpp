#include "Creatures.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Physics.h"
#include "j1Gui.h"

#include "UI_String.h"

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
	if (vision_area != nullptr)RELEASE(vision_area);
}

// Game Loop ==========================
bool Intelligent_Creature::Update()
{
	worker.Update();

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

/// NPC -----------------------------------------
// Constructors =======================
NPC::NPC()
{

}

NPC::NPC(const NPC & copy, bool generate_body) :Intelligent_Creature(copy, generate_body)
{
	uint size = copy.dialog_strings.size();
	for (uint k = 0; k < size; k++)
	{
		this->AddDialogStr(new UI_String(copy.dialog_strings[k]));
	}
}

// Destructors ========================
NPC::~NPC()
{
	uint size = dialog_strings.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(dialog_strings[k]);
	}
	dialog_strings.clear();
}

// Actions ============================
void NPC::StartDialog(Player* target)
{
	Action* act = worker.GenerateDialogAction(this, target);
	worker.AddAction(act);
}

// Functionality ======================
void NPC::AddDialogStr(const UI_String* str)
{
	dialog_strings.push_back((UI_String*)str);
}

UI_String * NPC::GetDialogByIndex(uint index)
{
	if (index >= dialog_strings.size())return nullptr;
	return dialog_strings[index];
}
/// ---------------------------------------------

/// Player --------------------------------------
// Constructors =======================
Player::Player()
{

}

Player::Player(const Player & copy, bool generate_body) :Intelligent_Creature(copy, generate_body), attack_range(copy.attack_range)
{
	current_attack_area = App->physics->TransformDefToBuilt(copy.GetCurrentAttackArea());
}

// Destructors ========================
Player::~Player()
{
	RELEASE(current_attack_area);
}

// Set Methods ========================
void Player::SetAttackRange(uint range)
{
	attack_range = range;
}

void Player::SetCurrentAttackArea(PhysBody * new_attack_area)
{
	current_attack_area = new_attack_area;
}

// Get Methods ========================
bool Player::GetInputBlocked() const
{
	return input_blocked;
}

uint Player::GetAttackRange() const
{
	return attack_range;
}

PhysBody * Player::GetCurrentAttackArea() const
{
	return current_attack_area;
}

// Functionality ======================
void Player::LockInput()
{
	input_blocked = true;
}

void Player::UnlockInput()
{
	input_blocked = false;
}

void Player::AttackLeft()
{
	//Place the attack area at the correct coordinates
	int x = 0, y = 0, w = 0, h = 0;
	this->body->GetPosition(x, y);
	w = this->body->GetWidth();
	h = this->body->GetHeight();
	current_attack_area->SetPosition(x - w * 0.5, y + h * 0.5);
	attack_timer.Start();
}

void Player::AttackRight()
{
	attack_timer.Start();
}
bool Player::ReadyToAttack() const
{
	return bool(attack_timer.Read() > attack_rate);
}
/// ---------------------------------------------