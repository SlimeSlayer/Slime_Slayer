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

	void Init();
	bool Enable();
	void Disable();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

private:

	// Data used during the segmented enable process
	pugi::xml_document	definitions_doc;
	pugi::xml_node		current_enable_group;
	pugi::xml_node		current_enble_node;
	ENTITY_TYPE			current_entity_type = NO_ENTITY;

	//Add definitions methods
	void AddCreatureDefinition(const pugi::xml_node* data_node);
	
	// Vector with all the creatures definitions
	std::vector<Creature*>	creatures_defs;

	// List with all the alive creatures
	std::list<Creature*> current_creatures;

public:

	//Enums Methods ---------
	ENTITY_TYPE		StrToEntityType(const char* str)const;
	CREATURE_TYPE	StrToCreatureType(const char* str)const;

	// Functionality --------
	Creature*	GenerateCreature(CREATURE_TYPE creature_type);
};
#endif