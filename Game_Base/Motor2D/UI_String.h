#ifndef _UI_STRING_
#define _UI_STRING_

#include "UI_Element.h"
#include "j1Fonts.h"

struct _TTF_Font;
struct SDL_Texture;

class UI_String : public UI_Element {
public:

	UI_String(const SDL_Rect& box, char* text, const SDL_Color& text_color = { 255,255,255,255 }, _TTF_Font* text_font = App->font->default);
	UI_String(const UI_String* copy);
	UI_String();

	~UI_String();

private:
	
	//Text
	std::string			text;
	_TTF_Font*			text_font = nullptr;
	SDL_Texture*		text_texture = nullptr;
	SDL_Color			text_color = { 255,255,255,255 };
	//Background
	bool				background = false;
	SDL_Color			back_color = { 255,255,255,0 };
	iPoint				back_margins = { 0,0 };

public:

	//Game loop
	bool		Start();
	void		Draw(bool debug) const;

	//Functionality
	char*		GetString()const;
	void		SetString(const char* new_text, bool generate = true);
	void		PushString(const char* new_text,uint position);
	bool		DeleteChar(uint position);
	bool		DeleteSegment(uint start, uint end);

	SDL_Texture*	GetTextTexture()const;
	uint			GetPixelLenght(uint end)const;
	uint			GetPixelHeightLenght()const;
	uint			GetLenght()const;

	void		DrawAt(int x, int y)const;
	bool		TokenizeString(uint margin = 0);
	bool		GenerateTexture();

	SDL_Rect	AdjustBox();
	bool		SetFont(_TTF_Font* new_font);
	void		SetColor(SDL_Color new_color);
	void		SetBack(bool back);
	void		SetBackMargins(iPoint marg);
	void		SetBackColor(SDL_Color new_color);
};

#endif