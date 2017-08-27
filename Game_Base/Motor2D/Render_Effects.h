#ifndef _RENDER_EFFECTS_H_
#define _RENDER_EFFECTS_H_

#define FADE_OUT_TIME 2.0f
#define FADE_IN_TIME 1.2f
#define FADE_MARGIN 1.5f

#include "SDL/include/SDL.h"

class j1App;

//Effects enums -------------
enum RENDER_EF_TYPE
{
	NO_EFFECT = 0,
	FADE_EFFECT,
	LAYER_EFFECT
};

enum SPEC_EF_TYPE
{
	NO_SPEC_EFFECT = 0,
	FADE_IN_EFFECT,
	FADE_OUT_EFFECT
};

///Render_Effect --------------------------------
typedef void(j1App::*app_function_ptr)();
class Render_Effect
{
public:

	Render_Effect();
	Render_Effect(RENDER_EF_TYPE effect_type,app_function_ptr function_ptr);
	~Render_Effect();

public:

	virtual bool Update(); /*Return true when the effect ends*/

protected:

	RENDER_EF_TYPE	effect_type = NO_EFFECT;
	SPEC_EF_TYPE	specific_type = NO_SPEC_EFFECT;


public:

	//Pointer to the callback function
	app_function_ptr function_pointer;

	//Set Methods -----------
	void SetEffectType(RENDER_EF_TYPE type);
	//void SetFunctionPtr(void(j1App::*f_ptr));

	//Get Methods -----------
	RENDER_EF_TYPE	GetEffectType()const;
	SPEC_EF_TYPE	GetSpecificType()const;

};
/// ---------------------------------------------

///Fade_Effect ----------------------------------
class Fade_Effect : public Render_Effect
{
public:

	Fade_Effect();
	Fade_Effect(bool fade_music, float fade_time, float start_alpha, float end_alpha, SDL_Color color, app_function_ptr function_ptr);
	~Fade_Effect();

public:

	bool Update();

protected:

	bool		fade_music = true;
	float		fade_time = 1.0f;
	float		start_alpha = 0.0f;
	float		end_alpha = 0.0f;
	float		current_alpha = 0.0f;
	SDL_Color	color = { 255,255,255,255 };

};
/// ---------------------------------------------

///Layer_Effect ---------------------------------
class Layer_Effect : public Render_Effect
{
public:

	Layer_Effect();
	Layer_Effect(SDL_Color color);
	~Layer_Effect();

public:

	bool Update();

protected:

	SDL_Color	color = { 255,255,255,255 };
	bool		end_ready = false;
public:

	void End();
};
/// ---------------------------------------------

#endif // !_RENDER_EFFECTS_H_
