#ifndef __ModuleCollision_H__
#define __ModuleCollision_H__

#define MAX_COLLIDERS 500

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_WALL,
	COLLIDER_PLAYER,
	COLLIDER_MAX // Used to initialize the matrix
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collisions : public j1Module
{
public:

	j1Collisions();
	~j1Collisions();

public:

	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

public:

	Collider*	AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	bool		EraseCollider(Collider* collider);
	void		DebugDraw();

private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = true;

};

#endif // __j1ModuleCollisions_H__