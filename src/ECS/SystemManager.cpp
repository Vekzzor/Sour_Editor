#include "SystemManager.h"


SystemManager::SystemManager()
{
}


SystemManager::~SystemManager()
{
}

void SystemManager::EntityDestroyed(Entity entity)
{
	// Erase a destroyed entity from all system lists
	// mEntities is a set so no check needed
	for (auto const& pair : mSystems)
	{
		auto const& system = pair.second;

		system->entities_.erase(entity);
	}
}

void SystemManager::EntitySignatureChanged(Entity entity, Signature entitySignature)
{
	// Notify each system that an entity's signature changed
	for (auto const& pair : mSystems)
	{
		auto const& type = pair.first;
		auto const& system = pair.second;
		auto const& systemSignature = mSignatures[type];

		// Entity signature matches system signature - insert into set
		if ((entitySignature & systemSignature) == systemSignature)
		{
			system->entities_.insert(entity);
		}
		// Entity signature does not match system signature - erase from set
		else
		{
			system->entities_.erase(entity);
		}
	}
}