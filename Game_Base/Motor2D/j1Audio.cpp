#include "j1Audio.h"

#include "p2Log.h"
#include "j1App.h"
#include "j1FileSystem.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Audio::j1Audio() : j1Module()
{
	current_music = NULL;
	name = "audio";
}

// Destructor
j1Audio::~j1Audio()
{}

// Called before render is available
bool j1Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	// Load all the musics folders
	std::string musics_folder = config.child("audio_data").attribute("file").as_string();

	pugi::xml_document audio_data_doc;
	App->fs->LoadXML(musics_folder.c_str(), &audio_data_doc);

	pugi::xml_node music_node = audio_data_doc.child("data").child("musics").first_child();

	while (music_node != NULL)
	{
		Music_Block new_music_block;
		std::string music_folder = "audio/";
		new_music_block.music_folder = music_folder += music_node.attribute("folder").as_string();
		new_music_block.music_id = StrToMusicID(music_node.attribute("id").as_string());
		musics.push_back(new_music_block);

		music_node = music_node.next_sibling();
	}

	audio_data_doc.reset();

	return ret;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	if(!active)
		return true;

	//LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(current_music != NULL)
	{
		Mix_FreeMusic(current_music);
	}

	for(std::list<Mix_Chunk*>::iterator item = fx.begin(); item != fx.end();  item++)
		Mix_FreeChunk(item._Ptr->_Myval);

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path)
{
	bool ret = true;

	if(!active)
		return false;

	if(current_music != NULL)
	{
		Mix_FreeMusic(current_music);
	}

	current_music = Mix_LoadMUS_RW(App->fs->Load(path), 1);

	if(current_music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(Mix_PlayMusic(current_music, -1) < 0)
		{
			LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
			ret = false;
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV_RW(App->fs->Load(path), 1);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if(!active)
		return false;

	if(id > 0 && id < fx.size())
	{
		//Iterate audio fx list to find the correct fx
		std::list<Mix_Chunk*>::iterator item = fx.begin();
		for (uint k = 0; k < id; k++)
			item++;

		Mix_PlayChannel(-1, item._Ptr->_Myval, repeat);
	}

	return ret;
}

void j1Audio::VolumeUp()
{
	current_music_volume = current_fx_volume = MIN(current_music_volume + 10, 128);
	Mix_Volume(-1, current_fx_volume);
	Mix_VolumeMusic(current_music_volume);
}

void j1Audio::VolumeDown()
{
	current_music_volume = current_fx_volume = MAX(current_music_volume - MIN(current_music_volume,10), 1);
	Mix_Volume(-1, current_fx_volume);
	Mix_VolumeMusic(current_music_volume);
}

void j1Audio::SetMasterVolume(float volume)
{
	master_volume = volume;
}

void j1Audio::SetMusicVolume(float volume)
{
	current_music_volume = volume;
	Mix_VolumeMusic(volume);
}

void j1Audio::SetFXVolume(float volume)
{
	current_fx_volume = volume;
	Mix_Volume(-1, volume);
}

void j1Audio::ResetAudioSystem()
{
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
	}

	if ((Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG) != MIX_INIT_OGG)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
	}
}

void j1Audio::PlayMusic(MUSIC_ID music_to_play_id)
{
	uint size = musics.size();
	for (uint i = 0; i < size; i++)
	{
		if (musics[i].music_id  == music_to_play_id)
		{
			PlayMusic(musics[i].music_folder.c_str());
		}
	}
}

void j1Audio::StartMusicFade()
{
	current_fx_fade_volume = current_fx_volume;
	current_music_fade_volume = current_music_volume;
}

void j1Audio::FadeMusicOut(float total_time)
{
	current_music_fade_volume = MAX(0.0f, current_music_fade_volume - ((App->GetDT() * 128) / total_time));
	Mix_VolumeMusic(current_music_fade_volume);
	current_fx_fade_volume = MAX(0.0f, current_fx_fade_volume - ((App->GetDT() * 128) / total_time));
	Mix_Volume(-1, current_fx_fade_volume);
}

void j1Audio::FadeMusicIn(float total_time)
{
	current_music_fade_volume = MIN(current_music_volume, current_music_fade_volume + ((App->GetDT() * 128) / total_time));
	Mix_VolumeMusic(current_music_fade_volume);
	current_fx_fade_volume = MIN(current_fx_volume, current_fx_fade_volume + ((App->GetDT() * 128) / total_time));
	Mix_Volume(-1, current_fx_fade_volume);
}

MUSIC_ID j1Audio::StrToMusicID(const char * str) const
{
	if (strcmp(str, "music_menu") == 0)		return MUSIC_MENU;
	if (strcmp(str, "music_in_game") == 0)	return MUSIC_IN_GAME;
	return MUSIC_NONE;
}

FX_ID j1Audio::StrToFXID(const char * str) const
{
	return FX_NONE;
}
