#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "CollisionWorld.h"
#include "Engine.h"

class EntityComponent;

class Entity
{

public:
	void LoadFromConfig(nlohmann::json Config);
	void Initialize();
	void Update(float DeltaTime);
	void Draw();
	void UnInitialize();

	void AddComponent(EntityComponent* Component);
	void RemoveComponent(EntityComponent* Component);

	std::string_view GetName() const { return { m_Name.c_str(), m_Name.size() }; }

	template <typename ComponentType>
	ComponentType* GetComponent()
	{
		static_assert(std::is_base_of_v<EntityComponent, ComponentType>, "ComponentType must be a subtype of BoxCollisionComponent");

		for (EntityComponent* Component : m_Components)
		{
			if (ComponentType* TypedComponent = dynamic_cast<ComponentType*>(Component))
			{
				return TypedComponent;
			}
		}
		return nullptr;
	}

	void UpdateWorldTransform();
	
private:
	std::list<EntityComponent*> m_Components;//todo make it unordered set
	std::string m_Name;
	
	Vector2 m_Position = {0, 0};
	bool m_UpdateWorldTransform = true;
};