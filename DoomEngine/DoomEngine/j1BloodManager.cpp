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

	if (App->pause == true)
		return true;

	bool ret = true;

	return true;
}

bool j1BloodManager::Update(float dt)
{
	BROFILER_CATEGORY("Blood Update", Profiler::Color::White);


	if (App->pause == true)
		return true;

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
						(*bloodDrop)->Update(dt);
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
 
	return true;
}

bool j1BloodManager::PostUpdate()
{
	BROFILER_CATEGORY("Blood PostUpdate", Profiler::Color::WhiteSmoke);


	for (const auto& stream : bloodStreams)
		for(const auto& drop : stream->myBloodDrops)
			if (App->render->IsOnCamera(drop->position.x, drop->position.y, drop->size.x, drop->size.y))
				drop->Draw();
				


	return true;
}

bool j1BloodManager::CleanUp()
{
	bool ret = true;

	// for each blood stream clear each blood drop (collider to delete), then clear the blood stream itself from the global list

	for (auto& stream : bloodStreams)
	{
		for (auto& bloodDrop : stream->myBloodDrops)
		{
			bloodDrop->CleanUp();   // check entity cleanup is ineed called; 
			RELEASE(bloodDrop); 
		}
		stream->myBloodDrops.clear(); 
		RELEASE(stream);
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


void j1BloodManager::CreateTargetedBloodSteam(SDL_Rect enemyRect, float damage, uint numberOfBloodDrops, fPoint shotSpeed)
{
	if (shotSpeed.IsZero() == true)  // caution, when near (1 tile away from enemy) 
	{
		CreateRandomBloodStream(enemyRect, 0.5F, numberOfBloodDrops);
		return; 
	}
	
	bloodStream* stream = DBG_NEW bloodStream;

	// for the random stream, get a central area inside the enemy rect; 
	SDL_Rect centerOriginArea = GetRectCentralArea(enemyRect, 0.85F);    // get the central area, let it be smaller so it seems more targeted 

	for (int i = 0; i < numberOfBloodDrops; ++i)
	{
		float originXpos = GetRandomValue(centerOriginArea.x, centerOriginArea.x + centerOriginArea.w);   // get a X value inside area
		float originYpos = GetRandomValue(centerOriginArea.y, centerOriginArea.y + centerOriginArea.h);   // get a Y value inside area

		fPoint speed = GenerateTargetedSpeedForBloodDrop(shotSpeed);
		Color c = GenerateColorForBloodDrop();

		j1EntityBloodDrop* drop = DBG_NEW j1EntityBloodDrop(originXpos, originYpos, speed, c);

		if (drop != nullptr)
			stream->myBloodDrops.push_back(drop);

	}

	bloodStreams.push_back(stream);
}

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

	speed.x = GetRandomValue(-bloodModuleSpeed * randomBloodSpeedXFactor, bloodModuleSpeed * randomBloodSpeedXFactor);  // between negative max and positive max
	speed.y = sqrt(pow(bloodModuleSpeed, 2.f) - pow(speed.x, 2.f));  // this calculus is always positive

	/*float speedYSign = GetRandomIntValue(0, 1);    // give the last calculus a proper sign
    if (speedYSign == 0)*/
		speed.y = -speed.y;    // always positive at start, like a fountain 

	return speed; 

}

fPoint j1BloodManager::GenerateTargetedSpeedForBloodDrop(fPoint shotSpeed)
{
	fPoint speed = fPoint(0, 0); 
	 
	 
	if (shotSpeed.x != 0)
		shotSpeed.x /= abs(shotSpeed.x);    // direction: either 1 or -1 (or it was 0 before) 
	if (shotSpeed.y != 0)
		shotSpeed.y /= abs(shotSpeed.y);    // direction: either 1 or -1 (or it was 0 before)

	if (shotSpeed.x == 0 && shotSpeed.y != 0)
	{
		speed.y = bloodModuleSpeed * shotSpeed.y;   // take a module and the direction 
	}
	else if (shotSpeed.y == 0 && shotSpeed.x != 0)
	{
		speed.x = bloodModuleSpeed * shotSpeed.x;    // take a module and the direction 
	}
	else   // diagonals
	{
		speed.x = sqrt((pow(bloodModuleSpeed, 2) / 2));   // from the hipotenuse formula, with two identical cathetus 
		speed.y = speed.x; 
		speed.x *= shotSpeed.x;    // give it a direction 
		speed.y *= shotSpeed.y;    // give it a direction 

	}

	// do a little spectrum so they do not have exactly the same speed, don't mind being a tad bigger than module

	float semiRange = 1.5f;

	speed.x += GetRandomValue(-semiRange, semiRange); 
	speed.y += GetRandomValue(-semiRange, semiRange);
	
	return speed; 
}


uint j1BloodManager::CalculateNumberOfBloodDropsToBeSpawned(float damage, float shotsPerSec)
{
	float drops = 0; 

	if (shotsPerSec == 0.0f) // chainsaw, 1 shot per frame, = 60 shots per second (60 FPS)
	{
		shotsPerSec = 60.f;  
	}

	if (damage == 0.0f || shotsPerSec == 0.0f)  // prevention for the next line divisions
		return 0U;  

	if (!App->entityFactory->player->myWeapons.empty())
		if (App->entityFactory->player->currentWeapon->GetWeaponType() == WEAPON_TYPE::CHAINSAW
			|| App->entityFactory->player->currentWeapon->GetWeaponType() == WEAPON_TYPE::MACHINE_GUN)   // cause why not??  
		{
			int prop = GetRandomIntValue(1, (int)shotsPerSec);

			if (prop < 20)
				return 1U;
			else
				return 0U;
		}



	drops = (damage * (damage / (damage * 7.f))) * (1.f / shotsPerSec);
	drops /= 3; 

	assert(drops < 1000 && "Too much blood drops!! Something went wrong :/");

	if (drops >= 1000)
		return 0U; 

	if (drops < 1)
	{
		int prop = GetRandomIntValue(1, (int)shotsPerSec);
		prop = abs(prop); 

		uint calc = 0U; 

		if (prop > 0U)
			calc = (uint)(int)(shotsPerSec / (float)prop);


		assert(calc < 1000 && "Too much blood drops!! Something went wrong :/");

		if (calc >= 1000)
			return 0U; 

		if (calc <= 1U)
			calc = 1U; 
		return calc; 
	}
		
	return (uint)(int)drops; 

}


bool j1BloodManager::Load(pugi::xml_node& node)
{
	for (auto streamNode = node.child("blood_stream"); streamNode; streamNode = streamNode.next_sibling("blood_stream"))
	{
		bloodStream* stream = DBG_NEW bloodStream;

		// append blood
		for (auto bloodDropNode = streamNode.child("blood_drop"); bloodDropNode; bloodDropNode = bloodDropNode.next_sibling("blood_drop"))
		{
			
			auto posNode = bloodDropNode.child("position"); 
			fPoint position = fPoint(posNode.attribute("x").as_float(), posNode.attribute("y").as_float()); 
			auto speedNode = bloodDropNode.child("last_speed");
			fPoint speed = fPoint(speedNode.attribute("x").as_float(), speedNode.attribute("y").as_float());
			auto colorNode = bloodDropNode.child("color");
			float r = colorNode.attribute("R").as_float(); 
			float g = colorNode.attribute("G").as_float();
			float b = colorNode.attribute("B").as_float();
			float a = colorNode.attribute("A").as_float();
			Color c = { r, g, b, a }; 

			j1EntityBloodDrop* drop = DBG_NEW j1EntityBloodDrop(position.x, position.y, speed, c);


			// now assign extra creation data that had been captured the previous gameplay
			drop->colliderActive = bloodDropNode.child("collider_active").attribute("value").as_bool(); 
			drop->to_delete = bloodDropNode.child("to_delete").attribute("value").as_bool();
			drop->floorReached = bloodDropNode.child("floor_reached").attribute("value").as_bool();
			drop->roofReached = bloodDropNode.child("roof_reached").attribute("value").as_bool();

			auto lastPosColNode = node.child("last_position_collider");
			drop->lastPosCollider.x = lastPosColNode.attribute("x").as_int(); 
			drop->lastPosCollider.y = lastPosColNode.attribute("y").as_int();
			drop->lastPosCollider.w = lastPosColNode.attribute("w").as_int();
			drop->lastPosCollider.h = lastPosColNode.attribute("h").as_int();

			drop->viscosityData.frameCounter = bloodDropNode.child("viscosity_actual_frame").attribute("value").as_uint(); 
 
			auto entitiesNode = bloodDropNode.child("related_entities_node");
			bool hasDynGroundCallback = entitiesNode.child("dynamic_ground_callback").child("has_dynamic_ground_callback").attribute("value").as_bool();

			if (hasDynGroundCallback == true)
			{
				uint groundEntityID = entitiesNode.child("dynamic_ground_callback").child("dynamic_ground_ID").attribute("value").as_uint();
				drop->dynGroundCallback = (j1EntityPlatformDynamic*)App->entityFactory->GetEntityFromID(groundEntityID); 
			}

			// then push it to the stream
			if (drop != nullptr)
				stream->myBloodDrops.push_back(drop);

		}

		// finally push the straem in the global list
		bloodStreams.push_back(stream); 

	}

	return true; 
}

bool j1BloodManager::Save(pugi::xml_node& node) const 
{
	for (auto& stream : bloodStreams)
		stream->Save((pugi::xml_node&)node.append_child("blood_stream"));

	return true;
}

 