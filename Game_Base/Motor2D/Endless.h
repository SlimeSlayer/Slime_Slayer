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

	// Called before render is available
	bool Awake(pugi::xml_node& data_node);

	// Called each loop iteration
	bool Update(float dt);

private:

	//Creatures spawn data
	j1Timer				spawn_timer;
	uint				next_spawn_time = 0;
	uint				initial_spawn_time = 0;
	uint				spawn_rate = 0;
	uint				wave = 0;
	std::vector<iPoint> spawn_coordinates;

	//Creatures count variables
	uint	start_creatures = 0;
	int		current_creatures = 0;
	uint	next_wave_creatures = 0;
	float	wave_evolve = 0.0f;

	//Wave passed UI
	bool				wave_animation = false;
	float				anim_alpha = 0.01f;
	j1Timer				anim_timer;
	uint				anim_duration = 0;
	UI_String*			wave_string = nullptr;
	UI_Progressive_Bar*	wave_progress_bar = nullptr;
	UI_Image*			wave_shield_icon = nullptr;

public:

	void CreaturesCount(uint defs);

};
#endif // !_TUTORIAL_H_
