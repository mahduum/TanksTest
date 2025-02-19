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

	void AddEntity(Entity* Entity);//todo with shared
	void RemoveEntity(Entity* Entity);//todo with shared
	void RemoveDeadEntities();

	bool CanSpawnEnemy() const { return m_EnemyEntitiesCount < m_MaxEnemyEntitiesInScene; }

private:
	void LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend, nlohmann::json Parameters);

	std::vector<Entity*> m_Entities;
	std::vector<::Entity*>::iterator m_ValidEntitiesEnd;
	std::string m_Name;

	int m_EnemyEntitiesCount = 0;
	const int m_MaxEnemyEntitiesInScene = 6;

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

	int m_FlowFieldTargetX = -1;
	int m_FlowFieldTargetY = -1;

	std::vector<FlowFieldCell> m_FlowFieldCells;
	int m_CellSizeX;
	int m_CellSizeY;
	int m_FlowFieldRows;
	int m_FlowFieldColumns;

	void CalculateDistances();
	void CalculateFlowDirections();

public:

	void SetTargetAndCalculateFlowField(int x, int y);
	Vector2Int GetTargetCellScenePosition() const;

	void GetNextNavNodeLocationFromLocation(int sceneX, int sceneY, Vector2Int& nextNodeSceneLocation, Vector2Int& flowDirection, const Entity* requester = nullptr) const;
	int GetCellIndexFromScenePosition(int sceneX, int sceneY) const;
	Vector2Int GetScenePositionFromCellCoords(int cellX, int cellY) const;
	Vector2Int GetCellCoordsFromScenePosition(int sceneX, int sceneY) const;
	Vector2Int GetCellCoordsFromLinearIndex(int index) const;
	int GetFlowDistanceAtSceneCoords(int SceneX, int SceneY) const;
};