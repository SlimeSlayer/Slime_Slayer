#ifndef __j1ENTITIES_MANAGER_H__
#define __j1ENTITIES_MANAGER_H__

#include "j1Module.h"
#include "BaseEntities.h"

class j1EntitiesManager : public j1Module
{
public:

	j1EntitiesManager();
	~j1EntitiesManager();

public:

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

private:

	std::list<Creature*> current_creatures;

public:

	// Functionality --------
	Creature* GenerateCreature(CREATURE_TYPE creature_type);

};
#endif