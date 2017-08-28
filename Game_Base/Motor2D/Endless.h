#ifndef _ENDLESS_H_
#define _ENDLESS_H_

#include "Scene.h"

#include "p2Point.h"
#include "j1Timer.h"
class UI_Progressive_Bar;
class UI_String;
class UI_Image;

class Endless : public Scene
{
public:

	Endless();
	~Endless();

public:

	void Init();
	bool Enable();
	void Disable();

	void RestartScene();

	// Called each loop iteration
	bool Update(float dt);

private:

	//Creatures spawn data
	j1Timer				spawn_timer;
	uint				initial_spawn_time = 0;
	uint				spawn_rate = 0;
	uint				wave = 0;
	float				wave_evolve = 0.0f;
	std::vector<iPoint> spawn_coordinates;

	//Wave passed UI
	UI_String*			wave_string = nullptr;
	UI_Progressive_Bar*	wave_progress_bar = nullptr;
	UI_Image*			wave_shield_icon = nullptr;

};
#endif // !_TUTORIAL_H_
