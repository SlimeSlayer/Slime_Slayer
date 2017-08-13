#include "j1Fonts.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1FileSystem.h"

#include "SDL\include\SDL.h"
#include "SDL_TTF\include\SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

j1Fonts::j1Fonts() : j1Module()
{
	name = "fonts";
}

// Destructor
j1Fonts::~j1Fonts()
{}

// Called before render is available
bool j1Fonts::Awake(pugi::xml_node& conf)
{
	LOG("Init True Type Font library");
	bool ret = true;

	if(TTF_Init() == -1)
	{
		LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		ret = false;
	}
	else
	{
		const char* path = conf.child("default_font").attribute("file").as_string(DEFAULT_FONT);
		int size = conf.child("default_font").attribute("size").as_int(DEFAULT_FONT_SIZE);
		FONT_ID font_id = StrToFontID(conf.child("default_font").attribute("id").as_string());
		SDL_Color font_color = {
			conf.child("default_font").attribute("r").as_uint(),
			conf.child("default_font").attribute("g").as_uint(),
			conf.child("default_font").attribute("b").as_uint(),
			conf.child("default_font").attribute("a").as_uint()
		};
		default = Load(path, size, font_color, font_id);
	}

	pugi::xml_node font_node = conf.child("font");
	while (font_node.root() != NULL)
	{
		const char* path = font_node.attribute("file").as_string(DEFAULT_FONT);
		int size = font_node.attribute("size").as_int(DEFAULT_FONT_SIZE);
		FONT_ID font_id = StrToFontID(font_node.attribute("id").as_string());
		SDL_Color font_color = {
			font_node.attribute("r").as_uint(),
			font_node.attribute("g").as_uint(),
			font_node.attribute("b").as_uint(),
			font_node.attribute("a").as_uint()
		};
		Load(path, size, font_color, font_id);

		font_node = font_node.next_sibling();
	}

	return ret;
}

bool j1Fonts::PreUpdate()
{
	return true;
}

bool j1Fonts::PostUpdate()
{
	return true;
}

// Called before quitting
bool j1Fonts::CleanUp()
{
	uint size = fonts.size();
	for (uint k = 0; k < size; k++)
	{
		TTF_CloseFont(fonts[k]->font);
		RELEASE(fonts[k]);
	}
	fonts.clear();

	TTF_Quit();
	return true;
}

// Load new texture from file path
TTF_Font* const j1Fonts::Load(const char* path, int size, SDL_Color color, FONT_ID font_id)
{
	TTF_Font* font = TTF_OpenFontRW(App->fs->Load(path), 1, size);
	
	if(font == NULL)
	{
		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
	}
	else
	{
		Font_Def* font_def = new Font_Def();
		LOG("Successfully loaded font %s size %d", path, size);
		font_def->font = font;
		font_def->font_color = color;
		font_def->font_id = font_id;
		fonts.push_back(font_def);
	}

	return font;
}

SDL_Texture * j1Fonts::Print(const char * text, SDL_Color color, _TTF_Font * font)
{
	SDL_Texture* ret = NULL;
	SDL_Surface* surface = TTF_RenderText_Blended((font) ? font : default, text, color);

	if (surface == NULL)
	{
		LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		ret = App->tex->LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return ret;
}

// calculate size of a text
bool j1Fonts::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret = false;

	if(TTF_SizeText((font) ? font : default, text, &width, &height) != 0)
		LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());
	else
		ret = true;

	return ret;
}

FONT_ID j1Fonts::StrToFontID(const char * str) const
{
	if (strcmp("def_font", str) == 0)				return DEF_FONT;
	if (strcmp("menu_ui_font", str) == 0)			return MENU_UI_FONT;
	if (strcmp("dialog_font", str) == 0)			return DIALOG_FONT;
	if (strcmp("ally_hitpoints_font", str) == 0)	return ALLY_HITPOINTS_FONT;
	if (strcmp("enemy_hitpoints_font", str) == 0)	return ENEMY_HITPOINTS_FONT;
	if (strcmp("experience_font", str) == 0)		return EXPERIENCE_FONT;
	if (strcmp("lvl_up_font", str) == 0)			return LVL_UP_FONT;
	return NO_FONT;
}

Font_Def * j1Fonts::GetFontByID(FONT_ID id) const
{
	uint size = fonts.size();
	for (uint k = 0; k < size; k++)
	{
		if (fonts[k]->font_id == id)return fonts[k];
	}
	return nullptr;
}
