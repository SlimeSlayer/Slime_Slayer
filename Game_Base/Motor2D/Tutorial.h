#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

#include "Scene.h"

class Tutorial : public Scene 
{
public:

	Tutorial();
	~Tutorial();

public:

	bool Enable();
	void Disable();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();
	
	//Collisions functions
	void OnCollision(PhysBody* A, PhysBody* B);

private:

	//DATA
	PhysBody*	end_trigger = nullptr;
};
#endif // !_TUTORIAL_H_
