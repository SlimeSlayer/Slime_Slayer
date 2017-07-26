#ifndef _CREATURES_H_
#define _CREATURES_H_

#ifndef _BASE_ENTITIES_H_
#include "BaseEntities.h"
#endif 

#include <vector>

class PhysBody;
class UI_String;

/// Intelligent_Creature ------------------------
class Intelligent_Creature : public Creature
{
public:

	Intelligent_Creature();
	Intelligent_Creature(const Intelligent_Creature& copy, bool generate_body = true);
	~Intelligent_Creature();

public:

	//Game Loop -------------
	bool Update();

protected:

	uint money = 0;

	PhysBody*	vision_area = nullptr;

public:

	//Set Methods -----------
	void SetPosition(float x, float y);
	void SetMoney(uint money);
	void SetVisionArea(PhysBody* new_area);

	//Get Methods -----------
	uint		GetMoney()const;
	PhysBody*	GetVisionArea()const;

	//Functionality ---------
	void AddMoney(uint gained_money);
	void UpdatePosition();

	virtual void HandleInteraction(Entity* target);
};
/// ---------------------------------------------

/// NPC -----------------------------------------
class NPC : public Intelligent_Creature
{
public:

	NPC();
	NPC(const NPC& copy, bool generate_body = true);
	~NPC();

protected:

	std::vector<UI_String*> dialog_strings;

public:

	//Actions ---------------
	void StartDialog(Player* target);

	//Functionality ---------
	void		AddDialogStr(const UI_String* str);
	UI_String*	GetDialogByIndex(uint index);
};
/// ---------------------------------------------

/// Player --------------------------------------
class Player : public Intelligent_Creature
{
public:

	Player();
	Player(const Player& copy, bool generate_body = true);
	~Player();

private:

	bool	input_blocked = false;

	uint		attack_range = 0;
	j1Timer		attack_timer;
	PhysBody*	current_attack_area = nullptr;

public:

	//Set Methods -----------
	void SetAttackRange(uint range);
	void SetCurrentAttackArea(PhysBody* new_attack_area);

	//Get Methods -----------
	bool		GetInputBlocked()const;
	uint		GetAttackRange()const;
	PhysBody*	GetCurrentAttackArea()const;

	//Functionality ---------
	void LockInput();
	void UnlockInput();

	void AttackLeft();
	void AttackRight();
	bool ReadyToAttack()const;
};
/// ---------------------------------------------
#endif // !_CREATURES_H_
