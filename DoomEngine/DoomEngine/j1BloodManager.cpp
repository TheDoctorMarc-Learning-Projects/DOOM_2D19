#include "j1BloodManager.h"
#include "j1Render.h"
#include "Brofiler/Brofiler.h"


j1BloodManager::j1BloodManager()
{
	name.assign("blood");

	engine.seed(rd()); 
}

j1BloodManager::~j1BloodManager()
{
}

bool j1BloodManager::Awake(pugi::xml_node & node)
{
	bool ret = true;

	return ret;
}

bool j1BloodManager::Start()
{

	return true;
}

bool j1BloodManager::PreUpdate()
{
	BROFILER_CATEGORY("Blood PreUpdate", Profiler::Color::Wheat);

	bool ret = true;

	return true;
}

bool j1BloodManager::Update(float dt)
{
	bool ret = true;
	BROFILER_CATEGORY("Blood Update", Profiler::Color::White);

	// update blood pos 

	if (!bloodStreams.empty())
	{
		for (auto stream = bloodStreams.begin(); stream != bloodStreams.end();)
		{
			for (auto bloodDrop = (*stream)->myBloodDrops.begin(); bloodDrop != (*stream)->myBloodDrops.end();)
			{
				if ((*bloodDrop) != nullptr)
				{
					if (!(*bloodDrop)->to_delete)
					{

						ret = (*bloodDrop)->Update(dt);

						++bloodDrop;

					}
					else
					{
						(*bloodDrop)->CleanUp();
						RELEASE(*bloodDrop);
						*bloodDrop = nullptr;
						bloodDrop = (*stream)->myBloodDrops.erase(bloodDrop);


					}

				}
				else
				{
					bloodDrop = (*stream)->myBloodDrops.erase(bloodDrop);
				}

				

			}

			if ((*stream)->myBloodDrops.size() == 0)
				stream = bloodStreams.erase(stream);
			else
				++stream;

		}

	}
	


	return ret;
}

bool j1BloodManager::PostUpdate()
{
	BROFILER_CATEGORY("Blood PostUpdate", Profiler::Color::WhiteSmoke);


	for (auto stream : bloodStreams)
		for(auto drop : stream->myBloodDrops)
			if (!drop->to_delete)
				if (App->render->IsOnCamera(drop->position.x, drop->position.y, drop->size.x, drop->size.y))
					drop->Draw();
		


	return true;
}

bool j1BloodManager::CleanUp()
{
	bool ret = true;

	// for each blood stream celar each blood drop (collider to delete), then clear the blood stream itself from the global list

	for (auto stream = bloodStreams.begin(); stream != bloodStreams.end(); ++stream)
	{
		for (auto bloodDrop = (*stream)->myBloodDrops.begin(); bloodDrop != (*stream)->myBloodDrops.end(); ++bloodDrop)
		{
			(*bloodDrop)->CleanUp();   // check entity cleanup is ineed called; 
			RELEASE(*bloodDrop); 
			*bloodDrop = nullptr; 
		}
		(*stream)->myBloodDrops.clear(); 
	}
	bloodStreams.clear(); 
	

	return ret;
}


void j1BloodManager::CreateRandomBloodStream(SDL_Rect enemyRect, float centerAreaScaleFactor, uint numberOfBloodDrops)
{
	bloodStream* stream = DBG_NEW bloodStream; 

	// for the random stream, get a central area inside the enemy rect; 
	SDL_Rect centerOriginArea = GetRectCentralArea(enemyRect, centerAreaScaleFactor);

	for (int i = 0; i < numberOfBloodDrops; ++i)
	{
		float originXpos = GetRandomValue(centerOriginArea.x, centerOriginArea.x + centerOriginArea.w);   // get a X value inside area
		float originYpos = GetRandomValue(centerOriginArea.y, centerOriginArea.y + centerOriginArea.h);   // get a Y value inside area

		fPoint speed = GenerateRandomSpeedForBloodDrop(); 
		Color c = GenerateColorForBloodDrop(); 
		
		j1EntityBloodDrop* drop = DBG_NEW j1EntityBloodDrop(originXpos, originYpos, speed, c); 

		if(drop != nullptr)
			stream->myBloodDrops.push_back(drop);
	
	}

	bloodStreams.push_back(stream); 
	
}


/*void j1BloodManager::CreateTargetedBloodSteam()
{

}*/

SDL_Rect j1BloodManager::GetRectCentralArea(SDL_Rect enemyRect, float centerAreaScaleFactor)
{
	bool biggerSideIsWidth = false; 
	int enemyRectBiggerSide = (enemyRect.w > enemyRect.h) ? enemyRect.w : enemyRect.h;
	int centerAreaBigSide = (int)((float)enemyRectBiggerSide * centerAreaScaleFactor);

	int enemyRectSmallerSide = (enemyRect.w < enemyRect.h) ? enemyRect.w : enemyRect.h;
	int centerAreaSmallSide = (int)((float)enemyRectSmallerSide * centerAreaScaleFactor);

	if (enemyRectBiggerSide == enemyRect.w)
		biggerSideIsWidth = true; 

	iPoint enemyRectCenterPos = iPoint(enemyRect.x + enemyRect.w / 2, enemyRect.y + enemyRect.h / 2); 
	iPoint targetpos = iPoint(0, 0); 

	SDL_Rect definitiveRect = { 0, 0, 0, 0};

	if (biggerSideIsWidth)
	{
		targetpos = iPoint(enemyRectCenterPos.x - centerAreaBigSide / 2, enemyRectCenterPos.y - centerAreaSmallSide / 2);
		definitiveRect = { targetpos.x, targetpos.y, centerAreaBigSide, centerAreaSmallSide };
	}
	else
	{
		targetpos = iPoint(enemyRectCenterPos.x - centerAreaSmallSide / 2, enemyRectCenterPos.y - centerAreaBigSide / 2);
		definitiveRect = { targetpos.x, targetpos.y, centerAreaSmallSide, centerAreaBigSide };
	}
		

	return definitiveRect; 
}


float j1BloodManager::GetRandomValue(float v1, float v2)
{

	std::uniform_real_distribution<float> range(v1, v2);

	float ret = range(rd);


	return ret;
}

int j1BloodManager::GetRandomIntValue(int v1, int v2)
{
	std::uniform_int_distribution<int> range(v1, v2);

	int ret = range(rd);


	return ret;
}

Color j1BloodManager::GenerateColorForBloodDrop()
{
	Color ret; 
	ret.r = GetRandomValue(BloodRedMostColor.r, BloodBlackMostColor.r);
	ret.g = GetRandomValue(BloodRedMostColor.g, BloodBlackMostColor.g);
	ret.b = GetRandomValue(BloodRedMostColor.b, BloodBlackMostColor.b);
	ret.a = GetRandomValue(BloodRedMostColor.a, BloodBlackMostColor.a);
    
	return ret; 
}

fPoint j1BloodManager::GenerateRandomSpeedForBloodDrop()
{
	fPoint speed = fPoint(0, 0); 

	speed.x = GetRandomValue(-bloodModuleSpeed, bloodModuleSpeed);  // between negative max and positive max
	speed.y = sqrt(pow(bloodModuleSpeed, 2.f) - pow(speed.x, 2.f));  // this calculus is always positive

	float speedYSign = GetRandomIntValue(0, 1);    // give the last calculus a proper sign
	if (speedYSign == 0)
		speed.y = -speed.y; 

	return speed; 

}

uint j1BloodManager::CalculateNumberOfBloodDropsToBeSpawned(float damage, float shotsPerSec)
{
	float drops = 0; 
	
	// if it shots like chainsaw, that means that each frame blood is generated, so there would be 60+ blood drops per second


	if (shotsPerSec == 0.0f) // chainsaw, 1 shot per frame, = 60 shots per second (60 FPS)
	{
		shotsPerSec = 60.f; // App->GetFPS();
	}

	drops = (damage * (damage / (damage * 5.f))) * (1.f / shotsPerSec);

	if (drops < 1)
	{
		
		int prop = GetRandomIntValue(0, (int)shotsPerSec);

		if (prop < 10)
			return 1U; 
	}
		
	return (uint)(int)drops; 

}