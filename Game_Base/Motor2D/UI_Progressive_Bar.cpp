#include "UI_Progressive_Bar.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"

//Constructors ============================================
UI_Progressive_Bar::UI_Progressive_Bar(const UI_Progressive_Bar* copy) :UI_Element(copy->box, PROGRESSIVE_BAR), max_value(copy->max_value), current_value(copy->current_value), full_color(copy->full_color), empty_color(copy->empty_color)
{

}

UI_Progressive_Bar::UI_Progressive_Bar(): UI_Element({ 0,0,0,0 }, PROGRESSIVE_BAR)
{

}

//Destructor ==============================================
UI_Progressive_Bar::~UI_Progressive_Bar()
{
	App->tex->UnLoad(bar_texture);
}

bool UI_Progressive_Bar::Update()
{
	UpdateTexture();

	UpdateChilds();

	return true;
}

// Game Loop ==============================================
void UI_Progressive_Bar::Draw(bool debug) const
{
	App->render->CallBlit(bar_texture, box.x, box.y, NULL, false, 1.0f, visual_layer);
}

void UI_Progressive_Bar::DrawAt(int x, int y) const
{
	App->render->CallBlit(bar_texture, x, y, NULL, false, 1.0f, visual_layer);
}

// Functionality ==========================================
void UI_Progressive_Bar::SetMaxValue(float max)
{
	max_value = max;
}

void UI_Progressive_Bar::SetCurrentValue(float current)
{
	current_value = current;
}

float UI_Progressive_Bar::GetMaxValue() const
{
	return max_value;
}

float UI_Progressive_Bar::GetCurrentValue() const
{
	return current_value;
}

void UI_Progressive_Bar::AddValue(float to_add)
{
	if (to_add < 0)
	{
		to_empty_val = to_add;
		current_value = MAX(0.0, current_value + to_add);
	}
	else current_value = MIN(max_value, current_value + to_add);
}

void UI_Progressive_Bar::SetToEmptyRestValue(float val)
{
	to_empty_rest_val = val;
}

void UI_Progressive_Bar::SetFullColor(SDL_Color clr)
{
	full_color = clr;
}

void UI_Progressive_Bar::SetEmptyColor(SDL_Color clr)
{
	empty_color = clr;
}

void UI_Progressive_Bar::SetToEmptyColor(SDL_Color clr)
{
	to_empty_color = clr;
}

void UI_Progressive_Bar::GenerateTexture()
{
	bar_texture = SDL_CreateTexture(App->render->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, box.w, box.h);
	App->tex->textures.push_back(bar_texture);
}

void UI_Progressive_Bar::UpdateTexture()
{
	SDL_SetRenderTarget(App->render->renderer, bar_texture);
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(App->render->renderer, full_color.r, full_color.g, full_color.b, full_color.a);
	SDL_RenderFillRect(App->render->renderer, &box);
}
