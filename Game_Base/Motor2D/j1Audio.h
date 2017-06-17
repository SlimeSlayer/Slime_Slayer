#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "j1Timer.h"

struct _Mix_Music;
struct Mix_Chunk;

#define DEFAULT_MUSIC_FADE_TIME 1.0f

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
	// Called before quitting
	bool CleanUp();

	//Turn UP/DOWN music volume
	void VolumeUp();
	void VolumeDown();
	void SetMasterVolume(float volume);
	void SetMusicVolume(float volume);
	void SetFXVolume(float volume);

	//Used with theoraplay audio system
	void ResetAudioSystem();

	//Play the music with the selected id
	void PlayMusic(MUSIC_ID music_to_play_id);
	
	//Used with the scenes transition (App fade)
	void StartMusicFade();
	void FadeMusicOut(float total_time);
	void FadeMusicIn(float total_time);

private:

	//Used when load audio blocks
	MUSIC_ID		StrToMusicID(const char* str)const;
	FX_ID			StrToFXID(const char* str)const;
	// Play a music file
	bool			PlayMusic(const char* path);
	// Load a WAV in memory
	unsigned int	LoadFx(const char* path);
	// Play a previously loaded WAV
	bool			PlayFx(unsigned int fx, int repeat = 0);

private:

	std::vector<Music_Block>	musics;
	std::vector<FX_Block>		effects;
	_Mix_Music*					current_music = NULL;

	std::list<Mix_Chunk*>	fx;

	float 					current_music_volume = 65.0f;
	float					current_fx_volume = 65.0f;
	float					current_music_fade_volume = 0.0f;
	float					current_fx_fade_volume = 0.0f;
};

#endif // __j1AUDIO_H__