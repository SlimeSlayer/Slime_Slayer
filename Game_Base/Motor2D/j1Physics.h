#ifndef _J1PHYSICS_H_
#define _J1PHYSICS_H_

#include "j1Module.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -3.8f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum collision_type
{
	PLAYER_COLLISION = 1,
	MAP_COLLISION = 2
	/*4,8,16,32,...*/
};

enum BODY_TYPE
{
	NO_BODY,
	PLAYER_BODY,
	MAP_BODY

};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:

	PhysBody() : listener(NULL), body(NULL)
	{
	
	}

public:

	//Get data from body ----------------------------------
	void	GetPosition(int& x, int &y) const;
	float	GetRotation() const;
	int		GetWidth()const;
	int		GetHeight()const;

	//Set body data ---------------------------------------
	void	Move(int x_vel, int y_vel);
	void	FixedRotation(bool value);
	void	SetPosition(int x, int y);


	//Collide functionality -------------------------------
	bool		Contains(int x, int y) const;
	int			RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	bool		IsInContact()const;
	inline void	HandleContact(PhysBody* contact_body);

public:

	int					width = 0, height = 0;
	b2Body*				body = nullptr;
	j1Module*			listener = nullptr;
	BODY_TYPE			collide_type = NO_BODY;
	b2RevoluteJoint*	joint = nullptr;
};

// Module --------------------------------------
class j1Physics : public j1Module, public b2ContactListener
{
public:

	j1Physics();
	~j1Physics();

public:

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

public:

	//Bodys Creations -----------------
	PhysBody* CreateCircle(int x, int y, int radius, collision_type type, BODY_TYPE b_type = NO_BODY, uint restitution = 0);
	PhysBody* CreateStaticCircle(int x, int y, int radius, collision_type type, BODY_TYPE b_type = NO_BODY, uint restitution = 0);
	PhysBody* CreateRectangle(int x, int y, int width, int height, collision_type type, BODY_TYPE b_type = NO_BODY, uint restitution = 0);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, collision_type type, BODY_TYPE b_type = NO_BODY, uint restitution = 0);
	PhysBody* CreateChain(int x, int y, int* points, int size, collision_type type, BODY_TYPE b_type = NO_BODY, uint restitution = 0);
	PhysBody* CreateSensorChain(int x, int y, int* points, int size, collision_type type, BODY_TYPE b_type = NO_BODY, uint restitution = 0);

	//Fixture Creations ---------------
	void SetFixture(b2FixtureDef& fixture, collision_type type);

	//Joints Creations ----------------
	bool CreateWeldJoint(PhysBody* A, PhysBody* B);
	bool CreateRevoluteJoint(b2Body* A, b2Body* B);

	bool DeleteBody(PhysBody* target);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	void If_Sensor_contact(PhysBody* bodyA, PhysBody* bodyB);

private:

	bool			debug = false;
	b2World*		world = nullptr;
	b2MouseJoint*	mouse_joint = nullptr;
	b2Body*			ground = nullptr;

	std::list<b2Body*> bodys_to_delete;

};
#endif