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

/// Collider ----------------------------------------------
struct Collider
{
	SDL_Rect rect = { 0,0,0,0 };
	bool to_delete = false;
	COLLIDER_TYPE type = COLLIDER_NONE;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr): rect(rectangle), type(type), callback(callback)
	{
	
	}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const
	{
		return (rect.x < r.x + r.w &&
			rect.x + rect.w > r.x &&
			rect.y < r.y + r.h &&
			rect.h + rect.y > r.y);
	}
};
/// -------------------------------------------------------
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

	std::vector<Collider*>	colliders;
	bool		matrix[COLLIDER_MAX][COLLIDER_MAX];
	
};

#endif // __j1ModuleCollisions_H__