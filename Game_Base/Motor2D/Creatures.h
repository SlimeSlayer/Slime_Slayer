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
	void Clean();

public:

	//Game Loop -------------
	bool Update();

protected:

	PhysBody*	vision_area = nullptr;

public:

	//Set Methods -----------
	void SetPosition(float x, float y);
	void SetVisionArea(PhysBody* new_area);

	//Get Methods -----------
	PhysBody*	GetVisionArea()const;

	//Functionality ---------
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

	bool		input_blocked = false;

	uint		attack_range = 0;
	j1Timer		attack_timer;
	PhysBody*	current_attack_area = nullptr;

	uint		experience = 0;				/*Current experience*/
	uint		next_lvl_experience = 0;	/*Necessary exp to level up*/
	float		experience_scale = 0.0f;	/*The number that multiply the next lvl experience*/

public:

	//Set Methods -----------
	void SetAttackRange(uint range);
	void SetCurrentAttackArea(PhysBody* new_attack_area);
	void SetExperience(uint experience);
	void SetExperienceScale(float exp_scale);
	void SetNextLevelExperience(uint exp);

	//Get Methods -----------
	bool		GetInputBlocked()const;
	uint		GetAttackRange()const;
	PhysBody*	GetCurrentAttackArea()const;
	uint		GetExperience()const;
	float		GetExperienceScale()const;
	uint		GetNextLevelExperience()const;

	//Functionality ---------
	void LockInput();
	void UnlockInput();

	void AttackLeft();
	void AttackRight();
	bool ReadyToAttack()const;

	void AddExperience(uint gained_exp);
};
/// ---------------------------------------------
#endif // !_CREATURES_H_
