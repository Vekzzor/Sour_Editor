#pragma once
#include "ECS.h"

class EntityManager
{
private:
	// Queue of unused entity IDs
	std::queue<Entity> mAvailableEntities{};

	// Array of signatures where the index corresponds to the entity ID
	std::array<Signature, MAX_ENTITIES> mSignatures{};

	// Total living entities - used to keep limits on how many exist
	uint32_t mLivingEntityCount{};

public:
	EntityManager();
	~EntityManager();
	Entity CreateEntity();
	void DestroyEntity(Entity entity);

	void SetSignature(Entity entity, Signature signature);
	Signature GetSignature(Entity entity);
};
