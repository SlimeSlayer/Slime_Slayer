#include "j1App.h"
#include "j1InputManager.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Physics.h"
#include "p2Point.h"
#include "math.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "p2Log.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif


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

bool j1Physics::Awake(pugi::xml_node& config) {

	LOG("ModulePhysics Configuration Loaded!");
	return true;
}

bool j1Physics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

// 
bool j1Physics::PreUpdate()
{
	//This update the physics world
	/*if (physics_update_timer.ReadTicks() > SIMULATE_RATE)
	{*/
		world->Step(1.0f / 60.0f, 6, 2);
		physics_update_timer.Start();
	//}

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}



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
			world->DestroyBody(item._Ptr->_Myval);
			bodys_to_delete.pop_front();
			item = bodys_to_delete.begin();
		}
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

	//Active/Desactive physic debug mode ------------------
	if (App->input_manager->GetEvent(COLLIDERS_DEBUG_MODE) == INPUT_DOWN)
	{
		App->collisions_debug = !App->collisions_debug;
	}

	if (!App->collisions_debug) return true;


	//Iterate all world bodies for draw & active player mouse input
	bool body_clicked = false;
	b2Body* click_body = nullptr;

	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->render->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
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
						App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
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
						App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->render->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->render->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
			}
			break;
			}


			//Handle mouse input to check if a body is selected
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{

				if (f->TestPoint({ PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()) }))
				{
					body_clicked = true;
					click_body = f->GetBody();
				}
			}

		}

	}

	b2MouseJointDef def;

	if (body_clicked)
	{
		def.bodyA = ground;
		def.bodyB = click_body;
		def.target = { PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()) };
		def.dampingRatio = 0.5f;
		def.frequencyHz = 2.0f;
		def.maxForce = 100.0f * click_body->GetMass();
		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);

	}

	if (mouse_joint && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		mouse_joint->SetTarget({ PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()) });
		App->render->DrawLine((App->input->GetMouseX()), (App->input->GetMouseY()), METERS_TO_PIXELS(mouse_joint->GetAnchorB().x), METERS_TO_PIXELS(mouse_joint->GetAnchorB().y), 255, 0, 0);
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

	delete world;

	return true;
}


PhysBody* j1Physics::CreateCircle(int x, int y, int radius, collision_type type, BODY_TYPE b_type, uint restitution)
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
	pbody->collide_type = b_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* j1Physics::CreateStaticCircle(int x, int y, int radius, collision_type type, BODY_TYPE b_type, uint restitution)
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
	pbody->collide_type = b_type;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* j1Physics::CreateRectangle(int x, int y, int width, int height, collision_type type, BODY_TYPE b_type, uint restitution)
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
	pbody->collide_type = b_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* j1Physics::CreateRectangleSensor(int x, int y, int width, int height, collision_type type, BODY_TYPE b_type, uint restitution)
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
	pbody->collide_type = b_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;
	pbody->collide_type = b_type;

	return pbody;
}

PhysBody* j1Physics::CreateChain(int x, int y, int* points, int size, collision_type type, BODY_TYPE b_type, uint restitution)
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
	pbody->collide_type = b_type;
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->collide_type = b_type;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* j1Physics::CreateSensorChain(int x, int y, int* points, int size, collision_type type, BODY_TYPE b_type, uint restitution)
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
	pbody->collide_type = b_type;

	return pbody;
}

void j1Physics::SetFixture(b2FixtureDef& fixture, collision_type type)
{
	fixture.filter.categoryBits = type;
	switch (type)
	{
	case PLAYER_COLLISION:
		fixture.filter.maskBits = MAP_COLLISION;
		break;
	case MAP_COLLISION:
		fixture.filter.maskBits = PLAYER_COLLISION /*| BULLET | MAP_ITEM | MINI_BLOB*/;
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

bool j1Physics::DeleteBody(PhysBody * target)
{
	if (target->body == nullptr)
	{
		LOG("Invalid PhysBody to delete");
		return false;
	}

	bodys_to_delete.push_back(target->body);

	return true;
}

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

void PhysBody::SetPosition(int x, int y)
{
	float x_meters = PIXEL_TO_METERS(x);
	float y_meters = PIXEL_TO_METERS(y);
	b2Vec2 position((float32)x_meters, (float32)y_meters);
	body->SetTransform(position, body->GetAngle());
}

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

	if (physA != nullptr && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB != nullptr && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void j1Physics::If_Sensor_contact(PhysBody* bodyA, PhysBody* bodyB)
{

}