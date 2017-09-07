#include "j1Player.h"

#include "j1App.h"
#include "j1EntitiesManager.h"
#include "j1InputManager.h"
#include "j1Physics.h"
#include "j1Render.h"
#include "Scene.h"
#include "Worker.h"
#include "j1Animator.h"
#include "j1FileSystem.h"
#include "p2Log.h"
#include "UI_Image.h"
#include "UI_Progressive_Bar.h"
#include "UI_String.h"
#include "j1Textures.h"

// Constructors =================================
j1Player::j1Player()
{
	name = "player";
}

// Destructors ==================================
j1Player::~j1Player()
{
	
}

// Game Loop ====================================
void j1Player::Init()
{
	enabled = false;
	active = false;
}

bool j1Player::Enable()
{
	//Generate player avatar
	avatar = (Player*)App->entities_manager->GenerateCreature(PLAYER_CREATURE, false);
	PhysBody* bdy_def = avatar->GetBody();
	avatar->SetBody(App->physics->TransformDefToBuilt(avatar->GetBody()));
	RELEASE(bdy_def);
	avatar->GetBody()->body->GetFixtureList()->SetFriction(0.0f);
	avatar->GetBody()->entity_related = avatar;
	App->animator->EntityPlay(avatar);

	//Adjust Player UI 
	//Life bar
	life_bar->SetMaxValue(avatar->GetMaxLife());
	life_bar->SetCurrentValue(avatar->GetCurrentLife());
	life_bar->GenerateTexture();
	
	//EXP bar
	exp_bar->SetMaxValue(avatar->GetNextLevelExperience());
	exp_bar->SetCurrentValue(avatar->GetExperience());
	exp_bar->GenerateTexture();

	//Money Str
	money_string->SetString("0 $");
	money_string->SetBoxPosition(140, 125);

	//Activate Player UI 
	avatar_ui_branch->Activate();
	avatar_ui_branch->ActivateChilds();

	enabled = true;
	active = false;

	return true;
}

void j1Player::Disable()
{
	LOG("Disabling Player");

	RELEASE(avatar);

	//Deactivate player UI
	avatar_ui_branch->Desactivate();
	avatar_ui_branch->DesactivateChids();

	active = enabled = false;

	LOG("Player Disabled");
}

bool j1Player::Awake(pugi::xml_node & node)
{
	LOG("Loading Player data document...");
	//Load player data document
	if (App->fs->LoadXML(node.child("data_doc").attribute("folder").as_string(), &data_doc))
	{
		LOG("Player data document correctly loaded!");
	}
	else
	{
		LOG("Error loading Player data document");
	}

	return true;
}

bool j1Player::Start()
{
	//Build avatar UI -------
	avatar_ui_branch = App->gui->GenerateUI_Element(UI_TYPE::UNDEFINED);
	avatar_ui_branch->SetInputTarget(this);

	//Avatar Icon -------------------------------
	avatar_icon = (UI_Image*)App->gui->GenerateUI_Element(UI_TYPE::IMG);
	avatar_icon->ChangeTextureId(TEXTURE_ID::AVATAR_ICON);
	avatar_icon->SetVisualLayer(10);
	avatar_icon->SetInputTarget(this);
	avatar_icon->SetBoxPosition(40, 40);
	avatar_icon->SetTextureScale(0.5f);
	avatar_icon->AdjustBox();
	avatar_ui_branch->AddChild(avatar_icon);
	// ------------------------------------------

	//Avatar Life Bar ---------------------------
	pugi::xml_node life_bar_node = data_doc.first_child().child("life_bar");
	life_bar = (UI_Progressive_Bar*)App->gui->GenerateUI_Element(UI_TYPE::PROGRESSIVE_BAR);
	life_bar->SetBox({ life_bar_node.attribute("x").as_int(),life_bar_node.attribute("y").as_int(),life_bar_node.attribute("w").as_int(),life_bar_node.attribute("h").as_int() });
	life_bar->SetFullColor(App->entities_manager->TokenStrToColor(life_bar_node.attribute("full").as_string()));
	life_bar->SetToEmptyColor(App->entities_manager->TokenStrToColor(life_bar_node.attribute("to_empty").as_string()));
	life_bar->SetEmptyColor(App->entities_manager->TokenStrToColor(life_bar_node.attribute("empty").as_string()));
	life_bar->SetToEmptyRestValue(life_bar_node.attribute("to_empty_rest_val").as_float());
	life_bar->SetVisualLayer(10);
	life_bar->SetInputTarget(this);
	avatar_ui_branch->AddChild(life_bar);
	// ------------------------------------------

	//Avatar EXP Bar ----------------------------
	pugi::xml_node exp_bar_node = data_doc.first_child().child("exp_bar");
	exp_bar = (UI_Progressive_Bar*)App->gui->GenerateUI_Element(UI_TYPE::PROGRESSIVE_BAR);
	exp_bar->SetBox({ exp_bar_node.attribute("x").as_int(),exp_bar_node.attribute("y").as_int(),exp_bar_node.attribute("w").as_int(),exp_bar_node.attribute("h").as_int() });
	exp_bar->SetFullColor(App->entities_manager->TokenStrToColor(exp_bar_node.attribute("full").as_string()));
	exp_bar->SetToEmptyColor(App->entities_manager->TokenStrToColor(exp_bar_node.attribute("to_empty").as_string()));
	exp_bar->SetEmptyColor(App->entities_manager->TokenStrToColor(exp_bar_node.attribute("empty").as_string()));
	exp_bar->SetToEmptyRestValue(exp_bar_node.attribute("to_empty_rest_val").as_float());
	exp_bar->SetVisualLayer(10);
	exp_bar->SetInputTarget(this);
	avatar_ui_branch->AddChild(exp_bar);
	// ------------------------------------------

	// Money String -----------------------------
	money_string = (UI_String*)App->gui->GenerateUI_Element(UI_TYPE::STRING);
	Font_Def* f_def = App->font->GetFontByID(FONT_ID::MONEY_FONT);
	money_string->SetFont(f_def->font);
	money_string->SetColor(f_def->font_color);
	money_string->SetVisualLayer(20);
	money_string->SetInputTarget(this);
	avatar_ui_branch->AddChild(money_string);
	// ------------------------------------------

	//Add the built branch at the GUI 
	App->gui->PushScreen(avatar_ui_branch);

	return true;
}

bool j1Player::Update(float dt)
{
	//When player is dead
	if (avatar == nullptr || avatar->GetBody() == nullptr)return true;


	//Update player avatar ----------------------
	avatar->Update();
	if (avatar == nullptr || avatar->GetBody() == nullptr)return true;
	avatar->Draw();

	//Update Player UI --------------------------
	//life_bar->Update();
	//exp_bar->Update();

	// Read all player inputs states ------------
	INPUT_STATE go_left_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_LEFT);
	INPUT_STATE go_right_input_state = App->input_manager->GetEvent(INPUT_EVENT::GO_RIGHT);
	INPUT_STATE jump_input_state = App->input_manager->GetEvent(INPUT_EVENT::JUMP);
	INPUT_STATE crouch_input_state = App->input_manager->GetEvent(INPUT_EVENT::CROUCH);
	INPUT_STATE attack_input_state = App->input_manager->GetEvent(INPUT_EVENT::ATTACK);
	
	
	// Get player necessary data ----------------
	int x_pos = 0, y_pos = 0;
	avatar->GetBody()->GetPosition(x_pos, y_pos);
	b2Vec2 linear_vel = avatar->GetBody()->body->GetLinearVelocity();


	// Center camera at player position ---------
	if (x_pos < 4050 && x_pos > 400)App->render->camera.x = -x_pos + PLAYER_CAMERA_X;
	App->render->camera.y = -y_pos + PLAYER_CAMERA_Y;

	// Check if player input is blocked ---------
	if (avatar->GetInputBlocked() || App->GetCurrentScene() == nullptr || App->pause)return true;

	// ATTACK INPUT -----------------------------
	if (attack_input_state == INPUT_DOWN || (attack_input_state == INPUT_REPEAT && avatar->ReadyToAttack()))
	{
		if (avatar->GetDirection() == RIGHT)
		{
			avatar->AttackRight();
		}
		else if (avatar->GetDirection() == LEFT)
		{
			avatar->AttackLeft();
		}
	}

	// AIR INPUT --------------------------------
	if (!avatar->GetBody()->IsInContact())
	{
		if (linear_vel.x < 0)
		{
			App->GetCurrentScene()->UpdateParallax(-avatar->GetMovSpeed());
		}
		else if (linear_vel.x > 0)
		{
			App->GetCurrentScene()->UpdateParallax(avatar->GetMovSpeed());
		}
		return true;
	}

	// ARROWS INPUT -----------------------------
	if (go_left_input_state == INPUT_REPEAT && !avatar->GetBody()->IsInSpecificContact(1, MAP_LEFT_LIMIT_COLLISION))
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(-avatar->GetMovSpeed(), linear_vel.y));
		App->GetCurrentScene()->UpdateParallax(-avatar->GetMovSpeed());
		avatar->SetDirection(LEFT);
	}
	else if (go_right_input_state == INPUT_REPEAT && !avatar->GetBody()->IsInSpecificContact(1, MAP_RIGHT_LIMIT_COLLISION))
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(avatar->GetMovSpeed(), linear_vel.y));
		App->GetCurrentScene()->UpdateParallax(avatar->GetMovSpeed());
		avatar->SetDirection(RIGHT);
	}
	else
	{
		avatar->GetBody()->body->SetLinearVelocity(b2Vec2(0.0f, linear_vel.y));
	}

	// JUMP INPUT -------------------------------
	if (jump_input_state == INPUT_DOWN && avatar->GetBody()->IsInSpecificContact(1,MAP_COLLISION))
	{
		avatar->GetBody()->body->ApplyForceToCenter(b2Vec2(0.0f, -avatar->GetJumpForce()), true);
	}

	return true;
}

bool j1Player::CleanUp()
{
	if (avatar != nullptr)RELEASE(avatar);

	return true;
}

// Save / Load functions ========================
bool j1Player::PartySave(pugi::xml_node & node) const
{
	node.append_attribute("money") = avatar->GetMoney();
	return true;
}

bool j1Player::PartyLoad(pugi::xml_node & node)
{
	avatar->AddMoney(node.attribute("money").as_uint());
	
	return true;
}

// Collisions functions =========================
void j1Player::OnSensorCollision(PhysBody * A, PhysBody * B)
{
	/*A is the sensor*/

	if (avatar == nullptr) return;

	// Attack sensor
	INPUT_STATE atk_input_state = App->input_manager->GetEvent(INPUT_EVENT::ATTACK);
	//Check if player is ready to attack
	if ((atk_input_state == INPUT_DOWN || (atk_input_state == INPUT_REPEAT && avatar->ReadyToAttack())) && A == avatar->GetCurrentAttackArea())
	{
		//Check if B is an enemy
		if (B->entity_related->GetDiplomacy() == ENEMY)
		{
			A->entity_related->worker.AddAction(A->entity_related->worker.GenerateAction(LG_ACTION_TYPE::LG_SIMPLE_ATTACK_ACTION, A->entity_related, (Creature*)B->entity_related));
		}
	}
}

//Functionality =================================
void j1Player::PlayerDeath()
{
	//Clean avatar
	avatar = nullptr;

	//Start scene death mode
	App->GetCurrentScene()->PlayerDeathMode();
}
