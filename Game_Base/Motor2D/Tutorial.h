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

	// Called each loop iteration
	bool Update(float dt);

private:

	//DATA

};
#endif // !_TUTORIAL_H_
