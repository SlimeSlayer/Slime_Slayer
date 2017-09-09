#include "BaseEntities.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1Animator.h"
#include "j1Physics.h"
#include "j1EntitiesManager.h"
#include "j1ParticleManager.h"
#include "j1Player.h"
#include "j1Render.h"

/// Entity --------------------------------------
//Base class where the entity pillars are defined

//Constructors ========================
Entity::Entity()
{

}

Entity::Entity(const Entity & copy, bool generate_body) : entity_type(copy.entity_type), diplomacy(copy.diplomacy), name(copy.name), description(copy.description), action_type(copy.action_type), direction(copy.direction)
{
	if (generate_body)
	{
		body = App->physics->TransformDefToBuilt(copy.body);
		body->entity_related = this;
	}
	else body = new PhysBody(*copy.body);

	//Set idle action to call animator
	action_type = IDLE_ACTION;
}

//Destructors =========================
Entity::~Entity()
{
	if (current_animation != nullptr)RELEASE(current_animation);
	if (body != nullptr)RELEASE(body);
}

void Entity::Clean()
{
	RELEASE(body);
}

//Game Loop ===========================
bool Entity::Update()
{
	//Update worker
	worker.Update();

	return true;
}

void Entity::Draw()
{
	if (body == nullptr)return;

	if (current_animation != nullptr)
	{
		int x = 0, y = 0;
		this->body->GetPosition(x, y);
		const Sprite* sprite = current_animation->GetCurrentSprite();
		SDL_Color color = current_animation->GetTexColor();
		if (worker.GetCurrentActionType() == LG_STUN_ACTION)
		{
			color = ((Stun_Action*)worker.GetCurrentAction())->blit_color;
		}
		App->render->CallBlit(current_animation->GetTexture(), x, y, sprite->GetFrame(), false, current_animation->GetSpritesHorizontalFlip(), current_animation->GetSpritesVerticalFlip(), current_animation->GetSpritesScale(), 0, 255, -sprite->GetXpivot(), -sprite->GetYpivot(), color,sprite->GetAngle());
	}
}

//Set Methods =========================
void Entity::SetEntityType(ENTITY_TYPE type)
{
	entity_type = type;
}

void Entity::SetDiplomacy(DIPLOMACY dip)
{
	diplomacy = dip;
}

void Entity::SetName(std::string new_name)
{
	name = new_name;
}

void Entity::SetDescription(std::string new_description)
{
	description = new_description;
}

void Entity::SetBody(PhysBody * new_body)
{
	body = new_body;
}

void Entity::SetPosition(float x, float y)
{
	body->SetPosition(x, y);
}

void Entity::SetAnimation(Animation * animation)
{
	current_animation = animation;
}

void Entity::SetActionType(ACTION_TYPE action_type)
{
	this->action_type = action_type;
}

void Entity::SetDirection(DIRECTION direction)
{
	this->direction = direction;
}

//Get Methods =========================
ENTITY_TYPE Entity::GetEntityType() const
{
	return entity_type;
}

DIPLOMACY Entity::GetDiplomacy() const
{
	return diplomacy;
}

const char* Entity::GetName() const
{
	return name.c_str();
}

const char* Entity::GetDescription() const
{
	return description.c_str();
}

PhysBody * Entity::GetBody() const
{
	return body;
}

Animation * Entity::GetAnimation() const
{
	return current_animation;
}

ACTION_TYPE Entity::GetActionType() const
{
	return action_type;
}

DIRECTION Entity::GetDirection() const
{
	return direction;
}

// Functionality ======================
void Entity::UpdatePosition()
{

}

const char* Entity::GenerateMouseInString()const
{
	return name.c_str();
}

/// Item ----------------------------------------
// Constructors =======================
Item::Item()
{

}

Item::Item(const Item & copy, bool generate_body) :Entity(copy, generate_body), item_type(copy.item_type), volatile_item(copy.volatile_item)
{

}

// Destructors ========================
Item::~Item()
{

}

//Set Methods =========================
void Item::SetItemType(ITEM_TYPE new_item_type)
{
	item_type = new_item_type;
}

void Item::SetAsVolatile()
{
	volatile_item = true;
}

//Get Methods =========================
ITEM_TYPE Item::GetItemType() const
{
	return item_type;
}

bool Item::GetIfVolatile() const
{
	return volatile_item;
}
/// ---------------------------------------------

/// Creature ------------------------------------
// Constructors =======================
Creature::Creature()
{

}

Creature::Creature(const Creature & copy, bool generate_body) : Entity(copy, generate_body), creature_type(copy.creature_type), current_life(copy.current_life),max_life(copy.max_life), life_bar(&copy.life_bar), attack_hitpoints(copy.attack_hitpoints), attack_rate(copy.attack_rate), mov_speed(copy.mov_speed), jump_force(copy.jump_force), money(copy.money), reward_experience(copy.reward_experience), level(copy.level)
{
	entity_type = CREATURE;
}

// Destructors ========================
Creature::~Creature()
{

}

bool Creature::Update()
{
	//Update worker
	worker.Update();

	life_bar.Update();

	return true;
}

// Game Loop ==========================
void Creature::Draw()
{
	if (body == nullptr)return;

	if (diplomacy == ENEMY)
	{
		int x = 0, y = 0;
		body->GetPosition(x, y);
		life_bar.DrawAt(x - life_bar.GetBox()->w * 0.5, y - body->height * 2 - App->entities_manager->bar_life_margin);
	}

	if (current_animation != nullptr)
	{
		int x = 0, y = 0;
		this->body->GetPosition(x, y);
		const Sprite* sprite = current_animation->GetCurrentSprite();
		SDL_Color color = current_animation->GetTexColor();
		if (worker.GetCurrentActionType() == LG_STUN_ACTION)
		{
			color = ((Stun_Action*)worker.GetCurrentAction())->blit_color;
		}
		App->render->CallBlit(current_animation->GetTexture(), x, y, sprite->GetFrame(), false, current_animation->GetSpritesHorizontalFlip(), current_animation->GetSpritesVerticalFlip(), current_animation->GetSpritesScale(), 0, 255, -sprite->GetXpivot(), -sprite->GetYpivot(), color,sprite->GetAngle());
	}
}

//Set Methods =========================
void Creature::SetCreatureType(CREATURE_TYPE new_creature_type)
{
	creature_type = new_creature_type;
}

void Creature::SetCurrentLife(uint new_life)
{
	float vl = new_life;
	life_bar.AddValue(vl - current_life);
	current_life = new_life;	
}

void Creature::SetMaxLife(uint new_max_life)
{
	max_life = new_max_life;
	life_bar.SetMaxValue(new_max_life);
}

void Creature::SetAttackHitPoints(uint new_attack)
{
	attack_hitpoints = new_attack;
}

void Creature::SetAttackRate(uint atk_rate)
{
	attack_rate = atk_rate;
}

void Creature::SetMovSpeed(float new_mov_speed)
{
	mov_speed = new_mov_speed;
}

void Creature::SetJumpForce(float new_jump_force)
{
	jump_force = new_jump_force;
}

void Creature::SetMoney(uint money)
{
	this->money = money;
}

void Creature::SetRewardExperience(uint experience)
{
	reward_experience = experience;
}

//Get Methods =========================
CREATURE_TYPE Creature::GetCreatureType() const
{
	return creature_type;
}

uint Creature::GetCurrentLife() const
{
	return current_life;
}

uint Creature::GetMaxLife() const
{
	return max_life;
}

UI_Progressive_Bar* Creature::GetLifeBar()
{
	return &life_bar;
}

uint Creature::GetAttackHitPoints() const
{
	return attack_hitpoints;
}

uint Creature::GetAttackRate() const
{
	return attack_rate;
}

float Creature::GetMovSpeed() const
{
	return mov_speed;
}

float Creature::GetJumpForce() const
{
	return jump_force;
}

uint Creature::GetMoney() const
{
	return money;
}

uint Creature::GetRewardExperience() const
{
	return reward_experience;
}

// Functionality ======================
void Creature::AddMoney(uint gained_money)
{
	money += gained_money;
}

void Creature::DropMoney()
{
	//Drop the creature money
	int x = 0, y = 0;
	body->GetPosition(x, y);
	for (uint k = 0; k < money; k++)
	{
		//Generate all the items
		Item* coin = App->entities_manager->GenerateItem(COIN_ITEM);
		
		//Set items correct states
		coin->SetPosition(x, y);
		
		float per_cent = (rand() % 100) * 0.01;
		int dir = rand() % 10 > 5 ? -1 : 1;
		coin->GetBody()->body->SetLinearVelocity(b2Vec2(CREATURES_DROPS_RAD * per_cent * dir, CREATURES_DROPS_IMPULSE + CREATURES_DROPS_RAD / MAX(per_cent, 0.5)));
		
		//Coins dropped have a ghost delay
		coin->worker.AddAction(coin->worker.GenerateAction(LG_ACTION_TYPE::LG_SPAWN_DELAY_ACTION, coin, 500));

		//Rest volatile timer for correct timing
		((Volatile_Item*)coin)->ResetVolatileTimer();

		if (k > 15)break;
	}

	//Reset money value
	money = 0;
}

void Creature::DropExperience()
{
	//Generate the experience particle
	App->particle_manager->GenerateTextParticle(App->player->avatar, PARTICLE_TYPE::EXPERIENCE_POINTS_PARTICLE, reward_experience * level);
	
	//Add the experience to the target
	App->player->avatar->AddExperience(reward_experience * level);
}
