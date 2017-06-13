#ifndef _PARALLAX_H_
#define _PARALLAX_H_

#include "SDL/include/SDL_rect.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "p2Log.h"

class Parallax
{
public:

	//Constructor =====================
	Parallax(uint cells_num)
	{
		parallax_cells.reserve(cells_num);
		this->cells_num = cells_num;
		for (uint k = 0; k < cells_num; k++)
		{
			fPoint cell = { 0.0f,0.0f };
			parallax_cells.push_back(cell);
		}
	}

	//Destructor ======================
	~Parallax()
	{
		App->tex->UnLoad(texture);
	}

private:

	SDL_Rect				viewport = { 0,0,0,0 };
	
	SDL_Texture*			texture = nullptr;
	SDL_Rect				texture_rect = { 0,0,0,0 };
	
	std::vector<fPoint>		parallax_cells;
	uint					cells_num = 0;

public:

	//Game Loop Methods ===============
	void Displace(float val)
	{
		for (uint k = 0; k < cells_num; k++)
		{
			parallax_cells[k].x += val;
		}
	}

	void Draw(int priority)
	{
		uint count = 0;
		for (uint k = 0; k < cells_num; k++)
		{
			//Only draw the textures inside the viewport
			if(!(parallax_cells[k].x + texture_rect.w < -App->render->camera.x || parallax_cells[k].x > -App->render->camera.x + App->render->camera.w))
			{
				float x = parallax_cells[k].x;
				x = (x > (floor(x) + 0.5f)) ? ceil(x) : floor(x);
				App->render->CallBlit(texture, x, parallax_cells[k].y, &texture_rect, false, priority);
				count++;
			}
			else
			{
				//Move parallax cells when moving right
				if (parallax_cells[0].x + texture_rect.w < -App->render->camera.x)
				{
					for (uint g = 0; g < cells_num; g++)
					{
						parallax_cells[g].x += texture_rect.w;
					}
					
				}
				//Move parallax cells when moving left
				else if (parallax_cells[cells_num - 1].x > -App->render->camera.x )
				{
					for (uint g = 0; g < cells_num; g++)
					{
						parallax_cells[g].x -= texture_rect.w;
					}
				}
			}
		}
	}

	//Set Methods =====================
	void SetTexture(SDL_Texture* new_texture)
	{
		texture = new_texture;
	}

	void SetTextureRect(SDL_Rect new_texture_rect)
	{
		texture_rect = new_texture_rect;
	}

	void SetPosition(int x, int y)
	{
		parallax_cells[0].x = x;
		parallax_cells[0].y = y;

		for (uint k = 1; k < cells_num; k++)
		{
			parallax_cells[k].x = parallax_cells[k-1].x + texture_rect.w;
			parallax_cells[k].y = parallax_cells[0].y;
		}
	}

};
#endif // !_PARALLAX_H_
