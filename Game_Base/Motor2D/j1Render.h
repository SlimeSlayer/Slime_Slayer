#ifndef __j1RENDER_H__
#define __j1RENDER_H__

#include "SDL/include/SDL.h"
#include "p2Point.h"
#include "j1Module.h"
#include <queue>
#include "Render_Effects.h"

#define FULLSCREEN_MS_VSYNC 16

///Class Blit_Call ------------------------------
//Little class to save all blit calls and organize them in a priority queue
class Blit_Call
{
public:

	Blit_Call();
	Blit_Call(const iPoint& position, const iPoint& pivot, SDL_Texture* texture, SDL_Rect* rect, bool use_camera, bool flip, float scale, int priority, uint opacity, SDL_Color color, double angle);
	~Blit_Call();

private:

	iPoint			position = { 0,0 };
	bool			use_camera = false;
	iPoint			pivot = { 0,0 };
	SDL_Texture*	texture = nullptr;
	SDL_Color		color = { 255,255,255,255 };
	SDL_Rect*		rect = nullptr;
	bool			flip = false;
	float			scale = 1.0f;
	uint			opacity = 255;
	int				priority = 0;
	double			angle = 0.0;


public:

	//Get Methods -----------
	int				GetX()const;
	int				GetY()const;
	int				GetXPivot()const;
	int				GetYPivot()const;
	SDL_Texture*	GetTex()const;
	SDL_Color		GetColor()const;
	const SDL_Rect*	GetRect()const;
	bool			GetUseCamera()const;
	bool			GetFlip()const;
	float			GetScale()const;
	uint			GetOpacity()const;
	double			GetAngle()const;

	//Operator to compare blit calls in the priority queue
	bool operator<(const Blit_Call& target) const;

};
/// ---------------------------------------------

class j1Render : public j1Module
{
public:

	j1Render();

	// Destructor
	virtual ~j1Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Save / Load functions -----------
	bool PartyLoad(pugi::xml_node& node);
	bool PartySave(pugi::xml_node& node) const;

private:

	//Priority queue to organize all the blits
	std::priority_queue<Blit_Call> blit_queue;

	//Queue of the render effects
	std::queue<Render_Effect*> effects_queue;

public:

	bool vsync = false;
	SDL_Renderer*	renderer = nullptr;
	SDL_Rect		camera = { 0,0,0,0 };
	SDL_Rect		viewport = { 0,0,0,0 };
	SDL_Rect		camera_viewport = { 0,0,0,0 };
	SDL_Color		background = { 0,0,0,0 };
	
public:

	//Add blit call ---------
	bool	CallBlit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, bool use_camera = false, bool horizontal_flip = false, float scale = 1.0f, int priority = 0, uint opacity = 255, int pivot_x = 0, int pivot_y = 0, SDL_Color color = { 255,255,255,255 }, double angle = 0.0);
	void	ClearBlitQueue();

	//Add render effect ------
	/*
	- FADE_EFFECT: bool fade_music, float fade_time. float start_alpha, float end_alpha, SDL_Color color
	*/
	bool	CallRenderEffect(RENDER_EF_TYPE type, ...);

	//Render effect methods --
	RENDER_EF_TYPE	GetCurrentEfType()const;
	const Render_Effect*	GetCurrentRenderEffect()const;

	//View port Methods ------
	void	SetViewPort(const SDL_Rect& rect);
	void	ResetViewPort();
	//Check if point is on camera view
	bool	IsOnCamera(const iPoint& point);
	//View frame change -----
	iPoint	ScreenToWorld(int x, int y) const;

	// Draw & Blit ----------
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, bool use_camera = false, bool horizontal_flip = false, float scale = 1.0f, uint opacity = 255, SDL_Color color = {255,255,255,255}, int pivot_x = 0, int pivot_y = 0, float speed = 1.0f, double angle = 0) const;

	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawTriangle(int x1, int y1, int x2, int y2, uint length, float angle, float x_angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
	bool DrawTriangle(const iPoint* pick, const iPoint* v_A, const iPoint* v_B, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
	
	// Set background color -
	void SetBackgroundColor(SDL_Color color);

	//Active/Deactivate vsync
	void ChangeVSYNCstate(bool state);

};

#endif // __j1RENDER_H__