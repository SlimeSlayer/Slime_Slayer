#ifndef _J1ANIMATOR_H_
#define _J1ANIMATOR_H_

#include "j1Module.h"
#include "j1Timer.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"
struct SDL_Texture;
class Entity;
class Item;
enum ENTITY_TYPE;

///Animation Sprite Class -----------------------
class Sprite
{
public:

	Sprite(const SDL_Rect& frame, const iPoint& pivot, int z_cord = 0, uint opacity = 255);
	~Sprite();

private:

	SDL_Rect		frame = { 0,0,0,0 };
	iPoint			pivot = { 0,0 };
	int				z_cord = 0;
	uint			opacity = 255;

public:

	//Functionality
	//Get Methods
	const SDL_Rect*		GetFrame()const;
	int					GetXpivot()const;
	int					GetYpivot()const;
	int					GetZ_cord()const;
	uint				GetOpacity()const;

};
/// ---------------------------------------------

///Animation Class ------------------------------
//Class that contains the animation basic necessary data
class Animation
{
public:

	Animation();
	virtual ~Animation();

protected:

	//Vector that storage the frames rect & pivot & z dimension
	std::vector<Sprite>		sprites;
	//Pointer to the animation texture
	SDL_Texture*			texture = nullptr;
	//Id of the animation enum type
	uint					enum_id = 0;
	//Current frame calculated by the timer
	float					current_frame = 0;
	bool					loop = true;
	int						loops = 0;
	//Animation update rate
	uint					speed = 2500;
	j1Timer					timer;
	//Animation flip 
	bool					flip_sprites = false;

public:

	//Functionality -------------------
	//Called before clean ---
	virtual void	ReleaseTexture();

	//Set Methods -----------
	void	SetTexture(const SDL_Texture* tex);
	void	SetLoop(bool loop_state);
	void	SetCurrentFrame(uint curr);
	void	SetSpeed(uint new_speed);
	void	SetId(uint id);

	//Get Methods -----------
	SDL_Texture*					GetTexture()const;
	bool							GetLoop()const;
	uint							GetCurrentFrame()const;
	uint							GetSpeed()const;
	const Sprite*					GetCurrentSprite();
	const std::vector<Sprite>*		GetAllSprites()const;
	uint							GetId()const;

	//Check if the animation is end
	bool IsEnd();
	//Reset the animation frame
	void Reset();

	//Add New sprite
	void AddSprite(const SDL_Rect& rect, const iPoint& point, const int z = 0, const uint opacity = 255);

};
/// ---------------------------------------------

/// Animation Block Class -----------------------
//Block that contains a vector of animations 
class Animation_Block
{
public:

	Animation_Block(uint enum_id = 0);

	~Animation_Block();

protected:

	//Vector of other animation blocks
	std::vector<Animation_Block*>	childs;
	//Enum id of this block
	uint							enum_id = 0;
	//Pointer to a vector of animations 
	Animation*						animation = nullptr;

public:

	//Functionality -------------------
	//Delete all contained blocks data
	void	ClearAnimationBlocks();

	//Set Methods -----------
	void	SetId(uint id);

	//Get Methods -----------
	uint				GetId()const;
	Animation*			GetAnimation()const;
	Animation_Block*	GetBlock(int index)const;
	uint				GetChildsNum()const;
	Animation_Block*	SearchId(uint id)const;

	//Add Methods -----------
	void	SetAnimation(const Animation* new_animation);
	void	AddAnimationBlock(Animation_Block* new_animation_block);

};
/// ---------------------------------------------

//Animator Module -------------------------------
class j1Animator : public j1Module
{
public:

	j1Animator();
	~j1Animator();

public:

	void Init();
	bool Enable();
	void Disable();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

private:

	//Data used to load animations during enable
	pugi::xml_document	animations_data;
	pugi::xml_node		current_enable_node;

	//Vector that allocates all the base animation blocks
	std::vector<Animation_Block*>	blocks;

public:

	//Load animation block
	bool	LoadAnimationBlock(const char* xml_folder, ENTITY_TYPE entity_type);

	//Animations Methods ----
	bool	EntityPlay(Entity* target);

	//Arrow sprite
	SDL_Texture* arrow = nullptr;
};
#endif // !_J1ANIMATOR_H_
