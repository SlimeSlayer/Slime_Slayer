#include "j1Render.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Audio.h"

#include <math.h>
#include <stdarg.h>

///Class Blit_Call ------------------------------
//Constructors ========================
Blit_Call::Blit_Call()
{

}

Blit_Call::Blit_Call(const iPoint & position, const iPoint& pivot, SDL_Texture* texture, SDL_Rect* rect, bool use_camera, bool horizontal_flip, bool vertical_flip, float scale, int priority, uint opacity, SDL_Color color, double angle)
	:position(position), pivot(pivot), texture(texture), rect(rect), use_camera(use_camera), horizontal_flip(horizontal_flip), vertical_flip(vertical_flip), scale(scale), priority(priority), opacity(opacity), color(color), angle(angle)
{

}

//Destructors =========================
Blit_Call::~Blit_Call()
{

}

int Blit_Call::GetX() const
{
	return position.x;
}

int Blit_Call::GetY() const
{
	return position.y;
}

int Blit_Call::GetXPivot() const
{
	return pivot.x;
}

int Blit_Call::GetYPivot() const
{
	return pivot.y;
}

SDL_Texture * Blit_Call::GetTex() const
{
	return texture;
}

SDL_Color Blit_Call::GetColor() const
{
	return color;
}

const SDL_Rect* Blit_Call::GetRect() const
{
	return rect;
}

bool Blit_Call::GetUseCamera() const
{
	return use_camera;
}

bool Blit_Call::GetHorizontalFlip() const
{
	return horizontal_flip;
}

bool Blit_Call::GetVerticalFlip() const
{
	return vertical_flip;
}

float Blit_Call::GetScale() const
{
	return scale;
}

uint Blit_Call::GetOpacity() const
{
	return opacity;
}

double Blit_Call::GetAngle() const
{
	return angle;
}

bool Blit_Call::operator<(const Blit_Call & target) const
{
	if (priority > target.priority)return true;
	return false;
}

//Operator to compare blit calls in the priority queue

/// ---------------------------------------------

j1Render::j1Render() : j1Module()
{
	name = "renderer";
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		vsync = true;
				
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;

		SDL_GL_SetSwapInterval(vsync);
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);

	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	uint size = blit_queue.size();
	for (uint k = 0; k < size; k++)
	{
		const Blit_Call* blit = &blit_queue.top();
		Blit(blit->GetTex(), blit->GetX(), blit->GetY(), blit->GetRect(), blit->GetUseCamera(), blit->GetHorizontalFlip(), blit->GetVerticalFlip(), blit->GetScale(), blit->GetOpacity(), blit->GetColor(), blit->GetXPivot(), blit->GetYPivot(), 1.0f, blit->GetAngle());
		blit_queue.pop();
	}

	return true;
}

bool j1Render::PostUpdate()
{
	//Vsync -------------------------------------
	if (App->render->vsync && App->win->fullscreen)
	{
		float delay = 0;
		float current_time = (float)App->GetCurrentFrameTime();
		if (current_time < FULLSCREEN_MS_VSYNC)
		{
			delay = FULLSCREEN_MS_VSYNC - current_time;
			delay = (delay > (floor(delay) + 0.5f)) ? ceil(delay) : floor(delay);
			SDL_Delay(delay);
		}
	}
	// ------------------------------------------

	//Blit the load process bar
	if (App->is_loading && App->load_scene_enabled)
	{
		App->DrawLoadProcess();
	}
	// ------------------------------------------

	// Update the current render effect ---------
	if (!effects_queue.empty())
	{
		Render_Effect*  ef = effects_queue.front();
		if (ef->Update())
		{
			effects_queue.pop();
			RELEASE(ef);
			if(!effects_queue.empty())effects_queue.front()->Update();
		}
	}
	// ------------------------------------------

	//Render Filters ----------------------------
	/*SDL_Surface *shot = SDL_CreateRGBSurface(0, viewport.w, viewport.h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, &viewport, SDL_PIXELFORMAT_ARGB8888, shot->pixels, shot->pitch);
	for (uint y = 0; y < viewport.h; y++)
	{
		for (uint x = 0; x < viewport.w; x++)
		{
			Uint32* dest_pixel = pixels + (y * viewport.w) + x;
			unsigned char* dest_r = (unsigned char*)dest_pixel;
			*dest_r = 0;
		}
	}*/
	
	// ------------------------------------------

	// Render Present ---------------------------
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
	SDL_RenderPresent(renderer);
	// ------------------------------------------

	if (App->render->vsync && !App->win->fullscreen)SDL_GL_SwapWindow(App->win->window);


	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::PartyLoad(pugi::xml_node& node)
{
	camera.x = node.child("camera").attribute("x").as_int();
	camera.y = node.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::PartySave(pugi::xml_node& node) const
{
	pugi::xml_node cam = node.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::ChangeVSYNCstate(bool state)
{
	vsync = state;
	SDL_GL_SetSwapInterval(state);
}

bool j1Render::CallBlit(SDL_Texture * texture, int x, int y, const SDL_Rect * section,bool use_camera, bool horizontal_flip, bool vertical_flip, float scale, int priority, uint opacity, int pivot_x, int pivot_y, SDL_Color color, double angle)
{
	bool ret = true;

	if (texture == nullptr)ret = false;
	else blit_queue.emplace(iPoint(x, y), iPoint(pivot_x, pivot_y), texture, (SDL_Rect*)section, use_camera, horizontal_flip, vertical_flip, scale, priority, opacity, color, angle);
	return true;
}

void j1Render::ClearBlitQueue()
{
	while (!blit_queue.empty()) { blit_queue.pop(); }
}

Render_Effect* j1Render::CallRenderEffect(RENDER_EF_TYPE type, ...)
{
	bool ret = false;
	Render_Effect* new_effect = nullptr;
	//List of all the variables next to the type
	va_list variables;

	//Allocate all the variables that proceed the type in the va_list
	va_start(variables, type);
	switch (type)
	{
	case FADE_EFFECT:
	{
		//Collect the variables from the va_list
		bool	fade_audio = va_arg(variables, bool);
		float	fade_time = va_arg(variables, double);
		float	start_alpha = va_arg(variables, double);
		float	end_alpha = va_arg(variables, double);
		SDL_Color color = va_arg(variables, SDL_Color);
		app_function_ptr ptr = va_arg(variables, app_function_ptr);

		//Build the effect with the collected variables
		new_effect = new Fade_Effect(fade_audio, fade_time, start_alpha, end_alpha, color, ptr);
		ret = true;
	}
		break;
	case LAYER_EFFECT:
	{
		//Collect all the variables of the va_list
		SDL_Color color = va_arg(variables, SDL_Color);
		//Build the effect with the collected variables
		new_effect = new Layer_Effect(color);
		ret = true;
	}
		break;
	}

	//Clean up the list
	va_end(variables);
	
	if (ret)effects_queue.push(new_effect);

	return new_effect;
}

RENDER_EF_TYPE j1Render::GetCurrentEfType() const
{
	if (effects_queue.empty())return NO_EFFECT;
	else return effects_queue.front()->GetEffectType();
}

SPEC_EF_TYPE j1Render::GetCurrentEfSpecType() const
{
	if (effects_queue.empty())return NO_SPEC_EFFECT;
	else return effects_queue.front()->GetSpecificType();
}

const Render_Effect * j1Render::GetCurrentRenderEffect() const
{
	return effects_queue.front();
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

bool j1Render::IsOnCamera(const iPoint & point)
{
	return(-camera.x < point.x && -camera.x + camera.w > point.x 
		&& -camera.y < point.y && -camera.y + camera.h > point.y);
}

iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, bool use_camera, bool horizontal_flip, bool vertical_flip, float scale, uint opacity, SDL_Color color, int pivot_x, int pivot_y, float speed, double angle) const
{
	bool ret = true;
	uint window_scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * window_scale;
	rect.y = (int)(camera.y * speed) + y * window_scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	if (!horizontal_flip)
	{
		rect.x -= pivot_x;
		rect.y -= pivot_y;
	}
	else
	{
		rect.x -= (rect.w-pivot_x);
		rect.y -= pivot_y;
	}

	if (use_camera)
	{
		rect.x -= (int)(camera.x);
		rect.y -= (int)(camera.y);
	}

	rect.w *= (window_scale * scale);
	rect.h *= (window_scale * scale);

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != 0 && pivot_y != 0)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	int op_check = 2;
	if (opacity != 255)
	{
		op_check = SDL_SetTextureAlphaMod(texture, opacity);
	}

	bool color_change = (color.r != 255 || color.g != 255 || color.b != 255);

	if (color_change)SDL_SetTextureColorMod(texture, color.r, color.g, color.b);

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, (SDL_RendererFlip)horizontal_flip))
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	if (op_check != 2)
	{
		op_check = SDL_SetTextureAlphaMod(texture,255);
	}

	if (color_change)
	{
		color.r = color.g = color.b = 255;
		SDL_SetTextureColorMod(texture, 255, 255, 255);
	}

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = ((float)M_PI / 180.0f);
	//In case that the circle is fixed at camera coordinates
	if (!use_camera)
	{
		for (uint i = 0; i < 360; ++i)
		{
			points[i].x = (int)(x + radius * cos(i * factor));
			points[i].y = (int)(y + radius * sin(i * factor));
		}
	}
	//Else if the circle is not fixed
	else
	{
		for (uint i = 0; i < 360; ++i)
		{
			points[i].x = (int)(x + radius * cos(i * factor)) + camera.x;
			points[i].y = (int)(y + radius * sin(i * factor)) + camera.y;
		}
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawTriangle(int x1, int y1, int x2, int y2, uint length, float angle, float x_angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	//Prepare render
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	/*
	A-----M-----B
	 -		   -
	  -       -
	   -     -
	    -   -
	     - -
		  0
	*/
	//Calculate 0 -> M vector
	fPoint mid_vector(x2 - x1, y2 - y1);
	mid_vector.Norm();
	mid_vector *= length * 0.5;

	//Calculate 0 -> A vector
	fPoint A_vector;
	A_vector.x = mid_vector.x * cos((angle * 0.5)) - mid_vector.y * sin((angle * 0.5));
	A_vector.y = mid_vector.x * sin((angle * 0.5)) + mid_vector.y * cos((angle * 0.5));
	A_vector.Norm();
	A_vector *= length;
	A_vector.y *= -sin(x_angle);

	//Calculate 0 -> B vector
	fPoint B_vector;
	B_vector.x = mid_vector.x * cos((-angle * 0.5)) - mid_vector.y * sin((-angle * 0.5));
	B_vector.y = mid_vector.x * sin((-angle * 0.5)) + mid_vector.y * cos((-angle * 0.5));
	B_vector.Norm();
	B_vector *= length;
	B_vector.y *= -sin(x_angle);

	iPoint v_A, v_B;
	iPoint position(x1, y1);

	//Calculate v_A vertex with vector 0 -> A
	v_A.x = position.x + A_vector.x;
	v_A.y = position.y + A_vector.y;

	//Calculate v_B vertex with vector 0 -> B
	v_B.x = position.x + B_vector.x;
	v_B.y = position.y + B_vector.y;

	DrawTriangle(&position, &v_A, &v_B, 0,0,0, 255);

	return true;
}

bool j1Render::DrawTriangle(const iPoint* pick, const iPoint* v_A, const iPoint* v_B, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	//Draw all the calculated vectors
	App->render->DrawLine(pick->x, pick->y, v_A->x, v_A->y, r, g, b, a);
	App->render->DrawLine(pick->x, pick->y, v_B->x, v_B->y, r, g, b, a);
	App->render->DrawLine(v_A->x, v_A->y, v_B->x, v_B->y, r, g, b, a);

	return true;
}

