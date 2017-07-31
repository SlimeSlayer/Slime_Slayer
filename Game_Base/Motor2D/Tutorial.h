#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

#include "Scene.h"

class Tutorial : public Scene 
{
public:

	Tutorial();
	~Tutorial();

public:

	void Init();
	bool Enable();
	void Disable();

	void RestartScene();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();
	
	//Collisions functions
	void BeginSensorCollision(PhysBody* A, PhysBody* B);

private:

	//DATA
	PhysBody*	end_trigger = nullptr;

};
#endif // !_TUTORIAL_H_
