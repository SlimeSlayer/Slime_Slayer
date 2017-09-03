#include "UI_Progressive_Bar.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"

//Constructors ============================================
UI_Progressive_Bar::UI_Progressive_Bar(const UI_Progressive_Bar* copy) :UI_Element(copy), max_value(copy->max_value), current_value(copy->current_value), full_color(copy->full_color), empty_color(copy->empty_color), to_empty_color(copy->to_empty_color), to_empty_rest_val(copy->to_empty_rest_val), to_empty_val(copy->to_empty_val)
{
	this->ui_type = UI_TYPE::PROGRESSIVE_BAR;
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
	App->render->CallBlit(bar_texture, box.x, box.y, NULL, use_camera, false, false, 1.0f, visual_layer);
}

void UI_Progressive_Bar::DrawAt(int x, int y) const
{
	App->render->CallBlit(bar_texture, x, y, NULL, use_camera, false, false, 1.0f, visual_layer);
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
		to_empty_val += to_add;
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
	if (bar_texture != nullptr)return;

	bar_texture = SDL_CreateTexture(App->render->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, box.w, box.h);
	SDL_SetTextureBlendMode(bar_texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	App->tex->textures.push_back(bar_texture);
}

void UI_Progressive_Bar::UpdateTexture()
{
	//Focus render at the bar box
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(App->render->renderer, bar_texture);
	
	
	//Calculate the percents
	float empty_per_cent_val = 1 - (current_value / max_value);
	float full_per_cent = 1 - empty_per_cent_val;
	float to_empty_per_cent_val = abs(to_empty_val / max_value);
	SDL_Rect full_rect = { 0,0,ceil(box.w * full_per_cent),box.h };
	SDL_Rect loc_box = { 0,0,box.w,box.h };

	//Update to empty value
	to_empty_val = MAX(abs(to_empty_val) - to_empty_rest_val * App->GetDT(), 0.0f);

	//Blit the empty color
	if (empty_per_cent_val > 0.000)
	{
		SDL_SetRenderDrawColor(App->render->renderer, empty_color.r, empty_color.g, empty_color.b, empty_color.a);
		SDL_RenderFillRect(App->render->renderer, &loc_box);
	}

	//Blit the empty color
	SDL_SetRenderDrawColor(App->render->renderer, full_color.r, full_color.g, full_color.b, full_color.a);
	SDL_RenderFillRect(App->render->renderer, &full_rect);
	
	//Blit the to_empty color
	if (to_empty_per_cent_val > 0.000)
	{
		SDL_Rect to_empty_rect = { full_rect.w,0,ceil(box.w * to_empty_per_cent_val),box.h };
		SDL_SetRenderDrawColor(App->render->renderer, to_empty_color.r, to_empty_color.g, to_empty_color.b, to_empty_color.a);
		SDL_RenderFillRect(App->render->renderer, &to_empty_rect);
	}
	//Reset render color
	SDL_SetRenderDrawColor(App->render->renderer, App->render->background.r, App->render->background.g, App->render->background.b, App->render->background.a);


	//Reset render target to window
	SDL_SetRenderTarget(App->render->renderer, NULL);
}

SDL_Texture * UI_Progressive_Bar::GetTexture() const
{
	return bar_texture;
}
