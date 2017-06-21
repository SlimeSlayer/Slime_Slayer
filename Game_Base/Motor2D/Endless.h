#ifndef _ENDLESS_H_
#define _ENDLESS_H_

#include "Scene.h"

class Endless : public Scene
{
public:

	Endless();
	~Endless();

public:

	bool Enable();
	void Disable();

	// Called each loop iteration
	bool Update(float dt);

private:

	//DATA

};
#endif // !_TUTORIAL_H_
