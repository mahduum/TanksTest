#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include "Entity.h"

class Entity;

class Scene
{

public:
	void LoadFromConfig(nlohmann::json Config);
	void Initialize();
	void Update(float DeltaTime);
	void Draw();
	void UnInitialize();

	void AddEntity(Entity* Entity);
	void RemoveEntity(Entity* Entity);

	template <typename ComponentType>
	std::vector<ComponentType*> GetComponents()
	{
		std::vector<ComponentType*> Components;

		for (Entity* Entity : m_Entities)
		{
			if(ComponentType* Component = Entity->GetComponent<ComponentType>())
			{
				Components.emplace_back(Component);
			}
		}

		return Components;
	}

private:
	void LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend);

	std::list<Entity*> m_Entities;
	std::string m_Name;
};