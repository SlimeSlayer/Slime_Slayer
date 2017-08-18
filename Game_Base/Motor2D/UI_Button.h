#ifndef _UI_BUTTON_
#define _UI_BUTTON_

#include "UI_Element.h"
#include "UI_Image.h"

enum BUTTON_STATE {

	ON,
	OFF,
	OVER

};

class UI_Button : public UI_Element {
public:

	UI_Button(const SDL_Rect& box, const UI_Image& tex_on, const UI_Image& tex_off, const UI_Image& tex_over);
	UI_Button(const UI_Button* copy);
	UI_Button();

	~UI_Button();

public:

	UI_Image			tex_on;
	UI_Image			tex_off;
	UI_Image			tex_over;
	BUTTON_STATE		button_state;

public:

	//Game Loop
	void			Draw(bool debug) const;
	void			HandleInput();

	//Functionality
	BUTTON_STATE	Change_State(BUTTON_STATE new_button_state);
	BUTTON_STATE	Get_State()const;
	void			SetTexON(SDL_Rect img_rect, TEXTURE_ID text_id = TEXTURE_NONE);
	void			SetTexOFF(SDL_Rect img_rect, TEXTURE_ID text_id = TEXTURE_NONE);
	void			SetTexOVER(SDL_Rect img_rect, TEXTURE_ID text_id = TEXTURE_NONE);
	void			UnBlock();
	void			SetVisualLayer(uint new_layer);
	void			SetLogicalLayer(uint new_layer);
	void			SetTextureScale(float scale);
	SDL_Rect		AdjustBox();
};

#endif // __UI_BUTTON__