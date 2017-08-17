#include "Creatures.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Physics.h"
#include "j1Gui.h"
#include "j1ParticleManager.h"
#include "j1EntitiesManager.h"
#include "j1Render.h"
#include "j1Animator.h"
#include "j1Player.h"
#include "UI_String.h"


/// Evolution_Template --------------------------
// Constructors =======================
Evolution_Template::Evolution_Template()
{
}

Evolution_Template::Evolution_Template(const Evolution_Template & copy) :creature_type(copy.creature_type), life(copy.life), attack_hitpoints(copy.attack_hitpoints), money(copy.money)
{

}

// Destructors ========================
Evolution_Template::~Evolution_Template()
{

}
/// ---------------------------------------------

/// Intelligent_Creature ------------------------
// Constructors =======================
Intelligent_Creature::Intelligent_Creature()
{

}

Intelligent_Creature::Intelligent_Creature(const Intelligent_Creature & copy, bool generate_body) :Creature(copy, generate_body)
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

void Intelligent_Creature::Clean()
{
	RELEASE(body);
	RELEASE(vision_area);
}

// Game Loop ==========================
bool Intelligent_Creature::Update()
{
	worker.Update();

	UpdatePosition();

	life_bar.Update();

	return true;
}

// Set Methods ========================
void Intelligent_Creature::SetPosition(float x, float y)
{
	body->SetPosition(x, y);
	vision_area->SetPosition(x, y);
}

void Intelligent_Creature::SetVisionArea(PhysBody * new_area)
{
	vision_area = new_area;
}

// Get Methods ========================
PhysBody * Intelligent_Creature::GetVisionArea() const
{
	return vision_area;
}

// Functionality ======================
void Intelligent_Creature::UpdatePosition()
{
	int x = 0,y = 0;
	body->GetPosition(x, y);
	vision_area->SetPosition((float)x, (float)y);
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

Player::Player(const Player & copy, bool generate_body) :Intelligent_Creature(copy, generate_body), attack_range(copy.attack_range), experience(copy.experience), experience_scale(copy.experience_scale), next_lvl_experience(copy.next_lvl_experience)
{
	current_attack_area = App->physics->TransformDefToBuilt(copy.GetCurrentAttackArea());
	current_attack_area->entity_related = this;
}

// Destructors ========================
Player::~Player()
{
	RELEASE(current_attack_area);
}

void Player::SetCurrentLife(uint new_life)
{
	float vl = new_life;
	life_bar.AddValue(vl - current_life);
	App->player->life_bar->AddValue(vl - current_life);
	current_life = new_life;
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

void Player::SetExperience(uint experience)
{
	this->experience = experience;
}

void Player::SetExperienceScale(float exp_scale)
{
	experience_scale = exp_scale;
}

void Player::SetNextLevelExperience(uint exp)
{
	next_lvl_experience = exp;
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

uint Player::GetExperience() const
{
	return experience;
}

float Player::GetExperienceScale() const
{
	return experience_scale;
}

uint Player::GetNextLevelExperience() const
{
	return next_lvl_experience;
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
	current_attack_area->SetPosition(x - w * 1.5, y + h * -0.25);
	//Reset attack timer
	attack_timer.Start();
}

void Player::AttackRight()
{
	//Place the attack area at the correct coordinates
	int x = 0, y = 0, w = 0, h = 0;
	this->body->GetPosition(x, y);
	w = this->body->GetWidth();
	h = this->body->GetHeight();
	current_attack_area->SetPosition(x + w * 1.5, y + h * -0.25);
	//Reset attack timer
	attack_timer.Start();
}

bool Player::ReadyToAttack() const
{
	return bool(attack_timer.Read() > attack_rate);
}

void Player::AddExperience(uint gained_exp)
{
	experience += gained_exp;

	//Check if player avatar has the necessary xp to lvl up
	if (experience >= this->next_lvl_experience)
	{
		//Update the avatar level
		experience = experience - next_lvl_experience;
		level++;
		next_lvl_experience = ceil(experience_scale * next_lvl_experience);

		//Update avatar stats
		App->entities_manager->LevelUpCreature(this);

		//Generate a output particle 
		App->particle_manager->GenerateTextParticle(this, PARTICLE_TYPE::LEVEL_UP_PARTICLE, NULL);
	}

	//Update player UI 
	App->player->exp_bar->SetMaxValue(this->GetNextLevelExperience());
	App->player->exp_bar->SetCurrentValue(this->GetExperience());
	App->player->exp_bar->UpdateTexture();
}
/// ---------------------------------------------


