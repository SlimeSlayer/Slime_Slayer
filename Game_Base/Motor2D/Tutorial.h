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
	
	//Save / Load functions
	bool GeneralSave(pugi::xml_node& node) const;
	bool GeneralLoad(pugi::xml_node& node);

	//Collisions functions
	void BeginSensorCollision(PhysBody* A, PhysBody* B);

private:

	//DATA
	PhysBody*	end_trigger = nullptr;
	bool		tutorial_completed = false;

public:

	//Functionality
	bool TutorialCompleted()const;

};
#endif // !_TUTORIAL_H_
