#include "Render_Effects.h"

#include <math.h>
#include "j1App.h"
#include "j1Render.h"
#include "j1Audio.h"

///Render_Effect --------------------------------
// Constructors =================================
Render_Effect::Render_Effect()
{

}

Render_Effect::Render_Effect(RENDER_EF_TYPE effect_type) :effect_type(effect_type)
{

}

// Destructors ==================================
Render_Effect::~Render_Effect()
{

}

// Game Loop ====================================
bool Render_Effect::Update()
{
	return true;
}

// Set Methods ==================================
void Render_Effect::SetEffectType(RENDER_EF_TYPE type)
{
	effect_type = type;
}

// Get Methods ==================================
RENDER_EF_TYPE Render_Effect::GetEffectType() const
{
	return effect_type;
}
/// ---------------------------------------------

///Fade_Effect ----------------------------------
// Constructors =================================
Fade_Effect::Fade_Effect() : Render_Effect(FADE_EFFECT)
{

}

Fade_Effect::Fade_Effect(bool fade_music, float fade_time, float start_alpha, float end_alpha, SDL_Color color) : Render_Effect(FADE_EFFECT), fade_music(fade_music), fade_time(fade_time), start_alpha(start_alpha), end_alpha(end_alpha), current_alpha(start_alpha), color(color)
{
	if(fade_music)App->audio->StartMusicFade();
}

// Destructors ==================================
Fade_Effect::~Fade_Effect()
{

}

// Game Loop ====================================
bool Fade_Effect::Update()
{
	//Draw a rect over the viewport
	App->render->DrawQuad(App->render->viewport, color.r, color.g, color.b, current_alpha, true, false);

	//Calculate the alpha value that has to be added/rested in this frame
	float to_fade = (App->GetDT() * abs(start_alpha - end_alpha) / fade_time);
	if (start_alpha < end_alpha)
	{
		current_alpha -= to_fade;
		if (fade_music)App->audio->FadeMusicIn(fade_time);
	}
	else
	{
		current_alpha += to_fade;
		if (fade_music)App->audio->FadeMusicOut(fade_time);
	}
	
	//Check if the alpha has reached the end value
	return bool(floor(current_alpha) == end_alpha);
}



// Functionality ================================

// Set Methods ==================================

// Get Methods ==================================

