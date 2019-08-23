#ifndef _J1ENEMYCACODEMON_H__
#define _J1ENEMYCACODEMON_H__

#include "j1Enemy.h"
#include "j1EntityPlatform.h"

#define extraPlatformTendencyOffset 5

struct actualCollisionType
{
	bool top = false; 
	bool bottom = false; 
	bool right = false; 
	bool left = true; 
};

class j1Enemy;

class j1EnemyCacodemon : public j1Enemy
{
public:
	j1EnemyCacodemon(int posX, int posY, std::string name);
	~j1EnemyCacodemon();

	// functionality ------

	//void ResolvePathDeviation();

	bool Move(float dt) override;  // call parent function, then if enemy does enaything extra, do it after call 

	void OnCollision(Collider* c1, Collider* c2) override;       
    void OnCollisionExit(Collider* c1, Collider* c2) override;    

	bool CleanUp() override;
	bool DoDie() override; 

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void SetDeviation(bool horizontal, Collider* c2);
	void KeepMovingTendency(); 

	void ResetPlatformState() {
		onPlatFormType.bottom = false; 
		onPlatFormType.left = false; 
		onPlatFormType.right = false; 
		onPlatFormType.top = false; 
	}

	iPoint GetShieldPos() const { return iPoint(position.x - shieldExtraSideSize / 2, position.y - shieldExtraSideSize / 2); };
	iPoint GetLastShieldPos() const { return iPoint(lastAirPos.x - shieldExtraSideSize / 2, lastAirPos.y - shieldExtraSideSize / 2); };

	SDL_Rect GetShieldRect() const {
		return { collider->rect.x - shieldExtraSideSize / 2, collider->rect.y - shieldExtraSideSize / 2,
				 collider->rect.w + shieldExtraSideSize, collider->rect.h + shieldExtraSideSize };
	};
	SDL_Rect GetLastShieldRect() const {
		return { lastPosCollider.x - shieldExtraSideSize / 2, lastPosCollider.y - shieldExtraSideSize / 2,
                 lastPosCollider.w + shieldExtraSideSize, lastPosCollider.h + shieldExtraSideSize }; 
	}; 

private:
	int shieldExtraSideSize = 30; 
	fPoint offPlatformPos = fPoint(0, 0); 
	bool keepMovingAfterPlatform = false; 
	bool shieldActive = true; 
	actualCollisionType onPlatFormType; 
	Collider* shieldAreaCollider = nullptr; 
	j1EntityPlatform* lastPlatform = nullptr;
};

#endif 


