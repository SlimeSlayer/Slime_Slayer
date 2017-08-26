#ifndef _RENDER_EFFECTS_H_
#define _RENDER_EFFECTS_H_

#include "SDL/include/SDL.h"

//Effects enums -------------
enum RENDER_EF_TYPE
{
	NO_EFFECT = 0,
	FADE_EFFECT
};

///Render_Effect --------------------------------
class Render_Effect
{
public:

	Render_Effect();
	Render_Effect(RENDER_EF_TYPE effect_type);
	~Render_Effect();

public:

	virtual bool Update(); /*Return true when the effect ends*/

protected:

	RENDER_EF_TYPE	effect_type = NO_EFFECT;

public:

	//Set Methods -----------
	void SetEffectType(RENDER_EF_TYPE type);

	//Get Methods -----------
	RENDER_EF_TYPE	GetEffectType()const;

};
/// ---------------------------------------------

///Fade_Effect ----------------------------------
class Fade_Effect : public Render_Effect
{
public:

	Fade_Effect();
	Fade_Effect(bool fade_music, float fade_time, float start_alpha, float end_alpha, SDL_Color color);
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
#endif // !_RENDER_EFFECTS_H_
