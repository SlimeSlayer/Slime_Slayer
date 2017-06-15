#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "j1Timer.h"

struct _Mix_Music;
struct Mix_Chunk;

#define DEFAULT_MUSIC_FADE_TIME 0.5f

enum MUSIC_ID
{
	MUSIC_NONE = 0,
	MUSIC_MENU,
	MUSIC_IN_GAME
};

enum FX_ID
{
	FX_NONE = 0
};

/// Music_Block ---------------------------------
struct Music_Block
{
	std::string	music_folder;
	MUSIC_ID	music_id = MUSIC_NONE;
};
/// ---------------------------------------------

/// FX_Block ------------------------------------
struct FX_Block
{
	Mix_Chunk* music = nullptr;
	FX_ID music_id = FX_NONE;
};
/// ---------------------------------------------

class j1Audio : public j1Module
{
public:

	j1Audio();
	~j1Audio();

public:

	// Called before render is available
	bool Awake(pugi::xml_node&);
	// Called each loop iteration
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();

	//Turn UP/DOWN music volume
	void VolumeUp();
	void VolumeDown();

	//Used with theoraplay audio system
	void ResetAudioSystem();

	//Play the music with the selected id
	void PlayMusic(MUSIC_ID music_to_play_id);
	
private:

	//Used when load audio blocks
	MUSIC_ID		StrToMusicID(const char* str)const;
	FX_ID			StrToFXID(const char* str)const;
	// Play a music file
	bool			PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);
	// Load a WAV in memory
	unsigned int	LoadFx(const char* path);
	// Play a previously loaded WAV
	bool			PlayFx(unsigned int fx, int repeat = 0);

private:

	std::vector<Music_Block>	musics;
	std::vector<FX_Block>		effects;
	_Mix_Music*					current_music = NULL;

	std::list<Mix_Chunk*>	fx;
	j1Timer					go_down_time;
	bool					go_down = false;
	uint					volume = 65;

};

#endif // __j1AUDIO_H__