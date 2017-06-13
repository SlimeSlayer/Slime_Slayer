#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

#define MID_PARALLAX_VAL 0.065
#define BACK_PARALLAX_VAL 0.8

class Parallax;
class PhysBody;
struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	void Init();

	bool Enable();
	void Disable();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	//Map Folders
	std::vector <std::string> map_folder;

private:

	//Private Scene Data ----
	PhysBody* floor_collider = nullptr;

	Parallax*	front_parallax = nullptr;
	Parallax*	mid_parallax = nullptr;
	Parallax*	back_parallax = nullptr;

public:

	//Functionality ---------
	void UpdateParallax(float disp);

};

#endif // __j1SCENE_H__