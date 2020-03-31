#include "PhysicsSystem.h"
#include "pch.h"

PhysicsSystem::PhysicsSystem() {}

PhysicsSystem::~PhysicsSystem() {}

void PhysicsSystem::CreatePhysicsBody(b2World world, std::vector<b2Vec2>& vertices) {}

void PhysicsSystem::Update()
{

	for(auto const& entity : mEntities)
	{
		Coordinator& coordinator = gCoordinator.GetInstance();
		auto& selectable		 = coordinator.GetComponent<Selectable>(entity);
		auto& polygon			 = coordinator.GetComponent<Polygon>(entity);

		auto lastPos = polygon.getPosition();
		auto newPos  = sf::Vector2f(polygon.m_body->GetPosition().x * pixelToMeterRatio,
									polygon.m_body->GetPosition().y * pixelToMeterRatio);

		auto delta = newPos - lastPos;
		selectable.AABB_lowerBounds[0] += delta.x, selectable.AABB_lowerBounds[1] += delta.y;
		selectable.AABB_upperBounds[0] += delta.x, selectable.AABB_upperBounds[1] += delta.y;
		polygon.AABB_lowerBounds[0] += delta.x, selectable.AABB_lowerBounds[1] += delta.y;
		polygon.AABB_upperBounds[0] += delta.x, selectable.AABB_upperBounds[1] += delta.y;
		polygon.setPosition(newPos);
		polygon.setRotation(polygon.m_body->GetAngle() * (180 / b2_pi));

		sf::Vector2f minBound = {FLT_MAX, FLT_MAX};
		sf::Vector2f maxBound = {-FLT_MAX, -FLT_MAX};
		for(auto& vertex : polygon.m_vertices)
		{
			sf::Vector2f pos = polygon.getTransform() * vertex.position;
			minBound.x		 = std::min(pos.x, minBound.x);
			minBound.y		 = std::min(pos.y, minBound.y);
			maxBound.x		 = std::max(pos.x, maxBound.x);
			maxBound.y		 = std::max(pos.y, maxBound.y);
		}

		polygon.m_lower.setPosition(minBound);
		polygon.m_upper.setPosition(maxBound);
		polygon.AABB_lowerBounds[0] = minBound.x, selectable.AABB_lowerBounds[1] = minBound.y;
		polygon.AABB_upperBounds[0] = maxBound.x, selectable.AABB_upperBounds[1] = maxBound.y;
		selectable.AABB_lowerBounds[0] = minBound.x, selectable.AABB_lowerBounds[1] = minBound.y;
		selectable.AABB_upperBounds[0] = maxBound.x, selectable.AABB_upperBounds[1] = maxBound.y;
	}
};

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