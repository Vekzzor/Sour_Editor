#pragma once
#include "ECS/ECS.h"
#include "RenderSystem.h"
class PhysicsSystem : public System
{
public:
	PhysicsSystem();
	~PhysicsSystem();
	void Init();
	//UNFINISHED
	void CreateCircleCollider(Entity _entity, b2BodyType _body_type, b2Vec2 _pos, float _radius);
	void CreateBoxCollider( Entity _entity, b2BodyType _body_type, float _hx, float _hy, b2Vec2 _center = b2Vec2(), float _angle = 0 );
	void CreateMeshCollider(Entity _entity, std::vector<b2Vec2>& _vertices, b2BodyType _body_type);
	void CreateMeshCollider(Entity _entity, b2BodyType _body_type);
	void CreateMeshCollider(Entity _entity, Mesh2D _polygon, b2BodyType _body_type);
	void DestroyPhysicsBody(Entity _entity);
	void Update();
private:
	b2World world_;
	int32 velocity_iterations_ = 8;
	int32 position_iterations_ = 3;
	const float kTimeStep = 1.0f / 60.0f;
public:
	void SetDebugDraw(b2Draw* _draw) { world_.SetDebugDraw(_draw); };
	void WorldStep() { world_.Step(kTimeStep, velocity_iterations_, position_iterations_); };
	const float getTimeStep() { return kTimeStep; };
	//TEMP
	void WorldDebugDraw() { world_.DebugDraw(); };
};





