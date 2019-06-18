#include "j1App.h"
#include "j1Entity.h"
#include "j1Render.h"
#include "j1EntityFactory.h"
#include "j1Window.h"
#include "p2Log.h"
#include "Brofiler\Brofiler.h"
#include "j1PathFinding.h"
#include "j1Map.h"

j1Entity::j1Entity(ENTITY_TYPE type, float positionX, float positionY,std::string name) : type(type), position(positionX,positionY), name(name)
{}



j1Entity::~j1Entity()
{
}

bool j1Entity::Start()
{
	return true;
}

bool j1Entity::PreUpdate()
{
	return true;
}

bool j1Entity::Update(float dt)
{
	return true;
}
bool j1Entity::PostUpdate()
{

	return true;
}

bool j1Entity::CleanUp()
{

	return true;
}

void j1Entity::Draw()
{
	//App->render->DrawCircle((position.x + pivot.x), (position.y + pivot.y), 3, 255, 0, 0, 255, false);
	

	if (entityTex != nullptr) // if we have any specific linked texture
	{
		App->render->Blit(entityTex, position.x, position.y, &atlasRect);
	}
	else // if not, use atlas
	{
		if (currentAnimation != nullptr) // if we have defined animation
			App->render->Blit(App->entityFactory->atlasTexture, position.x, position.y, &currentAnimation->GetCurrentFrame());
		else
			App->render->Blit(App->entityFactory->atlasTexture, position.x, position.y, &atlasRect);
	}

}

fPoint j1Entity::GetPosition()
{
	return position;
}

bool j1Entity::Move(float dt)
{
	return true;
}

