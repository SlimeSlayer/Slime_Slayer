#ifndef _J1PLAYER_H_
#define _J1PLAYER_H_

#include "j1Module.h"
#include "p2Point.h"

#define PLAYER_CAMERA_X 500
#define PLAYER_CAMERA_Y 800
#define DEFAULT_DIALOG_X 600
#define DEFAULT_DIALOG_Y 700

class Player;

class j1Player : public j1Module
{
public:

	j1Player();
	~j1Player();

public:

	void Init();
	bool Enable();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

public:

	//User avatar -----------
	Player* avatar = nullptr;

	//Dialogs Location ------
	iPoint dialogs_location = { DEFAULT_DIALOG_X,DEFAULT_DIALOG_Y };
};
#endif // !_J1PLAYER_H_
