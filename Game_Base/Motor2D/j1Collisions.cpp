#include "j1Collisions.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1InputManager.h"

j1Collisions::j1Collisions()
{
	uint size = colliders.size();
	for (uint i = 0; i < size; ++i)
	{
		colliders[i] = nullptr;
	}

	//WALL
	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;

	//PLAYER
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;

}

// Destructor
j1Collisions::~j1Collisions()
{}

bool j1Collisions::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	uint size = colliders.size();
	for (uint i = 0; i < size; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

// Called before render is available
bool j1Collisions::Update(float dt)
{
	//DEBUG -------------------------------------
	if (App->input_manager->GetEvent(COLLIDERS_DEBUG_MODE) == INPUT_DOWN)
	{
		App->collisions_debug = !App->collisions_debug;
	}
	// ------------------------------------------


	Collider* c1 = nullptr;
	Collider* c2 = nullptr;

	uint size = colliders.size();
	for (uint i = 0; i < size; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		uint size = colliders.size();
		for (uint k = i + 1; k < size; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	if(App->collisions_debug)DebugDraw();

	return true;
}

void j1Collisions::DebugDraw()
{
	Uint8 alpha = 80;
	uint size = colliders.size();
	for (uint i = 0; i < size; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_WALL: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 50, 250, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		}
	}
}

// Called before quitting
bool j1Collisions::CleanUp()
{
	LOG("Freeing all colliders");
	uint size = colliders.size();
	for (uint i = 0; i < size; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = new Collider(rect, type, callback);;

	colliders.push_back(ret);

	return ret;
}

bool j1Collisions::EraseCollider(Collider* collider)
{
	if (collider != nullptr)
	{
		// we still search for it in case we received a dangling pointer
		uint size = colliders.size();
		for (uint i = 0; i < size; ++i)
		{
			if (colliders[i] == collider)
			{
				collider->to_delete = true;
				break;
			}
		}
	}

	return false;
}

// -----------------------------------------------------
