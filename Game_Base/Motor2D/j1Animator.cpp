#include "j1Animator.h"

#include <time.h>
#include <stdlib.h>

#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "SDL/include/SDL_rect.h"
#include "p2Log.h"
#include "j1EntitiesManager.h"

///Animation Sprite Class -----------------------
//Constructor ===============
Sprite::Sprite(const SDL_Rect & frame, const iPoint & pivot, int z_cord, uint opacity, float angle) : frame(frame), pivot(pivot), z_cord(z_cord), opacity(opacity), angle(angle)
{

}

//Destructor ================
Sprite::~Sprite()
{

}

//Functionality =============
const SDL_Rect * Sprite::GetFrame() const
{
	return &frame;
}

int Sprite::GetXpivot() const
{
	return pivot.x;
}

int Sprite::GetYpivot() const
{
	return pivot.y;
}

int Sprite::GetZ_cord() const
{
	return z_cord;
}

uint Sprite::GetOpacity() const
{
	return opacity;
}

float Sprite::GetAngle() const
{
	return angle;
}

void Sprite::SetAngle(float val)
{
	angle = val;
}
/// ---------------------------------------------



///Animation Class ------------------------------
//Constructor =========================
Animation::Animation()
{

}

Animation::Animation(const Animation & copy) :texture(copy.texture), tex_color(copy.tex_color), enum_id(copy.enum_id), current_frame(copy.current_frame), loop(copy.loop), speed(copy.speed), horizontal_flip_sprites(copy.horizontal_flip_sprites), vertical_flip_sprites(copy.vertical_flip_sprites), sprites_scale(copy.sprites_scale)
{
	const std::vector<Sprite>* sprites = copy.GetAllSprites();
	uint size = sprites->size();
	for (uint k = 0; k < size; k++)
	{
		this->sprites.push_back(Sprite(sprites->at(k)));
	}
}

//Destructor ==========================
Animation::~Animation()
{
	sprites.clear();
	texture = nullptr;
	//This texture can't be unloaded
}


//Functionality =======================
void Animation::ReleaseTexture()
{
	App->tex->UnLoad(texture);
}

void Animation::SetTexture(const SDL_Texture * tex)
{
	texture = (SDL_Texture*)tex;
}

void Animation::SetTexColor(SDL_Color val)
{
	tex_color = val;
}

void Animation::SetLoop(bool loop_state)
{
	loop = loop_state;
}

void Animation::SetCurrentFrame(uint curr)
{
	if (curr < sprites.size())	current_frame = curr;
	else current_frame = sprites.size() - 1;
}

void Animation::SetSpeed(uint new_speed)
{
	speed = new_speed;
}

void Animation::SetId(uint id)
{
	enum_id = id;
}

void Animation::SetSpritesHorizontalFlip(bool sprites_flip)
{
	horizontal_flip_sprites = sprites_flip;
}

void Animation::SetSpritesVerticalFlip(bool sprites_flip)
{
	vertical_flip_sprites = sprites_flip;
}

void Animation::SetSpritesScale(float sprites_sc)
{
	sprites_scale = sprites_sc;
}

void Animation::SetSpritesAngle(float angle)
{
	uint size = sprites.size();
	for (uint k = 0; k < size; k++)
	{
		sprites[k].SetAngle(angle);
	}
}

SDL_Texture * Animation::GetTexture() const
{
	return texture;
}

SDL_Color Animation::GetTexColor() const
{
	return tex_color;
}

bool Animation::GetLoop() const
{
	return loop;
}

uint Animation::GetCurrentFrame() const
{
	return current_frame;
}

uint Animation::GetSpeed() const
{
	return speed;
}

const Sprite* Animation::GetCurrentSprite()
{
	if (current_frame == -1)return &sprites[sprites.size() - 1];

	if (App->pause)return &sprites[(int)current_frame];

	current_frame = (float)floor(timer.Read() / speed);
	if (current_frame >= sprites.size())
	{
		if (loop)
		{
			loops++;
			current_frame = 0;
			timer.Start();
		}
		else
		{
			loops = 0;
			current_frame = -1;
			return &sprites[sprites.size() - 1];
		}
	}

	return &sprites[(int)current_frame];
}

const std::vector<Sprite>* Animation::GetAllSprites()const
{
	return &sprites;
}

uint Animation::GetId() const
{
	return enum_id;
}

bool Animation::GetSpritesHorizontalFlip() const
{
	return horizontal_flip_sprites;
}

bool Animation::GetSpritesVerticalFlip() const
{
	return vertical_flip_sprites;
}

float Animation::GetSpritesScale() const
{
	return sprites_scale;
}

bool Animation::IsEnd()
{
	return (current_frame == -1 && !loop);
}

void Animation::Reset()
{
	timer.Start();
	current_frame = 0;
}

void Animation::AddSprite(const SDL_Rect & rect, const iPoint & point, const int z, const uint opacity)
{
	sprites.push_back(Sprite(rect, point, z, opacity));
}
/// ---------------------------------------------


/// Animation Block Class -----------------------
//Constructor =========================
Animation_Block::Animation_Block(uint enum_id) :enum_id(enum_id)
{

}

//Destructor ==========================
Animation_Block::~Animation_Block()
{
	ClearAnimationBlocks();

	if (animation != nullptr)RELEASE(animation);
}

//Functionality =======================
void Animation_Block::ClearAnimationBlocks()
{
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(childs[k]);
	}
	childs.clear();

	if (animation != nullptr)
	{
		animation->ReleaseTexture();
		RELEASE(animation);
	}
}

void Animation_Block::SetId(uint id)
{
	enum_id = id;
}

uint Animation_Block::GetId() const
{
	return enum_id;
}

Animation* Animation_Block::GetAnimation() const
{
	return animation;
}

Animation_Block * Animation_Block::GetBlock(int index) const
{
	return childs.at(index);
}

uint Animation_Block::GetChildsNum() const
{
	return childs.size();
}

Animation_Block * Animation_Block::SearchId(uint id) const
{
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k]->GetId() == id)return childs[k];
	}

	return nullptr;
}

void Animation_Block::SetAnimation(const Animation * new_animation)
{
	if (new_animation != nullptr)animation = (Animation*)new_animation;
}

void Animation_Block::AddAnimationBlock(Animation_Block* new_animation_block)
{
	if (new_animation_block != nullptr)childs.push_back(new_animation_block);
}
/// ---------------------------------------------

//Animator Module -------------------------------
//Constructor =========================
j1Animator::j1Animator()
{
	name = "animator";
}

//Destructor ==========================
j1Animator::~j1Animator()
{

}


//Game Loop ===========================
void j1Animator::Init()
{
	active = false;
	enabled = false;
}

bool j1Animator::Enable()
{
	bool ret = true;

	//Timer to count the operations time and respect the TIME_TO_ENABLE
	j1Timer process_timer;

	//If the enable node is null we focus it to the first one of the data document
	if (current_enable_node.root() == NULL)
	{
		current_enable_node = animations_data.first_child().first_child().first_child();
	}

	//Iterate all the nodes
	while (current_enable_node != NULL)
	{
		//Load the animation data of the current node
		std::string folder = current_enable_node.attribute("file").as_string();
		ENTITY_TYPE entity_type = App->entities_manager->StrToEntityType(current_enable_node.attribute("entity_type").as_string());
		
		//Check if the block to load is for multiple entities
		if (current_enable_node.attribute("multiple").as_bool())
		{
			ret = LoadMultipleAnimationBlock(folder.c_str(), entity_type);
		}
		else
		{
			ret = LoadSimpleAnimationBlock(folder.c_str(), entity_type);
		}

		//Log the result to help tracking
		if (ret)
		{
			LOG("%s correctly loaded!", folder.c_str());
		}
		else
		{
			LOG("Error loading: %s", folder.c_str());
		}
		
		//Focus the next node
		current_enable_node = current_enable_node.next_sibling();

		//Break the enable process when the time is exceeded
		if (current_enable_node.root() == NULL)
		{
			break;
		}
		
		if(process_timer.Read() > TIME_TO_ENABLE)
		{
			return false;
		}
	}
	
	enabled = true;
	active = false;

	return true;
}

void j1Animator::Disable()
{
	//Clean the data loaded on the animations tree
	uint size = blocks.size();
	for (uint k = 0; k < size; k++)
	{
		blocks[k]->ClearAnimationBlocks();
	}
	
	active = false;
	enabled = false;
	
}

bool j1Animator::Awake(pugi::xml_node& config)
{
	bool ret = true;

	//Load the animations data document from config
	if (App->fs->LoadXML(config.child("animations_data").attribute("file").as_string(), &animations_data))
	{
		LOG("Animations data document loaded!");
		ret = true;
	}
	else
	{
		LOG("Error loading animations data document!");
		ret = false;
	}

	return ret;
}

bool j1Animator::Start()
{
	// arrow = App->tex->Load("buff_manager/arrow.PNG");

	//Define the animations blocks base to build the tree when animations are loaded
	if (blocks.empty())
	{
		//Creatures base animation block
		Animation_Block* creatures_block = new Animation_Block();
		creatures_block->SetId(ENTITY_TYPE::CREATURE);
		blocks.push_back(creatures_block);
		//Items base animation block
		Animation_Block* items_block = new Animation_Block();
		items_block->SetId(ENTITY_TYPE::ITEM);
		blocks.push_back(items_block);

	}
	else
	{
		LOG("Error animations blocks already allocated!");
		return false;
	}

	return true;
}

bool j1Animator::CleanUp()
{
	//Clean the animation blocks
	uint size = blocks.size();

	for (uint k = 0; k < size; k++)
	{
		RELEASE(blocks[k]);
	}
	blocks.clear();

	return true;
}

bool j1Animator::LoadSimpleAnimationBlock(const char * xml_folder, ENTITY_TYPE entity_type)
{
	//Load animations data from XML folder
	
	//XML load
	LOG("Loading: %s ...", xml_folder);
	std::string load_folder = name + "/" + xml_folder;
	pugi::xml_document entity_anim_data;
	if (!App->fs->LoadXML(load_folder.c_str(), &entity_anim_data))return false;
	
	//Texture load
	load_folder = name + "/" + entity_anim_data.child("atlas").attribute("texture").as_string();
	SDL_Texture* texture = App->tex->Load(load_folder.c_str());
	if (texture == nullptr)
	{
		LOG("Error loading %s texture", load_folder.c_str());
		return false;
	}

	//Color Load
	SDL_Color tex_color = App->entities_manager->TokenStrToColor(entity_anim_data.child("atlas").attribute("color").as_string());

	//Allocate the animation block
	Animation_Block* entity_animation_block = new Animation_Block();
	
	//Focus document entity family type
	pugi::xml_node data_node = entity_anim_data.first_child().first_child();

	//Get the entity family specific type
	uint specific_type = 0;
	switch (entity_type)
	{
	case CREATURE:	specific_type = App->entities_manager->StrToCreatureType(data_node.attribute("id").as_string());		break;
	case ITEM:		specific_type = App->entities_manager->StrToItemType(data_node.attribute("id").as_string());			break;
	}
	if (specific_type == 0)
	{
		LOG("Error loading entity specific type!");
	}
	entity_animation_block->SetId(specific_type);
	
	//Focus the first action node
	data_node = data_node.first_child();

	while (data_node != NULL)
	{
		//Allocate the action animation block
		Animation_Block* action_animation_block = new Animation_Block();

		//Focus the first direction node of the current action node
		pugi::xml_node direction_node = data_node.first_child();

		//Get the action id
		ACTION_TYPE action_type = App->entities_manager->StrToActionType(data_node.attribute("enum").as_string());
		action_animation_block->SetId(action_type);
		//Get animation speed
		uint speed = MAX(1, data_node.attribute("speed").as_uint(0));
		//Get animation loop
		bool loop = data_node.attribute("loop").as_bool(false);
		//Get animation scale
		float scale = data_node.attribute("scale").as_float(1.0f);

		//Iterate the direction nodes
		while (direction_node != NULL)
		{
			//Allocate the direction animation block
			Animation_Block* direction_animation_block = new Animation_Block();
			
			//Get direction id
			DIRECTION direction_id = App->entities_manager->StrToDirection(direction_node.attribute("enum").as_string());
			direction_animation_block->SetId(direction_id);

			//Allocate the animation 
			Animation* new_animation = new Animation();

			//Set all animation attributes
			//Animation texture
			new_animation->SetTexture(texture);
			//Animation color
			new_animation->SetTexColor(tex_color);
			//Animation speed
			new_animation->SetSpeed(speed);
			//Animation loop
			new_animation->SetLoop(loop);
			//Animation flip
			new_animation->SetSpritesHorizontalFlip(direction_node.attribute("h_flip_sprites").as_bool(false));
			new_animation->SetSpritesVerticalFlip(direction_node.attribute("v_flip_sprites").as_bool(false));
			//Animation scale
			new_animation->SetSpritesScale(scale);
			//Animation enum id
			new_animation->SetId(direction_id);

			//Focus the first sprite of the current animation
			pugi::xml_node sprite = direction_node.first_child();
			while (sprite != NULL)
			{
				//Load sprite rect
				SDL_Rect rect = { sprite.attribute("x").as_int(),sprite.attribute("y").as_int(),sprite.attribute("w").as_int(),sprite.attribute("h").as_int() };
				
				//Load sprite pivot
				float pX = sprite.attribute("pX").as_float() * rect.w;
				pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
				float pY = sprite.attribute("pY").as_float() * rect.h;
				pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);
				
				//Load sprite opacity
				uint opacity = sprite.attribute("opacity").as_uint();
				if (opacity == 0)opacity = 255;

				//Add sprite at animation
				new_animation->AddSprite(rect, iPoint(pX, pY), sprite.attribute("z").as_int(), opacity);

				//Focus next animation sprite
				sprite = sprite.next_sibling();
			}

			//Add the animation generated to the direction block
			direction_animation_block->SetAnimation(new_animation);

			//Focus the next direction node
			direction_node = direction_node.next_sibling();

			//Add the built direction animation block to the action block
			action_animation_block->AddAnimationBlock(direction_animation_block);
		}

		//Add the built action animation block to the entity block
		entity_animation_block->AddAnimationBlock(action_animation_block);

		//Focus the next action node
		data_node = data_node.next_sibling();
	}

	//Add built entity animation block on the correct blocks vector branch

	uint size = blocks.size();
	for (uint k = 0; k < size; k++)
	{
		if (blocks[k]->GetId() == entity_type)
		{
			blocks[k]->AddAnimationBlock(entity_animation_block);
		}
	}
	
	return true;
}

bool j1Animator::LoadMultipleAnimationBlock(const char * xml_folder, ENTITY_TYPE entity_type)
{
	//Load animations data from XML folder

	//XML load
	LOG("Loading: %s ...", xml_folder);
	std::string load_folder = name + "/" + xml_folder;
	pugi::xml_document entity_anim_data;
	if (!App->fs->LoadXML(load_folder.c_str(), &entity_anim_data))return false;

	//Texture load
	std::string tex_load_folder = name + "/" + entity_anim_data.child("atlas").attribute("texture").as_string();
	SDL_Texture* texture = App->tex->Load(tex_load_folder.c_str());
	if (texture == nullptr)
	{
		LOG("Error loading %s texture", tex_load_folder.c_str());
		return false;
	}

	//Focus the first entity animation block
	pugi::xml_node entity_node = entity_anim_data.first_child().child("entity");

	while (entity_node != NULL)
	{
		//Allocate the animation block
		Animation_Block* entity_animation_block = new Animation_Block();

		//Get the entity family specific type
		uint specific_type = 0;
		switch (entity_type)
		{
		case CREATURE:	specific_type = App->entities_manager->StrToCreatureType(entity_node.attribute("id").as_string());		break;
		case ITEM:		specific_type = App->entities_manager->StrToItemType(entity_node.attribute("id").as_string());			break;
		}
		if (specific_type == 0)
		{
			LOG("Error loading entity specific type!");
		}
		entity_animation_block->SetId(specific_type);

		//Load texture color
		SDL_Color		tex_color = App->entities_manager->TokenStrToColor(entity_node.attribute("color").as_string());
		SDL_Texture*	specific_texture = nullptr;
		
		if (entity_node.attribute("spec_tex").as_string("error") != "error")
		{
			std::string spec_folder = name + "/" + entity_node.attribute("spec_tex").as_string();
			specific_texture = App->tex->Load(spec_folder.c_str());
		}

		//Focus the first action node
		pugi::xml_node action_node = entity_node.first_child();

		while (action_node != NULL)
		{
			//Allocate the action animation block
			Animation_Block* action_animation_block = new Animation_Block();

			//Focus the first direction node of the current action node
			pugi::xml_node direction_node = action_node.first_child();

			//Get the action id
			ACTION_TYPE action_type = App->entities_manager->StrToActionType(action_node.attribute("enum").as_string());
			action_animation_block->SetId(action_type);
			//Get animation speed
			uint speed = MAX(1, action_node.attribute("speed").as_uint(0));
			//Get animation loop
			bool loop = action_node.attribute("loop").as_bool(false);
			//Get animation scale
			float scale = action_node.attribute("scale").as_float(1.0f);

			//Iterate the direction nodes
			while (direction_node != NULL)
			{
				//Allocate the direction animation block
				Animation_Block* direction_animation_block = new Animation_Block();

				//Get direction id
				DIRECTION direction_id = App->entities_manager->StrToDirection(direction_node.attribute("enum").as_string());
				direction_animation_block->SetId(direction_id);

				//Allocate the animation 
				Animation* new_animation = new Animation();

				//Set all animation attributes
				//Animation texture
				if(specific_texture== nullptr)new_animation->SetTexture(texture);
				else new_animation->SetTexture(specific_texture);
				//Animation color
				new_animation->SetTexColor(tex_color);
				//Animation speed
				new_animation->SetSpeed(speed);
				//Animation loop
				new_animation->SetLoop(loop);
				//Animation flip
				new_animation->SetSpritesHorizontalFlip(direction_node.attribute("h_flip_sprites").as_bool(false));
				new_animation->SetSpritesVerticalFlip(direction_node.attribute("v_flip_sprites").as_bool(false));
				//Animation scale
				new_animation->SetSpritesScale(scale);
				//Animation enum id
				new_animation->SetId(direction_id);

				//Focus the first sprite of the current animation
				pugi::xml_node sprite = direction_node.first_child();
				while (sprite != NULL)
				{
					//Load sprite rect
					SDL_Rect rect = { sprite.attribute("x").as_int(),sprite.attribute("y").as_int(),sprite.attribute("w").as_int(),sprite.attribute("h").as_int() };

					//Load sprite pivot
					float pX = sprite.attribute("pX").as_float() * rect.w;
					pX = (pX > (floor(pX) + 0.5f)) ? ceil(pX) : floor(pX);
					float pY = sprite.attribute("pY").as_float() * rect.h;
					pY = (pY > (floor(pY) + 0.5f)) ? ceil(pY) : floor(pY);

					//Load sprite opacity
					uint opacity = sprite.attribute("opacity").as_uint();
					if (opacity == 0)opacity = 255;

					//Add sprite at animation
					new_animation->AddSprite(rect, iPoint(pX, pY), sprite.attribute("z").as_int(), opacity);

					//Focus next animation sprite
					sprite = sprite.next_sibling();
				}

				//Add the animation generated to the direction block
				direction_animation_block->SetAnimation(new_animation);

				//Focus the next direction node
				direction_node = direction_node.next_sibling();

				//Add the built direction animation block to the action block
				action_animation_block->AddAnimationBlock(direction_animation_block);
			}

			//Add the built action animation block to the entity block
			entity_animation_block->AddAnimationBlock(action_animation_block);

			//Focus the next action node
			action_node = action_node.next_sibling();
		}

		//Add built entity animation block on the correct blocks vector branch

		uint size = blocks.size();
		for (uint k = 0; k < size; k++)
		{
			if (blocks[k]->GetId() == entity_type)
			{
				blocks[k]->AddAnimationBlock(entity_animation_block);
			}
		}

		entity_node = entity_node.next_sibling();
	}
}

bool j1Animator::EntityPlay(Entity* target)
{
	//Branch to find the correct tree node
	Animation_Block* animation_branch = nullptr;

	//Iterate blocks vector
	uint size = blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//Search the correct entity type branch
		if (blocks[k]->GetId() == target->GetEntityType())
		{
			animation_branch = blocks[k];
		}

	}
	if (animation_branch == nullptr)
	{
		LOG("Error finding entity branch");
		return false;
	}
	
	//Find the specific entity type
	if (target->GetEntityType() == CREATURE)animation_branch = animation_branch->SearchId(((Creature*)target)->GetCreatureType());
	else animation_branch = animation_branch->SearchId(((Item*)target)->GetItemType());
	if (animation_branch == nullptr)
	{
		LOG("Error finding specific entity branch");
		return false;
	}

	//Find the action branch
	animation_branch = animation_branch->SearchId(target->GetActionType());
	if (animation_branch == nullptr)
	{
		LOG("Error finding action branch");
		return false;
	}

	//Find the direction branch
	animation_branch = animation_branch->SearchId(target->GetDirection());
	if (animation_branch == nullptr)
	{
		LOG("Error finding direction branch");
		return false;
	}

	//Set the animation of the branch found
	Animation* target_anim = target->GetAnimation();
	if (target_anim != nullptr)RELEASE(target_anim);
	target->SetAnimation(new Animation(*animation_branch->GetAnimation()));
	target->GetAnimation()->Reset();
		
	return true;
}