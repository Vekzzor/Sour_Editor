#include "Coordinator.h"


Coordinator::Coordinator()
{
}


Coordinator::~Coordinator()
{
}

Coordinator & Coordinator::GetInstance()
{
	static Coordinator instance; // Guaranteed to be destroyed.
								 // Instantiated on first use.
	return instance;
}

void Coordinator::Init()
{
	// Create pointers to each manager
	mComponentManager = std::make_unique<ComponentManager>();
	mEntityManager = std::make_unique<EntityManager>();
	mSystemManager = std::make_unique<SystemManager>();
}

Entity Coordinator::CreateEntity()
{
	return mEntityManager->CreateEntity();
}

void Coordinator::DestroyEntity(Entity entity)
{
	mEntityManager->DestroyEntity(entity);

	mComponentManager->EntityDestroyed(entity);

	mSystemManager->EntityDestroyed(entity);
}

uint32_t Coordinator::getEntityCount()
{
	return mEntityManager->getEntityCount();
}
