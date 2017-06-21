#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

#define MID_PARALLAX_VAL 15
#define BACK_PARALLAX_VAL 30
class Parallax;
class PhysBody;
struct SDL_Texture;

class Scene : public j1Module
{
public:

	Scene();
	~Scene();

public:

	void Init();
	void Disable();

	// Called before quitting
	bool CleanUp();

protected:

	//Private Scene Data ----
	//Ground collider
	PhysBody*	floor_collider = nullptr;
	//Parallax layers
	Parallax*	front_parallax = nullptr;
	Parallax*	mid_parallax = nullptr;
	Parallax*	back_parallax = nullptr;

public:

	//Functionality ---------
	void UpdateParallax(float disp);

};

#endif // __j1SCENE_H__