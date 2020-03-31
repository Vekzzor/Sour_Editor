#pragma once
#include "ECS/ECS.h"
class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();
	void CreatePhysicsBody(b2World world, std::vector<b2Vec2>& vertices);
	void Update();
};





