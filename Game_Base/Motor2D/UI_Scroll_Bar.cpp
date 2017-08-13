#include "UI_Scroll_Bar.h"

#include "j1App.h"
#include "j1Render.h"

// Constructors =================================
UI_Scroll_Bar::UI_Scroll_Bar()
{

}

// Destructors ==================================
UI_Scroll_Bar::~UI_Scroll_Bar()
{

}

// Game Loop ====================================

void UI_Scroll_Bar::Draw(bool debug) const
{
	//Draw Scroll & Content View Area Quad -
	if (debug)
	{
		App->render->DrawQuad({ box.x - App->render->camera.x, box.y - App->render->camera.y, box.w, box.h }, 150, 150, 0);
		App->render->DrawQuad({ box.x + ScrollBack.GetBox()->x - App->render->camera.x,box.y + ScrollBack.GetBox()->y - App->render->camera.y,ScrollBack.GetBox()->w, ScrollBack.GetBox()->h }, 90, 20, 0);
		App->render->DrawQuad({ box.x + ScrollItem.GetBox()->x - App->render->camera.x,box.y + ScrollItem.GetBox()->y - App->render->camera.y,ScrollItem.GetBox()->w, ScrollItem.GetBox()->h }, 90, 80, 110);
	}

	//Draw the scroll img ------------------
	ScrollBack.DrawAt(box.x, box.y);
	ScrollItem.DrawAt(box.x, box.y);

	//Draw Scroll Childs -------------------
	DrawChilds(debug);
}

bool UI_Scroll_Bar::CleanUp()
{
	return true;
}

// Set Methods ==================================
void UI_Scroll_Bar::SetScrollableItem(const iPoint position, const SDL_Rect rect, TEXTURE_ID id)
{
	ScrollItem = UI_Image({ position.x,position.y,rect.w,rect.h }, { 0,0 }, rect, id);
	ScrollItem.SetVisualLayer(this->visual_layer + 2);
}

void UI_Scroll_Bar::SetScrollableBack(const iPoint position, const SDL_Rect rect, TEXTURE_ID id)
{
	ScrollBack = UI_Image({ position.x,position.y,rect.w,rect.h }, { 0,0 }, rect, id);
	ScrollBack.SetVisualLayer(this->visual_layer + 1);
}

void UI_Scroll_Bar::SetScrollMaxValue(int maxvalue)
{
	MaxValue = maxvalue;
	if (Value > MaxValue)Value = MaxValue;
}

void UI_Scroll_Bar::SetScrollValue(float scrollvalue)
{
	Value = scrollvalue;

	if (Scroll_Type == LATERAL_BAR)
	{
		float percent = scrollvalue /MaxValue;
		ScrollItem.SetBoxPosition(ScrollItem.GetBox()->x + percent * ScrollBack.GetBox()->w, ScrollItem.GetBox()->y);
	}
}

void UI_Scroll_Bar::SetScrollBarType(SCROLL_BAR_TYPE type)
{
	Scroll_Type = type;
}

// Get Methods ==================================
float UI_Scroll_Bar::GetValue() const
{
	return Value;
}

// Functionality ================================
bool UI_Scroll_Bar::MoveScroll(int mouse_y_motion, int mouse_x_motion)
{
	//Select the Scroll Item ----------
	if (ScrollItem.MouseIsIn(box.x, box.y) || ScrollSelected || (App->gui->controller_mode && App->gui->ItemSelected == this))
	{
		ScrollSelected = true;
	}

	//Drag the Scroll Item ------------
	if (ScrollSelected)
	{
		if (Scroll_Type == VERTICAL_BAR || Scroll_Type == VERTICAL_INV_BAR)
		{
			mouse_x_motion = 0;

			if (ScrollItem.RectIsIn(ScrollBack.GetBox(), mouse_x_motion, mouse_y_motion) == false)
			{
				//Set at limits
				if (mouse_y_motion < 0)
				{
					mouse_y_motion = ScrollBack.GetBox()->y - ScrollItem.GetBox()->y;
				}
				else if (mouse_y_motion > 0)
				{
					mouse_y_motion = ScrollBack.GetBox()->y + ScrollBack.GetBox()->h - (ScrollItem.GetBox()->h + ScrollItem.GetBox()->y);
				}
			}

			//Update scroll position
			ScrollPosition += mouse_y_motion;

			//Update scroll value
			Value = ((ScrollItem.GetBox()->y - ScrollBack.GetBox()->y) * MaxValue) / (float)(ScrollBack.GetBox()->h - ScrollItem.GetBox()->h);
		}
		else if (Scroll_Type == LATERAL_BAR || Scroll_Type == LATERAL_INV_BAR)
		{
			mouse_y_motion = 0;

			if (ScrollItem.RectIsIn(ScrollBack.GetBox(), mouse_x_motion, mouse_y_motion, true) == false)
			{
				if (mouse_x_motion < 0)
				{
					mouse_x_motion = -(ScrollItem.GetBox()->x - ScrollBack.GetBox()->x);
				}
				else if (mouse_x_motion > 0)
				{
					mouse_x_motion = (ScrollBack.GetBox()->x + ScrollBack.GetBox()->w) - (ScrollItem.GetBox()->x + ScrollItem.GetBox()->w);
				}
			}

			//Update scroll position
			ScrollPosition += mouse_x_motion;

			//Update scroll value
			Value = ((ScrollItem.GetBox()->x - ScrollBack.GetBox()->x) * MaxValue) / (float)(ScrollBack.GetBox()->w - ScrollItem.GetBox()->w);
		}

		//Move the scroll Item ---
		ScrollItem.MoveBox(mouse_x_motion, mouse_y_motion);

	}

	return ScrollSelected;
}

void UI_Scroll_Bar::GoBottom()
{
	//Move the scroll Item ---
	ScrollItem.MoveBox(0, ScrollBack.GetBox()->y + ScrollBack.GetBox()->h - (ScrollItem.GetBox()->h + ScrollItem.GetBox()->y));

	//Update scroll value ----
	Value = ((ScrollItem.GetBox()->y - ScrollBack.GetBox()->y) * MaxValue) / (float)(ScrollBack.GetBox()->h - ScrollItem.GetBox()->h);
}

void UI_Scroll_Bar::UnselectScroll()
{
	ScrollSelected = false;
}

void UI_Scroll_Bar::RecalculateScrollValue()
{
	if (Scroll_Type == VERTICAL_BAR || Scroll_Type == VERTICAL_INV_BAR)
	{
		Value = ((ScrollItem.GetBox()->y - ScrollBack.GetBox()->y) * MaxValue) / (float)(ScrollBack.GetBox()->h - ScrollItem.GetBox()->h);
	}
	else
	{
		Value = ((ScrollItem.GetBox()->x - ScrollBack.GetBox()->x) * MaxValue) / (float)(ScrollBack.GetBox()->w - ScrollItem.GetBox()->w);
	}
}
