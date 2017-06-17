#ifndef _UI_SCROLL_BAR_
#define _UI_SCROLL_BAR_

#include "UI_Element.h"
#include "UI_Image.h"

enum SCROLL_BAR_TYPE 
{
	VERTICAL_BAR,
	VERTICAL_INV_BAR,
	LATERAL_BAR,
	LATERAL_INV_BAR

};

class UI_Scroll_Bar : public UI_Element 
{
public:

	UI_Scroll_Bar();
	~UI_Scroll_Bar();

private:

	SCROLL_BAR_TYPE			Scroll_Type;
	bool					ScrollSelected = false;

	uint					ContentLenght = 0;
	uint					ScrollPosition = 0;

	float					Value = 0.0f;
	int						MaxValue = 0;

public:

	UI_Image				ScrollItem;
	UI_Image				ScrollBack;

public:

	//Game Loop -------------
	void Draw(bool debug) const;
	bool CleanUp();
	
	//Set Methods -----------
	void SetScrollableItem(const iPoint position, const SDL_Rect rect, TEXTURE_ID id = TEXTURE_NONE);
	void SetScrollableBack(const iPoint position, const SDL_Rect rect, TEXTURE_ID id = TEXTURE_NONE);
	void SetScrollMaxValue(int maxvalue);
	void SetScrollValue(float scrollvalue);
	void SetScrollBarType(SCROLL_BAR_TYPE type);

	//Get Methods -----------
	float GetValue()const;

	//Functionality ---------
	bool MoveScroll(int mouse_y_motion, int mouse_x_motion);
	void GoBottom();
	void UnselectScroll();
	void RecalculateScrollValue();
};
#endif