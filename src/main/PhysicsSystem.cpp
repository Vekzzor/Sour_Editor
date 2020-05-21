#include "PhysicsSystem.h"
#include "pch.h"

PhysicsSystem::PhysicsSystem()
	: world_(b2Vec2(0.0f, 9.82f))
{}

PhysicsSystem::~PhysicsSystem() {}

void PhysicsSystem::Init() {}

void PhysicsSystem::CreateCircleCollider(Entity _entity,
										 b2BodyType _bodyType,
										 b2Vec2 _pos,
										 float _radius)
{
	PhysicsObject physObj;
	b2BodyDef bodyDef;
	bodyDef.type   = _bodyType;
	physObj.body = world_.CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = _radius;
	shape.m_p	  = _pos;

	b2FixtureDef fixtureDef;
	fixtureDef.density  = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.shape	= &shape;

	physObj.body->CreateFixture(&fixtureDef);
	gCoordinator.GetInstance().AddComponent(_entity, physObj);
}

void PhysicsSystem::CreateBoxCollider(
	Entity _entity, b2BodyType _bodyType, float _hx, float _hy, b2Vec2 _center, float _angle)
{

	PhysicsObject physObj;
	b2BodyDef bodyDef;
	bodyDef.type   = _bodyType;
	physObj.body = world_.CreateBody(&bodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(_hx, _hy, _center, _angle);

	b2FixtureDef fixtureDef;
	fixtureDef.density  = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.shape	= &shape;

	physObj.body->CreateFixture(&fixtureDef);
	gCoordinator.GetInstance().AddComponent(_entity, physObj);
}

void PhysicsSystem::CreateMeshCollider(Entity _entity,
									   std::vector<b2Vec2>& _vertices,
									   b2BodyType _bodyType)
{
	PhysicsObject physObj;
	b2BodyDef bodyDef;
	bodyDef.type			 = _bodyType;
	Coordinator& coordinator = gCoordinator.GetInstance();

	physObj.body   = world_.CreateBody(&bodyDef);
	auto& trans		 = coordinator.GetComponent<sf::Transformable>(_entity);
	sf::Vector2f pos = trans.getOrigin();
	pos.x *= trans.getScale().x;
	pos.y *= trans.getScale().y;

	physObj.body->SetTransform(b2Vec2(pos.x / kPixelToMeterRatio, pos.y / kPixelToMeterRatio),
								 trans.getRotation() * (b2_pi / 180));
	b2PolygonShape polyShape;
	int vertCount = _vertices.size();
	std::vector<int> fixtureV(std::ceil(vertCount / 8.0f));
	for(int i = 0; i < fixtureV.size(); i++)
	{
		fixtureV[i] = std::min(8, vertCount);
		vertCount -= 8;
	}
	if(fixtureV.back() < 3)
	{
		fixtureV[fixtureV.size() - 2] -= 2;
		fixtureV.back() += 2;
	}
	vertCount	= 0;
	bool created = false;
	for(int i = 0; i < fixtureV.size(); i++)
	{
		vertCount += fixtureV[i];
		std::vector<b2Vec2>::const_iterator first =
			_vertices.begin() + std::fmin(vertCount - fixtureV[i], fixtureV.size() - 8);
		std::vector<b2Vec2>::const_iterator last = _vertices.begin() + vertCount;
		std::vector<b2Vec2> newVec(first, last);
		for (auto& index : newVec)
		{
			index = index - physObj.body->GetPosition();
		}
		polyShape.Set(newVec.data(), newVec.size());
		b2FixtureDef fixtureDef;
		fixtureDef.density  = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.shape	= &polyShape;
		physObj.fixtures.push_back(physObj.body->CreateFixture(&fixtureDef));
		created = true;
	}
	coordinator.AddComponent(_entity, physObj);
}

void PhysicsSystem::CreateMeshCollider(Entity _entity,
									   b2BodyType _body_type)
{
	PhysicsObject physObj;
	b2BodyDef bodyDef;
	bodyDef.type			 = _body_type;
	Coordinator& coordinator = gCoordinator.GetInstance();

	physObj.body   = world_.CreateBody(&bodyDef);
	auto& trans		 = coordinator.GetComponent<sf::Transformable>(_entity);
	auto& mesh		 = coordinator.GetComponent<Mesh2D>(_entity);
	sf::Vector2f pos = trans.getPosition();
	//pos.x			 *= trans.getScale().x;
	//pos.y			 *= trans.getScale().y;
	physObj.body->SetTransform(b2Vec2(pos.x / kPixelToMeterRatio, pos.y / kPixelToMeterRatio),
								 trans.getRotation() * (b2_pi / 180));
	b2PolygonShape polyShape;
	if (mesh.vertices.size() > 8)
	{
		int vertCount	= mesh.vertices.size();
		int nrOfFixtures = std::ceil(vertCount / 8.0f);
		int remainder	= vertCount % 7;
		std::vector<int> fixtureV(nrOfFixtures, 7);
		fixtureV.back() = remainder;
		vertCount		= 2;
		for(int i = 0; i < fixtureV.size(); i++)
		{
			std::vector<b2Vec2> newVec;
			newVec.reserve(fixtureV[i] + 1);
			vertCount--;
			int start = vertCount;
			int end   = vertCount + fixtureV[i];
			vertCount += fixtureV[i];

			sf::Vector2f meshVert = trans.getTransform() * mesh.vertices.front().position;
			b2Vec2 vert{meshVert.x / kPixelToMeterRatio, meshVert.y / kPixelToMeterRatio};
			vert = vert - physObj.body->GetPosition();
			newVec.push_back(vert);

			for(int i = start; i < end; i++)
			{
				meshVert = trans.getTransform() * mesh.vertices[i].position;
				vert.x = meshVert.x / kPixelToMeterRatio, vert.y = meshVert.y / kPixelToMeterRatio;
				vert = vert - physObj.body->GetPosition();
				newVec.push_back(vert);
			}
			polyShape.Set(newVec.data(), newVec.size());
			b2FixtureDef fixtureDef;
			fixtureDef.density  = 1.0f;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape	= &polyShape;
			physObj.fixtures.push_back(physObj.body->CreateFixture(&fixtureDef));
		}
	}
	else
	{
		std::vector<b2Vec2> newVec;
		newVec.reserve(mesh.vertices.size());
		for(auto& vertex : mesh.vertices)
		{
			sf::Vector2f meshVert = trans.getTransform() * vertex.position;
			b2Vec2 vert;
			vert.x = meshVert.x / kPixelToMeterRatio, vert.y = meshVert.y / kPixelToMeterRatio;
			vert = vert - physObj.body->GetPosition();
			newVec.push_back(vert);
		}
		polyShape.Set(newVec.data(), newVec.size());
		b2FixtureDef fixtureDef;
		fixtureDef.density  = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.shape	= &polyShape;
		physObj.fixtures.push_back(physObj.body->CreateFixture(&fixtureDef));
	}
	coordinator.AddComponent(_entity, physObj);
}

void PhysicsSystem::CreateMeshCollider(Entity _entity, Mesh2D _polygon, b2BodyType _body_type)
{
	std::vector<b2Vec2> vertices(_polygon.vertices.size());
	for(int i = 0; i < _polygon.vertices.size(); i++)
		vertices[i] = b2Vec2(_polygon.vertices[i].position.x / kPixelToMeterRatio,
							 _polygon.vertices[i].position.y / kPixelToMeterRatio);

	PhysicsObject physObj;
	b2BodyDef bodyDef;
	bodyDef.type   = _body_type;
	physObj.body = world_.CreateBody(&bodyDef);

	b2PolygonShape polyShape;
	int vertCount = vertices.size();
	std::vector<int> fixtureV(std::ceil(vertCount / 8.0f));
	for(int i = 0; i < fixtureV.size(); i++)
	{
		fixtureV[i] = std::min(8, vertCount);
		vertCount -= 8;
	}
	if(fixtureV.back() < 3)
	{
		fixtureV[fixtureV.size() - 2] -= 2;
		fixtureV.back() += 2;
	}
	vertCount	= 0;
	bool created = false;
	for(int i = 0; i < fixtureV.size(); i++)
	{
		vertCount += fixtureV[i];
		std::vector<b2Vec2>::const_iterator first =
			vertices.begin() + std::fmin(vertCount - fixtureV[i], fixtureV.size() - 8);
		std::vector<b2Vec2>::const_iterator last = vertices.begin() + vertCount;
		std::vector<b2Vec2> newVec(first, last);
		polyShape.Set(newVec.data(), newVec.size());
		b2FixtureDef fixtureDef;
		fixtureDef.density  = 1.0f;
		fixtureDef.friction = 0.3f;
		fixtureDef.shape	= &polyShape;
		physObj.fixtures.push_back(physObj.body->CreateFixture(&fixtureDef));
		created = true;
	}

	gCoordinator.GetInstance().AddComponent(_entity, physObj);
}

void PhysicsSystem::DestroyPhysicsBody(Entity _entity)
{
	Coordinator& coordinator = gCoordinator.GetInstance();
	world_.DestroyBody(coordinator.GetComponent<PhysicsObject>(_entity).body);
}

void PhysicsSystem::Update()
{
	for(auto const& entity : entities_)
	{
		Coordinator& coordinator = gCoordinator.GetInstance();
		auto& physObj			 = coordinator.GetComponent<PhysicsObject>(entity);
		auto& transform			 = coordinator.GetComponent<sf::Transformable>(entity);

		auto lastPos = transform.getPosition();
		auto newPos  = sf::Vector2f(physObj.body->GetPosition().x * kPixelToMeterRatio,
									physObj.body->GetPosition().y * kPixelToMeterRatio);
		//newPos		 = transform.getTransform() * newPos;
		auto delta = newPos - lastPos;

		if(physObj.body->IsEnabled())
		{
			transform.move(delta);
			transform.setRotation(physObj.body->GetAngle() * (180 / b2_pi));
		}
		else
		{
			b2Vec2 newBodyPos(-delta.x / kPixelToMeterRatio, -delta.y / kPixelToMeterRatio);
			physObj.body->SetTransform(physObj.body->GetPosition() + newBodyPos,
										 transform.getRotation() * (b2_pi / 180));
		}
	}
}

/*void PhysicsSystem::Update()
{
	for (auto const& entity : mEntities)
	{
		auto& selectable = gCoordinator.GetComponent<Selectable>(entity);
		auto& transform = gCoordinator.GetComponent<sf::Transformable>(entity);
		auto& rigidbody = gCoordinator.GetComponent<b2Body>(entity);
		auto& geometry = gCoordinator.GetComponent<VoronoiGeometry>(entity);

		auto lastPos = transform.getPosition();
		auto newPos = sf::Vector2f(rigidbody.GetPosition().x * 10000, rigidbody.GetPosition().y * 10000);

		auto delta = newPos - lastPos;
		selectable.AABB_lowerBounds[0] += delta.x, selectable.AABB_lowerBounds[1] += delta.y;
		selectable.AABB_upperBounds[0] += delta.x, selectable.AABB_upperBounds[1] += delta.y;
		transform.setPosition(newPos);
		transform.setRotation(rigidbody.GetAngle()*(180 / b2_pi));

		sf::Vector2f minBound = { FLT_MAX, FLT_MAX };
		sf::Vector2f maxBound = { -FLT_MAX, -FLT_MAX };
		for (auto &vertex : geometry.m_vertices)
		{
			sf::Vector2f pos = transform.getTransform()* vertex.position;
			minBound.x = std::min(pos.x, minBound.x);
			minBound.y = std::min(pos.y, minBound.y);
			maxBound.x = std::max(pos.x, maxBound.x);
			maxBound.y = std::max(pos.y, maxBound.y);
		}

		geometry.m_lower.setPosition(minBound);
		geometry.m_upper.setPosition(maxBound);
		selectable.AABB_lowerBounds[0] = minBound.x, selectable.AABB_lowerBounds[1] = minBound.y;
		selectable.AABB_upperBounds[0] = maxBound.x, selectable.AABB_upperBounds[1] = maxBound.y;
	}
};*/