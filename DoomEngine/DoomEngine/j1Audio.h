#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include <list>
#include <string>
#include <map>
#include "j1Timer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

struct Mix_Chunk_Avanced
{
	Mix_Chunk* chunk; 
	int actualChannel = -1; 
};

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();

	bool Update(float dt); 
	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node& node) {
		
		Mix_RewindMusic();
		double musicPosSec = node.child("actual_music_time_sec").attribute("value").as_double(); 
		Mix_SetMusicPosition(musicPosSec);

		return true; 
	}
	bool Save(pugi::xml_node& node) const
	{
		node.append_child("actual_music_time_sec").append_attribute("value") = musTimer.ReadSec();

		return true; 
	}

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Stop the current muisc
	void PauseMusic()
	{
		Mix_PauseMusic(); 
	}

	// Resume the current muisc
	void ResumeMusic()
	{
		Mix_ResumeMusic();
	}

	// Load a WAV in memory
	unsigned int LoadFx(const char* path, std::string wantedName);

	// Play a previously loaded WAV
	bool PlayFx(std::string name, int repeat = 0, bool prioritary = false, float volume = (float)INT_MAX, float musReduc = (float)INT_MAX, float fxsReduc = (float)INT_MAX );

	//Set specific FX volume
	bool SetSpecificFxVolume(std::string name, float volume);

	// Set music volume
	void SetVolume(float volume);

	// Set FX volume
	void SetFxVolume(float volume);

	// Set both
	void ResetMusicAndFxVolumes(); 

	// Stop all FXs
	void StopAllFxs();

	// Stop specific FX
	void StopSpecificFx(std::string name); 

	// Pause specific FX
	void PauseSpecificFx(std::string name);

	// Resume specific FX
	void ResumeSpecificFx(std::string name);

	// Gather the status of a FX
	bool isPlayingFx(std::string name); 
	bool isPausedFx(std::string name); 

	// check if and fx exitst in the map
	bool existsFx(std::string name) const
	{
		auto item = fxMap.find(name);
		if (item != fxMap.end())
			return true;
		return false; 
	}

private:

	_Mix_Music*			music = nullptr;
	std::string prioritaryChunk = "empty"; 
	bool toUpdate = false; 
//	std::list<Mix_Chunk*>	fx;
	float final_volume = MIX_MAX_VOLUME;
	float final_fx_volume = MIX_MAX_VOLUME;
	float last_volume = 0.0f;
	float last_fx_volume = 0.0f; 
	float previous_volume = 0.0f; 
	float previous_fx_volume = 0.0f; 

	j1Timer musTimer; 

public: 
	std::map<std::string, Mix_Chunk_Avanced> fxMap;

};

#endif // __j1AUDIO_H__   