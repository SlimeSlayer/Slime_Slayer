#ifndef __j1FONTS_H__
#define __j1FONTS_H__

#include "j1Module.h"
#include "SDL\include\SDL_pixels.h"

#define DEFAULT_FONT "fonts/open_sans/OpenSans-Regular.ttf"
#define DEFAULT_FONT_SIZE 12

struct SDL_Texture;
struct _TTF_Font;

enum FONT_ID
{
	NO_FONT = 0,
	DEF_FONT,
	MENU_UI_FONT,
	ALLY_INF_FONT,
	ENEMY_INF_FONT,
	DIALOG_FONT,
	ENEMY_HITPOINTS_FONT,
	ALLY_HITPOINTS_FONT,
	EXPERIENCE_FONT,
	LVL_UP_FONT
};

/// Font_Def --------------------------
struct Font_Def
{
	FONT_ID		font_id = NO_FONT;
	_TTF_Font*	font = nullptr;
	SDL_Color	font_color = { 255,255,255,255 };
};
/// -----------------------------------


class j1Fonts : public j1Module
{
public:

	j1Fonts();
	~j1Fonts();

public:
	
	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called each loop iteration
	bool PreUpdate();

	// Called each loop iteration
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	std::vector<Font_Def*>			fonts;
	_TTF_Font*						default = nullptr;

public:

	// Load Font
	_TTF_Font* const Load(const char* path, int size, SDL_Color color, FONT_ID font_id);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

	//Transform str to Font_ID
	FONT_ID StrToFontID(const char* str)const;

	Font_Def* GetFontByID(FONT_ID id)const;
};


#endif // __j1FONTS_H__