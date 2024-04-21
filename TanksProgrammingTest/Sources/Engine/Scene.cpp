#include "Scene.h"
#include "Entity.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "TextureComponent.h"
#include <fstream>
#include <queue>

#include "../Game/BoxColliderComponent.h"
#include "../Game/BoxTweenSweepColliderComponent.h"	

#include "../Game/EnemyTankMovementComponent.h"

constexpr int m_NeighboursOffsets[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

Scene::Scene()
{
	m_ValidEntitiesEnd = m_Entities.end();
}

void Scene::LoadFromConfig(nlohmann::json Config)
{
	m_Name = Config.value("Name", "");

	if (Config.find("Entities") != Config.end())
	{
		ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
		for (auto Item : Config["Entities"].items())//entities that are not fixed in layout
		{
			Entity* NewEntity = new Entity();//todo what type of entity...

			nlohmann::json EntityConfig = Item.value();
			std::string TypeName = EntityConfig.value("Type", "");
			if (!TypeName.empty())
			{
				nlohmann::json EntityTemplateConfig = ResourceManagerPtr->GetJsonConfig(TypeName, ResourceType::Entity);
				NewEntity->LoadFromConfig(EntityTemplateConfig);
			}
			else
			{
				NewEntity->LoadFromConfig(Item.value());
			}

			AddEntity(NewEntity);
		}
	}

	nlohmann::json::const_iterator SceneLayoutIt = Config.find("SceneLayout");
	if (SceneLayoutIt != Config.end())
	{
		LoadSceneFromLayout((*SceneLayoutIt)["Content"], (*SceneLayoutIt)["Legend"], (*SceneLayoutIt)["Parameters"]);
	}
}

void Scene::Initialize()
{
	for (Entity* Entity : m_Entities)
	{
		Entity->Initialize();
	}
}

void Scene::Update(float DeltaTime)
{
	for (auto It = m_Entities.begin(); It != m_ValidEntitiesEnd; ++It)
	{
		(*It)->Update(DeltaTime);
	}
}

void Scene::Draw()
{
	for (auto It = m_Entities.begin(); It != m_ValidEntitiesEnd; ++It)
	{
		(*It)->Draw();
	}
}

void Scene::UnInitialize()
{
	for (auto It = m_Entities.begin(); It != m_ValidEntitiesEnd; ++It)
	{
		(*It)->UnInitialize();
	}
}

void Scene::AddEntity(Entity* Entity)
{
	if(Entity->GetComponent<EnemyTankMovementComponent>().has_value())
	{
		++m_EnemyEntitiesCount;//todo
	}

	if(m_Entities.end() == m_ValidEntitiesEnd)
	{
		m_Entities.emplace_back(Entity);
		m_ValidEntitiesEnd = m_Entities.end();
	}
	else
	{
		*m_ValidEntitiesEnd = Entity;
		++m_ValidEntitiesEnd;
	}
}

void Scene::RemoveEntity(Entity* Entity)
{
	if (Entity->GetComponent<EnemyTankMovementComponent>().has_value())
	{
		--m_EnemyEntitiesCount;
	}

	std::vector<::Entity*>::iterator RetIt = std::find(
		m_Entities.begin(), m_Entities.end(), Entity);
	if (RetIt != m_Entities.end())
	{
		(*RetIt)->UnInitialize();
		std::iter_swap(RetIt, m_Entities.end() - 1);
		m_Entities.pop_back();
		m_ValidEntitiesEnd = m_Entities.end();
	}
}

void Scene::RemoveDeadEntities()
{
	m_Entities.erase(std::remove_if(m_Entities.begin(), m_ValidEntitiesEnd, [&](Entity* e)
		{
			if(e->IsAlive == false)
			{
				e->UnInitialize();
				return true;
			}
			return false;
		}), m_ValidEntitiesEnd);
	m_ValidEntitiesEnd = m_Entities.end();
}

void Scene::LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend, nlohmann::json Parameters)
{
	int Row = 0;
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();

	m_CellSizeX = Parameters.value("CellWidth", 30);
	m_CellSizeY = Parameters.value("CellHeight", 35);

	auto&& Items = Content.items();
	m_FlowFieldRows = static_cast<int>(Content.size());
	const std::string_view& LineCharacters = Items.begin().value();

	//setup flow field
	m_FlowFieldColumns = static_cast<int>(std::distance(LineCharacters.begin(), LineCharacters.end()));
	m_FlowFieldCells.assign(m_FlowFieldRows * m_FlowFieldColumns, FlowFieldCell{});

	for (const auto& Item : Items)
	{
		int Column = 0;
		const std::string& Line = Item.value();
		for (char Character : Line)
		{
			size_t Index = Row * m_FlowFieldColumns + Column;
			auto [x, y] = GetCellCoordsFromLinearIndex(Index);

			if (Character != ' ')
			{
				const char Key[] = { Character, '\0' };
				nlohmann::json EntitySpecs = Legend[Key];

				Entity* NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate(EntitySpecs["Type"]);
				int Width = EntitySpecs["Width"];
				int Height = EntitySpecs["Height"];

				auto TexCompOpt = NewEntity->GetComponent<TextureComponent>();
				if(TexCompOpt.has_value())
				{
					TexCompOpt.value()->SetPosition(Column * Width, Row * Height);
					TexCompOpt.value()->SetScale(Width, Height);
				}

				NewEntity->SetPosition(Column * Width, Row * Height);
				NewEntity->SetFacingDirection(FacingDirection::Up);

				AddEntity(NewEntity);
				m_FlowFieldCells.at(Index).m_CanBeSteppedOn = NewEntity->CanBeSteppedOn();
			}
			else
			{
				m_FlowFieldCells.at(Index).m_CanBeSteppedOn = true;
			}
			++Column;
		}
		++Row;
	}

	SDL_Log("Flow field columns: %d", m_FlowFieldColumns);
}

void Scene::CalculateDistances()
{
	int IndexTarget = m_FlowFieldTargetX + m_FlowFieldTargetY * m_FlowFieldColumns;

	//Create a queue that will contain the indices to be checked.
	std::queue<int> IndicesToEvaluate;
	//Set the target tile's flow value to 0 and add it to the queue.
	m_FlowFieldCells[IndexTarget].m_FlowDistance = 0;
	IndicesToEvaluate.push(IndexTarget);

	//constexpr int m_NeighboursOffsets[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

	//Loop through the queue and assign distance to each tile.
	while (IndicesToEvaluate.empty() == false)
	{
		int IndexCurrent = IndicesToEvaluate.front();
		IndicesToEvaluate.pop();

		//Check each of the neighbors;
		for (const auto NeighbourOffsets : m_NeighboursOffsets)
		{
			int NeighborX = NeighbourOffsets[0] + IndexCurrent % m_FlowFieldColumns;//add offset in x to current index
			int NeighborY = NeighbourOffsets[1] + IndexCurrent / m_FlowFieldColumns;//add offset in y to current index
			int NeighbourIndex = NeighborX + NeighborY * m_FlowFieldColumns;//compose linear index

			//Ensure that the neighbor exists and isn't a wall.
			if (NeighbourIndex > -1 && NeighbourIndex < m_FlowFieldCells.size() &&
				NeighborX > -1 && NeighborX < m_FlowFieldColumns &&
				NeighborY > -1 && NeighborY < m_FlowFieldRows &&
				m_FlowFieldCells[NeighbourIndex].m_CanBeSteppedOn)
			{
				//Check if the tile has been assigned a distance yet or not.
				if (m_FlowFieldCells[NeighbourIndex].m_FlowDistance == m_FlowDistanceMax)
				{
					//If not the set it's distance and add it to the queue.
					m_FlowFieldCells[NeighbourIndex].m_FlowDistance = m_FlowFieldCells[IndexCurrent].m_FlowDistance + 1;
					IndicesToEvaluate.push(NeighbourIndex);
				}
			}
		}
	}
}

void Scene::CalculateFlowDirections()
{
	//constexpr int m_NeighboursOffsets[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

	for (int CellIndex = 0; CellIndex < m_FlowFieldCells.size(); CellIndex++)
	{
		//Ensure that the tile has been assigned a distance value.
		if (m_FlowFieldCells[CellIndex].m_FlowDistance != m_FlowDistanceMax)
		{
			//Set the best distance to the current tile's distance.
			unsigned char FlowFieldBest = m_FlowFieldCells[CellIndex].m_FlowDistance;

			//Check each of the neighbors;
			for (const auto m_NeighboursOffset : m_NeighboursOffsets)
			{
				int OffsetX = m_NeighboursOffset[0];
				int OffsetY = m_NeighboursOffset[1];

				int NeighborX = OffsetX + CellIndex % m_FlowFieldColumns;
				int NeighborY = OffsetY + CellIndex / m_FlowFieldColumns;
				int NeighbourIndex = NeighborX + NeighborY * m_FlowFieldColumns;

				//Ensure that the neighbor exists.
				if (NeighbourIndex > -1 && NeighbourIndex < m_FlowFieldCells.size() &&
					NeighborX > -1 && NeighborX < m_FlowFieldColumns &&
					NeighborY > -1 && NeighborY < m_FlowFieldRows)
				{
					//If the current neighbor's distance is lower than the best then use it.
					if (m_FlowFieldCells[NeighbourIndex].m_FlowDistance < FlowFieldBest)
					{
						FlowFieldBest = m_FlowFieldCells[NeighbourIndex].m_FlowDistance;
						m_FlowFieldCells[CellIndex].m_FlowDirectionX = OffsetX;
						m_FlowFieldCells[CellIndex].m_FlowDirectionY = OffsetY;
					}
				}
			}
		}
	}
}

void Scene::SetTargetAndCalculateFlowField(int sceneX, int sceneY)
{
	auto [x, y] = GetCellCoordsFromScenePosition(sceneX, sceneY);

	if (m_FlowFieldTargetX != x || m_FlowFieldTargetY != y)
	{
		m_FlowFieldTargetX = x;
		m_FlowFieldTargetY = y;

		//Ensure the target is in bounds.
		int IndexTarget = m_FlowFieldTargetX + m_FlowFieldTargetY * m_FlowFieldColumns;  // NOLINT(bugprone-narrowing-conversions)
		if (IndexTarget > -1 && IndexTarget < m_FlowFieldCells.size() &&
			m_FlowFieldTargetX > -1 && m_FlowFieldTargetX < m_FlowFieldColumns &&
			m_FlowFieldTargetY > -1 && m_FlowFieldTargetY < m_FlowFieldRows)
		{

			//Reset the tile flow data.
			for (auto& Cell : m_FlowFieldCells)
			{
				Cell.m_FlowDirectionX = 0;
				Cell.m_FlowDirectionY = 0;
				Cell.m_FlowDistance = m_FlowDistanceMax;
			}

			//Calculate the flow field.
			CalculateDistances();
			CalculateFlowDirections();
		}
	}
}

Vector2 Scene::GetTargetCellScenePosition() const
{
	auto [x, y] = GetScenePositionFromCellCoords(m_FlowFieldTargetX, m_FlowFieldTargetY);
	return Vector2 { static_cast<float>(x), static_cast<float>(y) };
}

void Scene::GetNextNavNodeLocationFromLocation(int sceneX, int sceneY, Vector2& nextNodeSceneLocation, Vector2& flowDirection, const Entity* requester)//todo on uninitialize release block grid point
{
	int CellIndex = GetCellIndexFromScenePosition(sceneX, sceneY);

	if (CellIndex > -1 && CellIndex < m_FlowFieldCells.size() &&
		m_FlowFieldTargetX > -1 && m_FlowFieldTargetX < m_FlowFieldColumns &&
		m_FlowFieldTargetY > -1 && m_FlowFieldTargetY < m_FlowFieldRows)
	{
		//todo return next coords
		auto& CellData = m_FlowFieldCells.at(CellIndex);
		std::tuple<int, int> FlowDirection = { CellData.m_FlowDirectionX, CellData.m_FlowDirectionY };

		auto [CurrentCellX, CurrentCellY] = GetCellCoordsFromLinearIndex(CellIndex);
		auto [GoToNeighbourSceneX, GoToNeighbourSceneY] = GetScenePositionFromCellCoords(CurrentCellX + CellData.m_FlowDirectionX, CurrentCellY + CellData.m_FlowDirectionY);

		nextNodeSceneLocation.Set(GoToNeighbourSceneX, GoToNeighbourSceneY);
		flowDirection.Set(CellData.m_FlowDirectionX, CellData.m_FlowDirectionY);

		/*get requester key and set cell to be one max value, I enter the index of the cell I blocked when I leave, and when I enter I release the previous cell*/
		if (requester != nullptr)
		{
			//constexpr int m_NeighboursOffsets[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

			//recalculate offsets
			CellData.m_FlowDistance = m_FlowDistanceMax;//no is not walkable
			std::queue<int> NeighboursToReevaluate;

			//get cells around, find best one, set its own value to +1 and direction to this cell
			for (auto NeighbourOffsets : m_NeighboursOffsets)
			{
				int NeighborX = NeighbourOffsets[0] + CellIndex % m_FlowFieldColumns;//add offset in x to current index
				int NeighborY = NeighbourOffsets[1] + CellIndex / m_FlowFieldColumns;//add offset in y to current index
				int NeighbourIndex = NeighborX + NeighborY * m_FlowFieldColumns;//compose linear index
				//only if it finds value greater than its own, first try to find smaller value and point to it, if it cannot find smaller value, must rebuild two levels
				NeighboursToReevaluate.push(NeighbourIndex);
			}

			while (NeighboursToReevaluate.empty() == false)
			{
				int IndexCurrent = NeighboursToReevaluate.front();
				//IndicesToEvaluate.pop();

			}
		}
	}
	else
	{
		SDL_LogError(0, "Requested coordinates (%d, %d) are outside flow field!", sceneX, sceneY);
	}
}

//todo
void Scene::ReCalculateDistancesInZone(int StartIndex, int Depth)
{
	StartIndex = m_FlowFieldTargetX + m_FlowFieldTargetY * m_FlowFieldColumns;

	//Create a queue that will contain the indices to be checked.
	std::queue<int> IndicesToEvaluate;
	//Set the target tile's flow value to 0 and add it to the queue.
	m_FlowFieldCells[StartIndex].m_FlowDistance = 0;
	IndicesToEvaluate.push(StartIndex);

	//constexpr int m_NeighboursOffsets[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

	//Loop through the queue and assign distance to each tile.
	while (IndicesToEvaluate.empty() == false)
	{
		int IndexCurrent = IndicesToEvaluate.front();
		IndicesToEvaluate.pop();

		auto& RedirectFrom = m_FlowFieldCells[IndexCurrent];

		std::vector<std::tuple<int, std::tuple<int, int>>> IndicesAndDirectionsToNeighbour;

		//Check each of the neighbors;
		for (const auto NeighbourOffsets : m_NeighboursOffsets)
		{
			int NeighborX = NeighbourOffsets[0] + IndexCurrent % m_FlowFieldColumns;//add offset in x to current index//tddo put in a method
			int NeighborY = NeighbourOffsets[1] + IndexCurrent / m_FlowFieldColumns;//add offset in y to current index
			int NeighbourIndex = NeighborX + NeighborY * m_FlowFieldColumns;//compose linear index

			//Ensure that the neighbor exists and isn't a wall.
			if (NeighbourIndex > -1 && NeighbourIndex < m_FlowFieldCells.size() &&
				NeighborX > -1 && NeighborX < m_FlowFieldColumns &&
				NeighborY > -1 && NeighborY < m_FlowFieldRows &&
				m_FlowFieldCells[NeighbourIndex].m_CanBeSteppedOn)
			{
				IndicesAndDirectionsToNeighbour.emplace_back(NeighbourIndex, std::make_tuple(NeighbourOffsets[0], NeighbourOffsets[1]));
				//if there is a lower value available simply point to it,
				//else test or equal value
				//todo set to max and 
				//Check if the tile has been assigned a distance yet or not.
				//if (m_FlowFieldCells[NeighbourIndex].m_FlowDistance == m_FlowDistanceMax)
				//{
				//	//If not the set it's distance and add it to the queue.
				//	m_FlowFieldCells[NeighbourIndex].m_FlowDistance = m_FlowFieldCells[IndexCurrent].m_FlowDistance + 1;
				//	IndicesToEvaluate.push(NeighbourIndex);
			}
		}

		if(IndicesAndDirectionsToNeighbour.empty())
		{
			return;
		}

		std::ranges::sort(IndicesAndDirectionsToNeighbour, [&](const std::tuple<int, std::tuple<int, int>>& A, const std::tuple<int, std::tuple<int, int>>& B)
			{
					return m_FlowFieldCells[std::get<0>(A)].m_FlowDistance < m_FlowFieldCells[std::get<0>(B)].m_FlowDistance;
			});

		const auto& NeighbourData = IndicesAndDirectionsToNeighbour.at(0);
		
		RedirectFrom.m_FlowDirectionX = std::get<0>(std::get<1>(NeighbourData));
		RedirectFrom.m_FlowDirectionY = std::get<1>(std::get<1>(NeighbourData));

		//if not false we found cell that itself does not need redirecting
		if (m_FlowFieldCells[std::get<0>(NeighbourData)].m_FlowDistance < RedirectFrom.m_FlowDistance == false)
		{
			IndicesToEvaluate.push(std::get<0>(NeighbourData));
		}
	}
}

int Scene::GetCellIndexFromScenePosition(int sceneX, int sceneY) const
{
	auto [Column, Row] = GetCellCoordsFromScenePosition(sceneX, sceneY);//column and row in grid space
	//auto LinearIndex = Row * m_c
	auto [VerifyX, VerifyY] = GetCellCoordsFromLinearIndex(Column + Row * m_FlowFieldColumns);

	if(Column != VerifyX)
	{
		SDL_Log("Column is not verified, was: %d, is: %d,from scene space: %d, flow field columns: %d", Column, VerifyX, sceneX, m_FlowFieldColumns);
	}

	if (Row != VerifyY)
	{
		SDL_Log("Row is not verified, was: %d, is: %d, from scene space: %d", Row, VerifyY, sceneX);
	}

	return Column + Row * m_FlowFieldColumns;
}

std::tuple<int, int> Scene::GetScenePositionFromCellCoords(int cellX, int cellY) const
{
	return { cellX * m_CellSizeX, cellY * m_CellSizeY };
}

std::tuple<int, int> Scene::GetCellCoordsFromScenePosition(int sceneX, int sceneY) const
{
	return { (sceneX / m_CellSizeX), (sceneY / m_CellSizeY) };//todo if max pos is, get max pos of collider
}

std::tuple<int, int> Scene::GetCellCoordsFromLinearIndex(int index) const
{
	return { index % m_FlowFieldColumns, index / m_FlowFieldColumns };

}
