#include "j1App.h"
#include "j1InputManager.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Physics.h"
#include "p2Point.h"
#include "math.h"
#include "j1Audio.h"
#include "Scene.h"
#include "p2Log.h"
#include <assert.h>

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

/// PhysBodyDef -------------------------------------------
// Constructors =================================
PhysBodyDef::PhysBodyDef()
{

}

PhysBodyDef::PhysBodyDef(const PhysBodyDef & copy) :shape_type(copy.shape_type), collision_type(copy.collision_type), body_type(copy.body_type), body_interaction_type(copy.body_interaction_type), restitution(copy.restitution), friction(copy.friction), density(copy.density), width(copy.width), height(copy.height), fixed_rotation(copy.fixed_rotation), is_sensor(copy.is_sensor), listener(copy.listener)
{

}

// Destructors ==================================
PhysBodyDef::~PhysBodyDef()
{

}
/// -------------------------------------------------------

/// PhysBody ----------------------------------------------
// Constructors =================================
PhysBody::PhysBody() : listener(NULL), body(NULL)
{

}

PhysBody::PhysBody(const PhysBody & copy) : body_type(copy.body_type), collision_type(copy.collision_type), width(copy.width), height(copy.height), listener(copy.listener), entity_related(copy.entity_related)
{
	if (copy.body != nullptr)
	{
		body = App->physics->CopyBody(copy.body);
		body->SetUserData(this);
	}
	if (copy.body_def != nullptr)
	{
		body_def = new PhysBodyDef(*copy.body_def);
	}
}

// Destructors ==================================
PhysBody::~PhysBody()
{
	if (body != nullptr)
	{
		body->SetUserData(nullptr);
		App->physics->DeleteBody(body);
	}
	if (body_def != nullptr)
	{
			RELEASE(body_def);
	}
}

// Get Methods ==================================
void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

int PhysBody::GetWidth() const
{
	return width;
}

int PhysBody::GetHeight() const
{
	return height;
}

// Set Methods ==================================
void PhysBody::Move(int x_vel, int y_vel)
{
	int x, y;
	GetPosition(x, y);
	x += x_vel;
	y += y_vel;
	x = PIXEL_TO_METERS(x);
	y = PIXEL_TO_METERS(y);
	SetPosition(x, y);
}

void PhysBody::FixedRotation(bool value)
{
	body->SetFixedRotation(value);
}

void PhysBody::SetPosition(float x, float y)
{
	if (body == NULL)return;

	float x_meters = PIXEL_TO_METERS(x);
	float y_meters = PIXEL_TO_METERS(y);
	b2Vec2 position((float32)x_meters, (float32)y_meters);
	body->SetTransform(position, body->GetAngle());
}

void PhysBody::Interpolate()
{
	b2Vec2 new_pos = body->GetPosition() + App->GetDT() * body->GetLinearVelocity();
	body->SetTransform(new_pos, body->GetAngle());
}

// Collision Methods ============================
bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

bool PhysBody::IsInContact()const
{
	return  (body->GetContactList() != nullptr);
}

inline void PhysBody::HandleContact(PhysBody* contact_body)
{
	/*//Check if collided body is in the bottom
	b2Vec2 this_location = this->body->GetWorldPoint(b2Vec2(0, PIXEL_TO_METERS(this->height)));
	b2Vec2 contact_location = contact_body->body->GetWorldPoint(b2Vec2(0, 0));
	bool at_bottom = (this_location.y < contact_location.y);



	b2Vec2 body_vel = body->GetLinearVelocity();
	switch (contact_body->collide_type)
	{
	case none:		return;		break;

	case map:
	break;
	case mini_blob:
	if (collide_type == player_mouth)
	{
	(((j1Scene*)App->current_scene))->BlobContact();
	LOG("K");
	}
	break;

	case map_item:
	if (collide_type == player && at_bottom)
	{
	App->player->Impact();
	}
	break;

	case player_mouth:
	if (collide_type == bullet)
	{
	App->player->PickBullet(App->player->FindBullet(this));
	}
	break;

	case platform_black:
	if (collide_type == player && at_bottom)
	{
	App->player->Die();
	}
	else if (collide_type == mini_blob && at_bottom)
	{
	((j1Scene*)App->current_scene)->Reset();
	}
	else if (collide_type == bullet)
	{
	App->player->DeleteBullet(App->player->FindBullet(this));
	}
	break;

	case platform_blue:
	break;

	case platform_green:
	if (at_bottom)
	{
	body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, -App->player->GetVerticalAcceleration()));
	}
	break;

	case platform_yellow:
	if (at_bottom)
	{
	body_vel *= 1.05f;
	body->SetLinearVelocity(body_vel);
	}
	break;

	case platform_purple:
	if (collide_type == player && at_bottom)
	{
	App->player->Impact();
	App->player->Regenerate();
	}
	break;

	case platform_red:
	if (collide_type == player && at_bottom)
	{
	App->player->Impact();
	App->player->Bleed();
	}
	break;

	case platform_orange:
	if (body_vel.y < 0 && at_bottom)
	{
	body_vel.y *= 0;
	body->SetLinearVelocity(body_vel);
	App->player->Impact();
	}
	break;
	}*/
}

void j1Physics::BeginContact(b2Contact* contact)
{

	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (!contact->GetFixtureA()->IsSensor() && !contact->GetFixtureB()->IsSensor())
	{
		if (physA != nullptr && physA->listener != NULL)
			physA->listener->BeginCollision(physA, physB);

		if (physB != nullptr && physB->listener != NULL)
			physB->listener->BeginCollision(physB, physA);
	}
	else if (contact->GetFixtureA()->IsSensor() ^ contact->GetFixtureB()->IsSensor() && physA != nullptr && physB != nullptr)
	{
		if(contact->GetFixtureA()->IsSensor())physA->listener->BeginSensorCollision(physA, physB);
		else physB->listener->BeginSensorCollision(physB, physA);
	}
}
/// -------------------------------------------------------

/// Module ------------------------------------------------
j1Physics::j1Physics() : j1Module()
{
	name = "physics";

	world = NULL;
	mouse_joint = NULL;
}

// Destructor
j1Physics::~j1Physics()
{
}

void j1Physics::Init()
{
	active = false;
	enabled = false;
}

void j1Physics::Disable()
{
	//Clean the bodies to delete list ---------------------
	if (!bodys_to_delete.empty())
	{
		std::list<b2Body*>::iterator item = bodys_to_delete.begin();
		while (item != bodys_to_delete.end())
		{
			if(item._Ptr->_Myval != NULL)world->DestroyBody(item._Ptr->_Myval);

			item++;
		}
		bodys_to_delete.clear();
	}

	b2Body* bdy = world->GetBodyList();
	while (bdy != nullptr)
	{
		b2Body* bdy_next = bdy->GetNext();
		world->DestroyBody(bdy);
		bdy = bdy_next;
	}

	enabled = active = false;
}

bool j1Physics::Awake(pugi::xml_node& config) {

	LOG("ModulePhysics Configuration Loaded!");
	return true;
}

bool j1Physics::Start()
{
	LOG("Creating Physics 2D environment");

	// Create physic world ------------
	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetAutoClearForces(false);
	world->SetContactListener(this);

	// Create world ground ------------
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

// 
bool j1Physics::PreUpdate()
{
	//Track timing values to generate a fixed time step -------------
	fixed_timestep_accumulator += App->GetDT();

	const int steps_number = static_cast<int> (std::floor(fixed_timestep_accumulator / FIXED_TIMESTEP)); /*static_cast is used here because the conversion is safe (no need run-time check)*/

	if (steps_number > 0)
	{
		fixed_timestep_accumulator -= steps_number * FIXED_TIMESTEP;
	}

	assert("Low Framerate",fixed_timestep_accumulator < FIXED_TIMESTEP); /*If the framerate is lower than the needed for Physics simulation breakpoints*/

	const int steps_clamped = MIN(steps_number, MAX_STEPS);

	for (uint k = 0; k < steps_clamped; k++)
	{
		world->Step(FIXED_TIMESTEP, 8, 4);
	}
	
	if(steps_clamped)world->ClearForces(); /*Finally we clear the world forces*/

	SmoothStates(); /*Interpolate the world bodies to smooth the frames without physics step*/
	// --------------------------------------------------------------
	

	// Iterates all the world contact to call the listeners --
	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		
		if (c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureB()->GetBody()->GetUserData();

			if (pb1 == nullptr || pb2 == nullptr)
			{
				continue;
			}

			if (!c->GetFixtureA()->IsSensor() && !c->GetFixtureB()->IsSensor())
			{
				
				if (pb1->body_type != BODY_TYPE::MAP_BODY && pb2->body_type != BODY_TYPE::MAP_BODY && pb1->listener)
				{
					pb1->listener->OnCollision(pb1, pb2);
					pb2->listener->OnCollision(pb2, pb1);
				}
			}
			else if (c->GetFixtureA()->IsSensor() ^ c->GetFixtureB()->IsSensor())
			{
				if(c->GetFixtureA()->IsSensor())pb1->listener->OnSensorCollision(pb1, pb2);
				else pb2->listener->OnSensorCollision(pb2, pb1);
			}

		}
	}
	// --------------------------------------------------------------

	return true;
}

// 
bool j1Physics::PostUpdate()
{
	//Clean the bodies to delete list ---------------------
	if (!bodys_to_delete.empty())
	{
		std::list<b2Body*>::iterator item = bodys_to_delete.begin();
		while (item != bodys_to_delete.end())
		{
			if (item._Ptr->_Myval != NULL)world->DestroyBody(item._Ptr->_Myval);

			item++;
		}
		bodys_to_delete.clear();
	}

	//Iterate all world bodies to apply contact effects ---
	b2Body* body = world->GetBodyList();
	b2ContactEdge* edge = nullptr;
	while (body)
	{

		edge = body->GetContactList();
		while (edge)
		{
			((PhysBody*)body->GetUserData())->HandleContact(((PhysBody*)edge->contact->GetFixtureA()->GetBody()->GetUserData()));
			((PhysBody*)body->GetUserData())->HandleContact(((PhysBody*)edge->contact->GetFixtureB()->GetBody()->GetUserData()));

			edge = edge->next;
		}
		body = body->GetNext();
	}

	//Active/Deactivate physic debug mode ------------------
	if (App->input_manager->GetEvent(COLLIDERS_DEBUG_MODE) == INPUT_DOWN)
	{
		App->collisions_debug = !App->collisions_debug;
	}

	if (!App->collisions_debug) return true;


	//Iterate all world bodies for draw & active player mouse input
	bool body_clicked = false;
	b2Body* click_body = nullptr;
	b2Vec2 m_pos = { PIXEL_TO_METERS(App->input->GetMouseX()) - PIXEL_TO_METERS(App->render->camera.x) ,PIXEL_TO_METERS(App->input->GetMouseY()) + PIXEL_TO_METERS(-App->render->camera.y) };

	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			SDL_Color draw_color = { 255,255,255,255 };
			switch (((PhysBody*)f->GetBody()->GetUserData())->body_type)
			{
			case COLLISION_TYPE::ITEM_COLLISION:
			case COLLISION_TYPE::STATIC_ITEM_COLLISION:
				draw_color.r = 0;
				break;
			case COLLISION_TYPE::MAP_COLLISION:
				draw_color.g = 0;
				break;
			case COLLISION_TYPE::PLAYER_COLLISION:
				draw_color.b = 0;
				break;
			}

			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->render->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), draw_color.r, draw_color.g, draw_color.b);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), draw_color.r, draw_color.g, draw_color.b);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), draw_color.r, draw_color.g, draw_color.b);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), draw_color.r, draw_color.g, draw_color.b);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), draw_color.r, draw_color.g, draw_color.b);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->render->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), draw_color.r, draw_color.g, draw_color.b);
			}
			break;
			}


			//Handle mouse input to check if a body is selected
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				if (f->TestPoint(m_pos))
				{
					click_body = f->GetBody();
					body_clicked = true;
				}
			}

		}

	}

	b2MouseJointDef def;

	if (body_clicked)
	{
		def.bodyA = ground;
		def.bodyB = click_body;
		def.target = m_pos;
		def.dampingRatio = 0.5f;
		def.frequencyHz = 2.0f;
		def.maxForce = 100.0f * click_body->GetMass();
		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);

	}

	if (mouse_joint && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		mouse_joint->SetTarget(m_pos);
		App->render->DrawLine((App->input->GetMouseX()) - App->render->camera.x, (App->input->GetMouseY()) - App->render->camera.y, METERS_TO_PIXELS(mouse_joint->GetAnchorB().x), METERS_TO_PIXELS(mouse_joint->GetAnchorB().y), 255, 0, 0);
	}

	if (mouse_joint && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
	}

	return true;
}


// Called before quitting
bool j1Physics::CleanUp()
{
	LOG("Destroying physics world");

	//Clean the bodies to delete list ---------------------
	if (!bodys_to_delete.empty())
	{
		std::list<b2Body*>::iterator item = bodys_to_delete.begin();
		while (item != bodys_to_delete.end())
		{
			if (item._Ptr->_Myval != NULL)world->DestroyBody(item._Ptr->_Myval);

			item++;
		}
		bodys_to_delete.clear();
	}

	RELEASE(world);

	return true;
}


PhysBody* j1Physics::CreateCircle(int x, int y, int radius, COLLISION_TYPE type, BODY_TYPE b_type, uint restitution)
{
	//Body
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	b2Body* b = world->CreateBody(&body);

	//Shape
	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);

	//Fixture
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = restitution;
	SetFixture(fixture, type);
	b->CreateFixture(&fixture);

	//PhysBody
	PhysBody* pbody = new PhysBody();
	pbody->body_type = b_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* j1Physics::CreateStaticCircle(int x, int y, int radius, COLLISION_TYPE type, BODY_TYPE b_type, uint restitution)
{
	//Body
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	b2Body* b = world->CreateBody(&body);

	//Shape
	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);

	//Fixture
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = restitution;
	SetFixture(fixture, type);
	b->CreateFixture(&fixture);

	//PhysBody
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->body_type = b_type;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* j1Physics::CreateRectangle(int x, int y, int width, int height, COLLISION_TYPE type, BODY_TYPE b_type, uint restitution)
{
	//Body
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	b2Body* b = world->CreateBody(&body);

	//Shape
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	//Fixture
	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 0.1f;
	fixture.restitution = restitution;
	SetFixture(fixture, type);
	b->CreateFixture(&fixture);

	//PhysBody
	PhysBody* pbody = new PhysBody();
	pbody->body_type = b_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* j1Physics::CreateRectangleSensor(int x, int y, int width, int height, COLLISION_TYPE type, BODY_TYPE b_type, uint restitution)
{
	//Body
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	b2Body* b = world->CreateBody(&body);

	//Shape
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	//Fixture
	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.restitution = restitution;
	fixture.isSensor = true;
	SetFixture(fixture, type);
	b->CreateFixture(&fixture);

	//PhhysBody
	PhysBody* pbody = new PhysBody();
	pbody->body_type = b_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;
	pbody->body_type = b_type;

	return pbody;
}

PhysBody* j1Physics::CreateChain(int x, int y, int* points, int size, COLLISION_TYPE type, BODY_TYPE b_type, uint restitution)
{
	//Body
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	b2Body* b = world->CreateBody(&body);

	//Shape
	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];
	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}
	shape.CreateLoop(p, size / 2);
	delete p;

	//Fixture
	b2FixtureDef fixture;
	fixture.shape = &shape;
	SetFixture(fixture, type);
	fixture.restitution = restitution;
	b->CreateFixture(&fixture);

	//PhysBody
	PhysBody* pbody = new PhysBody();
	pbody->body_type = b_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->body_type = b_type;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* j1Physics::CreateSensorChain(int x, int y, int* points, int size, COLLISION_TYPE type, BODY_TYPE b_type, uint restitution)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);

	}

	shape.Set(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.isSensor = true;

	SetFixture(fixture, type);

	fixture.restitution = restitution;
	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->body_type = b_type;

	return pbody;
}

PhysBody * j1Physics::TransformDefToBuilt(PhysBody* target)
{
	//Body
	b2BodyDef body;
	body.type = target->body_def->body_interaction_type;
	body.fixedRotation = target->body_def->fixed_rotation;
	body.position.Set(0, 0);
	b2Body* b = world->CreateBody(&body);

	b2Shape* shape = nullptr;

	switch (target->body_def->shape_type)
	{
	case b2Shape::Type::e_polygon:
	{
		//Square shape
		b2PolygonShape* box = new b2PolygonShape();

		box->SetAsBox(PIXEL_TO_METERS(target->body_def->width) * 0.5f, PIXEL_TO_METERS(target->body_def->height) * 0.5f);
		shape = box;
	}
	break;

	case b2Shape::Type::e_circle:
	{
		//Circle shape
		b2CircleShape* circle = new b2CircleShape();
		circle->m_radius = PIXEL_TO_METERS(target->body_def->width);
		shape = circle;
	}
	break;
	}

	//Fixture
	b2FixtureDef fixture;
	fixture.shape = shape;
	fixture.restitution = target->body_def->restitution;
	fixture.friction = target->body_def->friction;
	fixture.density = target->body_def->density;
	fixture.isSensor = target->body_def->is_sensor;
	SetFixture(fixture, target->body_def->collision_type);
	b->CreateFixture(&fixture);

	//PhysBody
	PhysBody* pbody = new PhysBody();
	pbody->body_type = target->body_def->body_type;
	pbody->collision_type = target->body_def->collision_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = target->body_def->width * 0.5f;
	pbody->height = target->body_def->height * 0.5f;
	pbody->listener = target->body_def->listener;

	if (shape != nullptr)RELEASE(shape);

	return pbody;
}

void j1Physics::SetFixture(b2FixtureDef& fixture, COLLISION_TYPE type)
{
	fixture.filter.categoryBits = type;
	switch (type)
	{
	case PLAYER_COLLISION:
		fixture.filter.maskBits = MAP_COLLISION | ITEM_COLLISION | STATIC_ITEM_COLLISION | ENEMY_SENSOR_COLLISION | NEUTRAL_SENSOR_COLLISION;
		break;
	case MAP_COLLISION:
		fixture.filter.maskBits = PLAYER_COLLISION | ITEM_COLLISION | GHOST_COLLISION | ENEMY_COLLISION;
		break;
	case ITEM_COLLISION:
		fixture.filter.maskBits = PLAYER_COLLISION | MAP_COLLISION | ITEM_COLLISION;
		break;
	case STATIC_ITEM_COLLISION:
		fixture.filter.maskBits = PLAYER_COLLISION;
		break;
	case NPC_COLLISION:
		fixture.filter.maskBits = ALLY_SENSOR_COLLISION;
		break;
	case ENEMY_COLLISION:
		fixture.filter.maskBits = MAP_COLLISION | ALLY_SENSOR_COLLISION;
		break;
	case GHOST_COLLISION:
		fixture.filter.maskBits = MAP_COLLISION;
		break;
	case NEUTRAL_SENSOR_COLLISION:
		fixture.filter.maskBits = PLAYER_COLLISION | NPC_COLLISION;
		break;
	case ALLY_SENSOR_COLLISION:
		fixture.filter.maskBits = NPC_COLLISION | ENEMY_COLLISION;
		break;
	case ENEMY_SENSOR_COLLISION:
		fixture.filter.maskBits = PLAYER_COLLISION;
		break;
	}
	return;
}

bool j1Physics::CreateWeldJoint(PhysBody * A, PhysBody * B)
{
	b2Vec2 world_point = A->body->GetWorldPoint(b2Vec2(0, 0));
	b2WeldJointDef joint_def;
	joint_def.bodyA = A->body;
	joint_def.bodyB = B->body;

	joint_def.localAnchorA = joint_def.bodyA->GetLocalPoint(world_point);
	joint_def.localAnchorB = joint_def.bodyB->GetLocalPoint(world_point);
	joint_def.referenceAngle = joint_def.bodyB->GetAngle() - joint_def.bodyA->GetAngle();
	joint_def.collideConnected = true;
	joint_def.dampingRatio = 0.5f;
	joint_def.frequencyHz = 1;

	joint_def.Initialize(A->body, B->body, world_point);

	b2Joint* joint = world->CreateJoint(&joint_def);

	return true;
}

bool j1Physics::CreateRevoluteJoint(b2Body* A, b2Body* B)
{
	b2Vec2 world_point = A->GetWorldPoint(b2Vec2(0, 0));

	b2RevoluteJointDef def;
	def.bodyA = A;
	def.bodyB = B;

	def.collideConnected = false;
	def.localAnchorA.Set(0, 0);
	def.localAnchorB.Set(0, 0);
	def.enableMotor = true;
	def.maxMotorTorque = 100000.0f;
	def.motorSpeed = 2.0f;

	/*def.Initialize(A, B, A->GetWorldCenter());
	def.collideConnected = true;*/

	b2RevoluteJoint* joint = (b2RevoluteJoint*)world->CreateJoint(&def);

	return true;
}

b2Body * j1Physics::CopyBody(const b2Body* target)
{
	//Body
	b2BodyDef body;
	body.type = target->GetType();
	body.position.Set(0,0);
	b2Body* b = world->CreateBody(&body);

	//Shape
	b2BlockAllocator alloc;
	b2Shape* shape = target->GetFixtureList()->GetShape()->Clone(&alloc);
	
	//Fixture
	b2FixtureDef fixture;
	fixture.shape = shape;
	fixture.density = 0.1f;
	fixture.restitution = target->GetFixtureList()->GetRestitution();
	SetFixture(fixture, ((COLLISION_TYPE)target->GetFixtureList()->GetFilterData().categoryBits));
	b->CreateFixture(&fixture);

	return b;
}

bool j1Physics::DeleteBody(b2Body * target)
{
	bodys_to_delete.push_back(target);

	return true;
}

void j1Physics::If_Sensor_contact(PhysBody* bodyA, PhysBody* bodyB)
{

}

void j1Physics::SmoothStates()
{
	const float oneMinusRatio = 1.f - fixed_timestep_accumulator_ratio;

	for (b2Body * b = world->GetBodyList(); b != NULL; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody)
		{
			continue;
		}

		PhysBody* phys_bdy = (PhysBody*)b->GetUserData();
		
		if(phys_bdy != nullptr)phys_bdy->Interpolate();
	}
}

unsigned short j1Physics::GetFixedTimestepAcRatio() const
{
	return fixed_timestep_accumulator_ratio;
}

b2Shape::Type j1Physics::StrToBodyShape(const char * str) const
{
	if (strcmp(str, "circle") == 0)		return b2Shape::Type::e_circle;
	if (strcmp(str, "edge") == 0)		return b2Shape::Type::e_edge;
	if (strcmp(str, "square") == 0)		return b2Shape::Type::e_polygon;
	if (strcmp(str, "chain") == 0)		return b2Shape::Type::e_chain;
	if (strcmp(str, "type_count") == 0)	return b2Shape::Type::e_typeCount;
}

COLLISION_TYPE j1Physics::StrToCollisionType(const char * str) const
{
	if (strcmp(str, "player_collision") == 0)			return COLLISION_TYPE::PLAYER_COLLISION;
	if (strcmp(str, "npc_collision") == 0)				return COLLISION_TYPE::NPC_COLLISION;
	if (strcmp(str, "enemy_collision") == 0)			return COLLISION_TYPE::ENEMY_COLLISION;
	if (strcmp(str, "map_collision") == 0)				return COLLISION_TYPE::MAP_COLLISION;
	if (strcmp(str, "item_collision") == 0)				return COLLISION_TYPE::ITEM_COLLISION;
	if (strcmp(str, "static_item_collision") == 0)		return COLLISION_TYPE::STATIC_ITEM_COLLISION;
	if (strcmp(str, "neutral_sensor_collision") == 0)	return COLLISION_TYPE::NEUTRAL_SENSOR_COLLISION;
	if (strcmp(str, "ally_sensor_collision") == 0)		return COLLISION_TYPE::ALLY_SENSOR_COLLISION;
	if (strcmp(str, "enemy_sensor_collision") == 0)		return COLLISION_TYPE::ENEMY_SENSOR_COLLISION;
}

BODY_TYPE j1Physics::StrToBodyType(const char * str) const
{
	if (strcmp(str, "player_body") == 0)			return BODY_TYPE::PLAYER_BODY;
	if (strcmp(str, "npc_body") == 0)				return BODY_TYPE::NPC_BODY;
	if (strcmp(str, "enemy_body") == 0)				return BODY_TYPE::ENEMY_BODY;
	if (strcmp(str, "map_body") == 0)				return BODY_TYPE::MAP_BODY;
	if (strcmp(str, "item_body") == 0)				return BODY_TYPE::ITEM_BODY;
	if (strcmp(str, "ally_sensor_body") == 0)		return BODY_TYPE::ALLY_SENSOR_BODY;
	if (strcmp(str, "enemy_sensor_body") == 0)		return BODY_TYPE::ENEMY_SENSOR_BODY;
	if (strcmp(str, "neutral_sensor_body") == 0)	return BODY_TYPE::NEUTRAL_SENSOR_BODY;
	return NO_BODY;
}
b2BodyType j1Physics::StrToInteractionType(const char * str) const
{
	if (strcmp(str, "dynamic") == 0)	return b2BodyType::b2_dynamicBody;
	if (strcmp(str, "kinematic") == 0)	return b2BodyType::b2_kinematicBody;
	if (strcmp(str, "static") == 0)		return b2BodyType::b2_staticBody;
}
/// -------------------------------------------------------