#ifndef _J1PLAYER_H_
#define _J1PLAYER_H_

#include "j1Module.h"

class Creature;

class j1Player : public j1Module
{
public:

	j1Player();
	~j1Player();

public:

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

private:

	Creature* avatar = nullptr;

};
#endif // !_J1PLAYER_H_
