#define _CRT_SECURE_NO_WARNINGS
#include "UI_String.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Log.h"

//Constructors ============================================
UI_String::UI_String(const SDL_Rect& box, char * text, const SDL_Color& text_color, _TTF_Font * text_font) : UI_Element(box, STRING), text(text), text_font(text_font), text_color(text_color) {}

UI_String::UI_String(const UI_String* copy) : UI_Element(copy->box, STRING), text(copy->text), text_font(copy->text_font), text_texture(copy->text_texture), text_color(copy->text_color), background(copy->background), back_color(copy->back_color), back_margins(copy->back_margins) {}

UI_String::UI_String() : UI_Element({0,0,0,0}, STRING), text(""), text_font(nullptr) {}

//Destructor ==============================================
UI_String::~UI_String()
{
	if (text_texture != nullptr)
	{
		if (!App->tex->UnLoad(text_texture));
	}
	text_texture = nullptr;
	text_font = nullptr;
}


//Game Loop ===============================================
bool UI_String::Start()
{
	GenerateTexture();
	return true;
}

void UI_String::Draw(bool debug) const
{
	//This Draw
	
	if (debug)
	{
		if(!use_camera)App->render->DrawQuad({ box.x - App->render->camera.x,box.y - App->render->camera.y,box.w,box.h }, 255, 255, 255);
		else App->render->DrawQuad({ box.x,box.y,box.w,box.h }, 255, 255, 255);
	}
	if (text_texture != nullptr)
	{
		App->render->CallBlit(text_texture, box.x, box.y, NULL, use_camera, false, 1.0f, visual_layer);
	}

	//Childs Draw
	DrawChilds(debug);
}


//Functionality ===========================================
char * UI_String::GetString() const
{
	return (char*)text.c_str();
}

uint UI_String::GetLenght()const
{
	return uint(text.length());
}

void UI_String::SetString(const char * new_text, bool generate)
{
	text = new_text;
	if (generate)GenerateTexture();
}

void UI_String::PushString(const char * new_text, uint position)
{
	if (text_texture != nullptr)
	{
		App->tex->UnLoad(text_texture);
		text_texture = nullptr;
	}
	text.insert(position, new_text);
	text_texture = App->font->Print(text.c_str(), text_color, text_font);
}

bool UI_String::DeleteChar(uint position)
{
	if (text.length() == 0) return false;

	text.erase(position,1);
	
	if (text_texture != nullptr) {
		App->tex->UnLoad(text_texture);
		text_texture = nullptr;
	}

	if(text.length() > 0)text_texture = App->font->Print(text.c_str(), text_color, text_font);
	return true;
}

bool UI_String::DeleteSegment(uint start, uint end)
{
	//Check for bad input
	if (start > text.length() || end > text.length() || end < start)return false;
	
	//Delete str chars
	bool ret = true;
	for (uint k = start; k < end; k++)
	{
		text.erase(start,1);
		if (!ret)break;
	}

	if (text_texture != nullptr) {
		App->tex->UnLoad(text_texture);
		text_texture = nullptr;
	}
	//Generate str texture if it contain alive chars
	if (ret && text.length() > 0)text_texture = App->font->Print(text.c_str(), text_color, text_font);

	return ret;
}

SDL_Texture * UI_String::GetTextTexture() const
{
	return text_texture;
}

uint UI_String::GetPixelLenght(uint end) const
{
	std::string segment = text.substr(0, end);
	int x, y;
	App->font->CalcSize(segment.c_str(), x, y, text_font);
	return x;
}

uint UI_String::GetPixelHeightLenght() const
{
	int x, y;
	App->font->CalcSize("T", x, y, text_font);
	return y;
}

void UI_String::DrawAt(int x, int y) const
{
	if (text_texture != nullptr)App->render->CallBlit(text_texture, box.x + x, box.y + y, NULL, use_camera);
}

bool UI_String::TokenizeString(uint margin)
{
	


	int w, h;
	int w_size = 0, h_size = 0;
	char* copy = (char*)text.c_str();
	char* token = strtok(copy, "\n");
	SDL_Texture* texture = nullptr;
	std::list<char*>		texttokens;
	std::list<SDL_Texture*>	texttextures;

	while (token != NULL) {

		texture = App->font->Print(token, text_color, App->font->default);
		texttextures.push_back(texture);
		SDL_QueryTexture(texture, NULL,NULL, &w, &h);
		texttokens.push_back(token);
		h_size += h + margin;
		if (w > w_size)w_size = w;
		token = strtok(NULL, "\n");

	}
	
	text_texture = SDL_CreateTexture(App->render->renderer, SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_TARGET, w_size, h_size);
	SDL_SetRenderTarget(App->render->renderer, text_texture);

	uint k = 0;
	std::list<SDL_Texture*>::iterator texture_item = texttextures.begin();
	while (texture_item._Ptr != NULL)
	{
		
		App->render->CallBlit(texture_item._Ptr->_Myval, 0, (h * k) + margin);
		texture_item++;
		k++;
	}

	SDL_SetTextureBlendMode(text_texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(App->render->renderer, NULL);
	return true;
}

bool UI_String::GenerateTexture()
{
	if (text.length() == 0)return false;
	if (strlen(text.c_str()) > 0)
	{
		//Unload old texture
		if (text_texture != nullptr)
		{
			App->tex->UnLoad(text_texture);
		}
		//Generate new text texture
		text_texture = App->font->Print(text.c_str(), text_color, text_font);

		if (background)
		{
			//Generate background texture
			//Build texture rect
			uint t_w = 0, t_h = 0;
			App->tex->GetSize(text_texture, t_w, t_h);
			SDL_Rect back_tex_rect = { 0,0,t_w + back_margins.x * 2, t_h + back_margins.y * 2 };
			//Generate base texture
			SDL_Texture* back_texture = SDL_CreateTexture(App->render->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, back_tex_rect.w, back_tex_rect.h);
			//Focus and blit back color on it
			SDL_SetRenderTarget(App->render->renderer, back_texture);
			SDL_SetRenderDrawColor(App->render->renderer, back_color.r, back_color.g, back_color.b, back_color.a);
			SDL_RenderFillRect(App->render->renderer, &back_tex_rect);
			//Blit text texture over
			App->render->Blit(text_texture, back_margins.x, back_margins.y);

			//Delete text texture and set fused texture as text texture
			App->tex->UnLoad(text_texture);
			text_texture = back_texture;
			 
			//Reset render target
			SDL_SetRenderTarget(App->render->renderer, NULL);
			//Reset render color
			SDL_SetRenderDrawColor(App->render->renderer, App->render->background.r, App->render->background.g, App->render->background.b, App->render->background.a);
		}
		return true;
	}
	return false;
}

SDL_Rect UI_String::AdjustBox()
{
	int w, h;
	SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);
	box.w = w;
	box.h = h;

	return box;
}

bool UI_String::SetFont(_TTF_Font * new_font)
{
	if (new_font == nullptr)return false;
	
	text_font = new_font;
	return true;
}

void UI_String::SetColor(SDL_Color new_color)
{
	text_color = new_color;
}

void UI_String::SetBack(bool back)
{
	background = back;
}

void UI_String::SetBackMargins(iPoint marg)
{
	back_margins = marg;
}

void UI_String::SetBackColor(SDL_Color new_color)
{
	back_color = new_color;
}

