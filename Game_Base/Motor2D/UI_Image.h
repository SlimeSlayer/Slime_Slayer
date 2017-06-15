#ifndef _UI_IMG_
#define _UI_IMG_

#include "UI_Element.h"
#include "p2Point.h"

class UI_Image : public UI_Element {
public:

	UI_Image(const SDL_Rect& box, const iPoint pivot, const SDL_Rect& texture_rect = {0,0,0,0}, TEXTURE_ID texture_id = TEXTURE_NONE);
	UI_Image(const UI_Image* copy);
	UI_Image();

	~UI_Image();

private:

	SDL_Rect		texture_rect = { 0,0,0,0 };
	iPoint			pivot = { 0,0 };
	TEXTURE_ID		texture_id = TEXTURE_NONE;
	SDL_Texture*	texture = nullptr;

public:

	//Game Loop
	void				Draw(bool debug) const;

	//Functionality
	SDL_Rect			AdjustBox();
	void				DrawAt(int x, int y)const;
	void				ChangeTextureRect(SDL_Rect new_rect);
	void				ChangeTextureId(TEXTURE_ID texture_id);
	iPoint				GetPivot()const;
	SDL_Rect			GetTextureBox()const;
};

#endif // __UI_IMG__