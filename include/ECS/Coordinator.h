#pragma once
#include "ComponentManager.h"
#include "ECS.h"
#include "EntityManager.h"
#include "SystemManager.h"

class Coordinator
{
private:
	std::unique_ptr<ComponentManager> mComponentManager;
	std::unique_ptr<EntityManager> mEntityManager;
	std::unique_ptr<SystemManager> mSystemManager;

public:
	Coordinator();
	~Coordinator();

	static Coordinator& GetInstance();

	void Init();

	// Entity methods
	Entity CreateEntity();

	void DestroyEntity(Entity entity);

	uint32_t getEntityCount();

	// Component methods
	template <typename T>
	void RegisterComponent()
	{
		mComponentManager->RegisterComponent<T>();
	}

	template <typename T>
	void AddComponent(Entity entity, T component)
	{
		mComponentManager->AddComponent<T>(entity, component);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), true);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}
	template <typename T>
	bool HasComponent(Entity entity)
	{
		return mComponentManager->HasComponent<T>(entity);
	}

	void getComponentNames(std::vector<const char*>& _vec)
	{
		mComponentManager->getComponentNames(_vec);
	}
	
	template <typename T>
	void RemoveComponent(Entity entity)
	{
		mComponentManager->RemoveComponent<T>(entity);

		auto signature = mEntityManager->GetSignature(entity);
		signature.set(mComponentManager->GetComponentType<T>(), false);
		mEntityManager->SetSignature(entity, signature);

		mSystemManager->EntitySignatureChanged(entity, signature);
	}

	template <typename T>
	T& GetComponent(Entity entity)
	{
		return mComponentManager->GetComponent<T>(entity);
	}

	template <typename T>
	ComponentType GetComponentType()
	{
		return mComponentManager->GetComponentType<T>();
	}

	// System methods
	template <typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		return mSystemManager->RegisterSystem<T>();
	}

	template <typename T>
	void SetSystemSignature(Signature signature)
	{
		mSystemManager->SetSignature<T>(signature);
	}

	Coordinator(Coordinator const&) = delete;
	void operator=(Coordinator const&) = delete;
};
