#ifndef _J1PLAYER_H_
#define _J1PLAYER_H_

#include "j1Module.h"
#include "p2Point.h"

#define PLAYER_CAMERA_X 500
#define PLAYER_CAMERA_Y 800
#define DEFAULT_DIALOG_Y_MARGIN 35

class Player;
class UI_Image;
class UI_String;
class UI_Progressive_Bar;

class j1Player : public j1Module
{
public:

	j1Player();
	~j1Player();

public:

	void Init();
	bool Enable();
	void Disable();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	//Save / Load functions
	bool PartySave(pugi::xml_node& node) const;
	bool PartyLoad(pugi::xml_node& node);

	// Collisions functions
	void OnSensorCollision(PhysBody* A, PhysBody* B);

private:

	//Data used during initial load
	pugi::xml_document data_doc;
	
	//Player data -----------
	uint	extra_live_money = 0;

public:

	//User UI ---------------
	UI_Element*			avatar_ui_branch = nullptr;
	UI_Image*			avatar_icon = nullptr;
	UI_Progressive_Bar*	life_bar = nullptr;
	UI_Progressive_Bar*	exp_bar = nullptr;
	UI_String*			money_string = nullptr;

	//User avatar -----------
	Player* avatar = nullptr;

public:

	//Get Methods -----------
	uint	GetExtraLifeMoney()const;

	//Functionality ---------
	void PlayerDeath();

};
#endif // !_J1PLAYER_H_
