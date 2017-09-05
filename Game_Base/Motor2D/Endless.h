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

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	//Save / Load functions
	bool PartySave(pugi::xml_node& node) const;
	bool PartyLoad(pugi::xml_node& node);

private:

	//Creatures spawn data
	j1Timer				spawn_timer;
	uint				next_spawn_time = 0;
	uint				initial_spawn_time = 0;
	uint				spawn_rate = 0;
	std::vector<iPoint> spawn_coordinates;

	//Creatures count variables
	uint	start_creatures = 0;
	int		current_alive_creatures = 0;
	int		current_defeat_creatures = 0;
	uint	next_wave_creatures = 0;
	
	//Wave related data
	float	wave_evolve = 0.0f;
	uint	wave = 0;
	uint	effect_waves = 0;

	//Wave passed UI
	bool				wave_animation = false;
	float				anim_alpha = 0.01f;
	float				bar_anim_add = 0.0f;
	float				bar_anim_added = 0.0f;
	j1Timer				anim_timer;
	uint				anim_duration = 0;
	UI_String*			wave_string = nullptr;
	UI_String*			wave_percent_string = nullptr;
	UI_Progressive_Bar*	wave_progress_bar = nullptr;
	UI_Image*			wave_shield_icon = nullptr;

public:

	void CreaturesCount(uint defs);

};
#endif // !_TUTORIAL_H_
