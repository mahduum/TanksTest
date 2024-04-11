#pragma once

#include <vector>
#include <nlohmann/json.hpp>

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

	template <typename ComponentType>
	ComponentType* GetComponent()
	{
		for (EntityComponent* Component : m_Components)
		{
			if (ComponentType* TypedComponent = dynamic_cast<ComponentType*>(Component))
			{
				return TypedComponent;
			}
		}
		return nullptr;
	}
	
private:
	std::list<EntityComponent*> m_Components;
	std::string m_Name;
};