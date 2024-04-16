#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include "Entity.h"

class Entity;

class Scene
{

public:
	Scene();
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

	std::vector<Entity*> m_Entities;
	std::vector<::Entity*>::iterator m_ValidEntitiesEnd;
	std::string m_Name;

private:
	/*Flow field section.*/
	static constexpr unsigned char m_FlowDistanceMax = 255;

	struct FlowFieldCell
	{
		bool m_CanBeSteppedOn = false;
		int m_FlowDirectionX = 0;
		int m_FlowDirectionY = 0;
		unsigned char m_FlowDistance = m_FlowDistanceMax;
	};

	std::vector<FlowFieldCell> m_FlowFieldCells;
	int m_FlowFieldRows;
	int m_FlowFieldColumns;
	int m_FlowFieldTargetX;
	int m_FlowFieldTargetY;

public:

	void CalculateDistances();
	void CalculateFlowDirections();
	void SetTargetAndCalculateFlowField(int x, int y);
};