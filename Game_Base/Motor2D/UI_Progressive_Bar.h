#ifndef _UI_PROGRESSIVE_BAR_
#define _UI_PROGRESSIVE_BAR_

#include "UI_Element.h"

class UI_Progressive_Bar : public UI_Element
{
public:
	
	UI_Progressive_Bar(const UI_Progressive_Bar* copy);
	UI_Progressive_Bar();
	~UI_Progressive_Bar();

protected:

	float			max_value = 0.0;
	float			current_value = 0.0;
	SDL_Color		full_color = { 255,255,255,255 };
	SDL_Color		empty_color = { 255,255,255,255 };
	SDL_Color		to_empty_color = { 255,255,255,255 };
	float			to_empty_rest_val = 0.0;
	float			to_empty_val = 0.0;
	SDL_Texture*	bar_texture = nullptr;

public:

	//Game Loop
	bool	Update();
	void	Draw(bool debug)const;
	void	DrawAt(int x, int y)const;

	//Functionality
	//Value functions
	void	SetMaxValue(float max);
	void	SetCurrentValue(float current);
	float	GetMaxValue()const;
	float	GetCurrentValue()const;
	void	AddValue(float to_add);
	void	SetToEmptyRestValue(float val);

	//Color functions
	void	SetFullColor(SDL_Color clr);
	void	SetEmptyColor(SDL_Color clr);
	void	SetToEmptyColor(SDL_Color clr);

	//Texture functions
	void	GenerateTexture();
	void	UpdateTexture();
};
#endif