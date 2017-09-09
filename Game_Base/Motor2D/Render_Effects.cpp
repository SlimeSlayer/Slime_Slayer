#include "Render_Effects.h"

#include "p2Log.h"
#include <math.h>
#include "j1App.h"
#include "j1Render.h"
#include "j1Audio.h"

///Render_Effect --------------------------------
// Constructors =================================
Render_Effect::Render_Effect()
{

}

Render_Effect::Render_Effect(RENDER_EF_TYPE effect_type, app_function_ptr function_pointer) :effect_type(effect_type), function_pointer(function_pointer)
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

/*void Render_Effect::SetFunctionPtr(void(*f_ptr))
{
	function_ptr = f_ptr;
}*/

// Get Methods ==================================
RENDER_EF_TYPE Render_Effect::GetEffectType() const
{
	return effect_type;
}
SPEC_EF_TYPE Render_Effect::GetSpecificType() const
{
	return specific_type;
}
/// ---------------------------------------------

///Fade_Effect ----------------------------------
// Constructors =================================
Fade_Effect::Fade_Effect() : Render_Effect(FADE_EFFECT, nullptr)
{

}

Fade_Effect::Fade_Effect(bool fade_music, float fade_time, float start_alpha, float end_alpha, SDL_Color color,app_function_ptr function_ptr) : Render_Effect(FADE_EFFECT, function_ptr), fade_music(fade_music), fade_time(fade_time), start_alpha(start_alpha), end_alpha(end_alpha), current_alpha(start_alpha), color(color)
{
	if(fade_music)App->audio->StartMusicFade();
	if (start_alpha < end_alpha)specific_type = FADE_OUT_EFFECT;
	else specific_type = FADE_IN_EFFECT;
}

// Destructors ==================================
Fade_Effect::~Fade_Effect()
{

}

// Game Loop ====================================
bool Fade_Effect::Update()
{
	//Check current alpha to avoid timing bug
	current_alpha > 255 ? current_alpha = 255 : current_alpha < 0.1 ? current_alpha = 0 : current_alpha;
	
	//Draw a rect over the viewport
	App->render->DrawQuad(App->render->viewport, color.r, color.g, color.b, current_alpha, true, false);

	//Calculate the alpha value that has to be added/rested in this frame
	float to_fade = (App->GetDT() * abs(start_alpha - end_alpha) / fade_time);
	if (start_alpha > end_alpha)
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
	bool ret = (MIN(abs(current_alpha - end_alpha), FADE_MARGIN) < FADE_MARGIN);
	if (ret && function_pointer != nullptr)
	{
		LOG("PTR!");
		//(App->*function_pointer)();
		App->load_scene_enabled = App->is_loading = false;
	}

	return ret;
}
/// ---------------------------------------------

///Layer_Effect ---------------------------------
// Constructors =================================
Layer_Effect::Layer_Effect() :Render_Effect(LAYER_EFFECT, nullptr)
{

}

Layer_Effect::Layer_Effect(SDL_Color color) : Render_Effect(LAYER_EFFECT, nullptr), color(color)
{

}

// Destructors ==================================
Layer_Effect::~Layer_Effect()
{

}

// Game Loop ====================================
bool Layer_Effect::Update()
{
	//Draw a colorized rect over the viewport constantly
	App->render->DrawQuad(App->render->viewport, color.r, color.g, color.b, color.a, true, false);
	
	return end_ready;
}

void Layer_Effect::End()
{
	end_ready = true;
}
