#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Audio.h"


#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Audio::j1Audio() : j1Module()
{
	music = nullptr;
	name.assign("audio");
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

	return ret;

}

bool j1Audio::Start()
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - GUI
	LoadFx("sound/fx/dsswtchn.wav", "buttonClick");
	LoadFx("sound/fx/At Vance - Four Seasons - Summer.wav", "musSlider"); 
	LoadFx("sound/fx/dsbossit.wav", "fxsSlider");

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - coins
	LoadFx("sound/fx/figurePickUp.wav", "figurePickUp");
	LoadFx("sound/fx/dsitemup.wav", "itemPickUp");
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - weapons
	LoadFx("sound/fx/dswpnup.wav", "weaponPickUp");
	LoadFx("sound/fx/dsshotgn.wav", "shotgunShotFire");
	LoadFx("sound/fx/dspistol.wav", "machineGunShotFire");
	LoadFx("sound/fx/dssawup.wav", "chainsawStart");
	LoadFx("sound/fx/dssawidl.wav", "chainsawIdle");
	LoadFx("sound/fx/dssawful.wav", "chainsawShotFire");
	LoadFx("sound/fx/dssawhit.wav", "chainsawHit");

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - enemies  
	LoadFx("sound/fx/dsclaw.wav", "EnemyIMPAttack");
	LoadFx("sound/fx/dsbgdth1.wav", "EnemyIMPDeath");
	LoadFx("sound/fx/dsbgdth2.wav", "EnemyIMPDeath2");
	LoadFx("sound/fx/dspopain.wav", "EnemyIMPInjured");
	LoadFx("sound/fx/dsbgact.wav", "EnemyIMPNearby");

	LoadFx("sound/fx/dsfirsht.wav", "EnemyCacodemonAttack"); // long range 
	LoadFx("sound/fx/dssgtdth.wav", "EnemyCacodemonDeath");
	LoadFx("sound/fx/dsdmpain.wav", "EnemyCacodemonInjured");
	LoadFx("sound/fx/dsdmact.wav", "EnemyCacodemonNearby");

	LoadFx("sound/fx/dsclaw.wav", "EnemyBaronOfHellAttack"); // melee 
	LoadFx("sound/fx/dsfirsht.wav", "EnemyBaronOfHellAttack2"); // long range 
	LoadFx("sound/fx/dsbrsdth.wav", "EnemyBaronOfHellDeath");
	LoadFx("sound/fx/dsdmpain.wav", "EnemyBaronOfHellInjured");
	LoadFx("sound/fx/dsdmact.wav", "EnemyBaronOfHellNearby");


	LoadFx("sound/fx/dsclaw.wav", "EnemyHellKnightAttack");  
	LoadFx("sound/fx/dsbrsdth.wav", "EnemyHellKnightDeath");
	LoadFx("sound/fx/dsdmpain.wav", "EnemyHellKnightInjured");
	LoadFx("sound/fx/dsdmact.wav", "EnemyHellKnightNearby");

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - player
	LoadFx("sound/fx/dsoof.wav", "fall");
	LoadFx("sound/fx/dash.wav", "dash");
	LoadFx("sound/fx/dsplpain.wav", "playerInjured");
	LoadFx("sound/fx/dspldeth.wav", "playerDeath");
	LoadFx("sound/fx/dsslop.wav", "playerDeath2");


	int i = 1; 
	for (auto chunk = fxMap.begin(); chunk != fxMap.end(); ++chunk)
	{
		(*chunk).second.actualChannel = i; 
		++i; 
	}

	Mix_AllocateChannels(i);    
 

	return true;
}

bool j1Audio::Update(float dt)
{

	BROFILER_CATEGORY("Audio Update", Profiler::Color::DeepPink);

	if (toUpdate == true)
		if (prioritaryChunk != "empty")
			if (isPlayingFx(prioritaryChunk) == false)
				ResetMusicAndFxVolumes();

	return true; 
}

void j1Audio::ResetMusicAndFxVolumes()
{

	SetVolume(previous_volume); 
	SetFxVolume(previous_fx_volume); 

	prioritaryChunk = "empty"; 
	toUpdate = false; 
}

// Called before quitting
bool j1Audio::CleanUp()
{

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	Mix_AllocateChannels(0);

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}


	for (auto& fx : fxMap)
	{
		Mix_FreeChunk(fx.second.chunk);
		fx.second.chunk = nullptr; 
	}
		
	fxMap.clear(); 

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if(!active)
		return false;

	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

			Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	musTimer.Start(); 

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path, std::string wantedName)
{
	unsigned int ret = 0;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		Mix_Chunk_Avanced advancedChunk; 
		advancedChunk.chunk = chunk; 
		fxMap.insert(std::make_pair(wantedName, advancedChunk));
		ret = fxMap.size();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(std::string name, int repeat, bool prioritary, float volume, float musReduc, float fxsReduc)
{
	
	if(!active)
		return false;

	if (existsFx(name) == false)
		return false;
	
	if (prioritary == true)
	{
		prioritaryChunk = name;  // capture the chunk name that has a priority 
		toUpdate = true; 

		SetVolume(musReduc);   // set the music to a lower volume while playing the wanted fx
		SetFxVolume(fxsReduc);   // set the other fxs to a lower volume while playing the wanted fx

	}

	SetSpecificFxVolume(name, volume);   // finally, set the desired fx to be played on top with the desired volume

	int channel = 0;

	channel = Mix_PlayChannel(fxMap.at(name).actualChannel, fxMap.at(name).chunk, repeat);


	return true;
}

bool j1Audio::SetSpecificFxVolume(std::string name, float volume)
{
	bool ret = false;

	if (!active)
		return false;

	final_fx_volume = MIX_MAX_VOLUME * volume;
	if (final_fx_volume < 0.0f || final_fx_volume > MIX_MAX_VOLUME)
		final_fx_volume = (final_fx_volume < 0.0f) ? 0.0f : MIX_MAX_VOLUME;


	if (existsFx(name) == true)  
		Mix_VolumeChunk(fxMap.at(name).chunk, final_fx_volume);

	return ret;
}



void j1Audio::SetVolume(float volume)
{
	previous_volume = last_volume;


	if (previous_volume == 0.f)
		previous_volume = 0.5f;  // default equals max volume / 2 = 64 (0.5f)

	final_volume = MIX_MAX_VOLUME * volume;
	if (final_volume < 0.0f || final_volume > MIX_MAX_VOLUME)
		final_volume = (final_volume < 0.0f) ? 0.0f : MIX_MAX_VOLUME;

	Mix_VolumeMusic(final_volume);
	last_volume = volume;
}

void j1Audio::SetFxVolume(float volume)
{
	previous_fx_volume = last_fx_volume;

	if (previous_fx_volume == 0.f)
		previous_fx_volume = 0.5f; // default equals max volume / 2 = 64 (0.5f)

	final_fx_volume = MIX_MAX_VOLUME * volume;
	if (final_fx_volume < 0.0f || final_fx_volume > MIX_MAX_VOLUME)
		final_fx_volume = (final_fx_volume < 0.0f) ? 0.0f : MIX_MAX_VOLUME;

	for (auto& fx : fxMap)
		Mix_VolumeChunk(fx.second.chunk, final_fx_volume);
	
	
	last_fx_volume = volume; 

	/*for (std::list<Mix_Chunk*>::iterator item_fx = fx.begin();item_fx!=fx.end() ; ++item_fx)
	{
		Mix_VolumeChunk((*item_fx), final_fx_volume);
		
	}*/
}

void j1Audio::StopAllFxs()
{
	if (toUpdate == true)
		if (prioritaryChunk != "empty")
			if (isPlayingFx(prioritaryChunk) == false)
				ResetMusicAndFxVolumes();
	
	 for(auto& item : fxMap)
		 if (isPlayingFx(item.first) == true)
			 Mix_HaltChannel(item.second.actualChannel);
}

void j1Audio::StopSpecificFx(std::string name)
{
	// prevention
	auto item = fxMap.find(name);
	if (item == fxMap.end())
		return;

	if (isPlayingFx((*item).first) == true)
		Mix_HaltChannel((*item).second.actualChannel);
	
}


void j1Audio::PauseSpecificFx(std::string name)
{
	// prevention
	auto item = fxMap.find(name);
	if (item == fxMap.end())
		return;

	if (isPlayingFx((*item).first) == true)
			Mix_Pause((*item).second.actualChannel);

	
}

void j1Audio::ResumeSpecificFx(std::string name)
{
	// prevention
	auto item = fxMap.find(name);
	if (item == fxMap.end())
		return;

	if (isPausedFx((*item).first))
		Mix_Resume((*item).second.actualChannel);

}

bool j1Audio::isPlayingFx(std::string name)
{
	// prevention
	auto item = fxMap.find(name); 
	if ( item == fxMap.end())
		return false; 

	if (Mix_Playing((*item).second.actualChannel) == 0)
		return false;
	return true;
}

bool j1Audio::isPausedFx(std::string name)
{
	// prevention
	auto item = fxMap.find(name);
	if (item == fxMap.end())
		return false;

	if (Mix_Paused((*item).second.actualChannel) == 0)
		return false;
	return true;
}


