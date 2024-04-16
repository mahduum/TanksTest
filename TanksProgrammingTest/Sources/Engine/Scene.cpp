#include "Scene.h"
#include "Entity.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "TextureComponent.h"
#include <fstream>
#include <queue>

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

			if (NewEntity->GetName() == "Player")
			{
				SDL_Log("Setting player as flow field target");
				auto [x, y] = NewEntity->GetPositionXY();
				m_FlowFieldTargetX = x;
				m_FlowFieldTargetX = y;
			}

			AddEntity(NewEntity);
		}
	}

	nlohmann::json::const_iterator SecenLayoutIt = Config.find("SceneLayout");
	if (SecenLayoutIt != Config.end())
	{
		LoadSceneFromLayout((*SecenLayoutIt)["Content"], (*SecenLayoutIt)["Legend"]);
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
		if ((*It)->GetName() == "Projectile")
			SDL_Log("Drawing entity from scene by name: %s", (*It)->GetName().data());

		(*It)->Draw();
	}
}

void Scene::UnInitialize()
{
	for (auto It = m_Entities.begin(); It != m_ValidEntitiesEnd; ++It)
	{
		if ((*It)->GetName() == "Projectile")
			SDL_Log("Uninitializing entity from scene by name: %s", (*It)->GetName().data());

		(*It)->UnInitialize();
	}
}

void Scene::AddEntity(Entity* Entity)
{
	if(m_Entities.end() == m_ValidEntitiesEnd)
	{
		m_Entities.push_back(Entity);
		m_ValidEntitiesEnd = m_Entities.end();
		SDL_Log("Adding new entity with push back: %s", Entity->GetName().data());
	}
	else
	{
		SDL_Log("Adding new entity with replace: %s", Entity->GetName().data());
		delete *m_ValidEntitiesEnd;
		*m_ValidEntitiesEnd = Entity;
		++m_ValidEntitiesEnd;
	}
}

void Scene::RemoveEntity(Entity* Entity)//todo add removed entities to pool for reuse
{
	std::vector<::Entity*>::iterator RetIt = std::remove(
		m_Entities.begin(), m_Entities.end(), Entity);
	if(RetIt != m_Entities.end())
	{
		m_ValidEntitiesEnd = RetIt;
		(*m_ValidEntitiesEnd)->UnInitialize();
		SDL_Log("Removed entity from scene by name: %s", (*RetIt)->GetName().data());
	}
}

void Scene::LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend)
{
	int Row = 0;
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();

	auto&& Items = Content.items();
	m_FlowFieldRows = static_cast<int>(Content.size());
	const std::string_view& LineCharacters = Items.begin().value();
	m_FlowFieldColumns = static_cast<int>(std::distance(LineCharacters.begin(), LineCharacters.end()));
	m_FlowFieldCells.assign(m_FlowFieldRows * m_FlowFieldColumns, FlowFieldCell{});

	for (const auto& Item : Items)
	{
		int Column = 0;
		const std::string& Line = Item.value();
		
		for (char Character : Line)
		{
			size_t Index = Row * m_FlowFieldColumns + Column;
			SDL_Log("Current index: %d", Index);
			if (Character != ' ')
			{
				const char Key[] = { Character, '\0' };
				nlohmann::json EntitySpecs = Legend[Key];

				Entity* NewEntity = ResourceManagerPtr->CreateEntityFromDataTemplate(EntitySpecs["Type"]);

				TextureComponent* TextureComponentPtr = NewEntity->GetComponent<TextureComponent>();
				int Width = EntitySpecs["Width"];
				int Height = EntitySpecs["Height"];
				TextureComponentPtr->SetPosition(Column * Width, Row * Height);
				TextureComponentPtr->SetScale(Width, Height);

				NewEntity->SetPosition(Column * Width, Row * Height);
				NewEntity->SetRotation(FacingDirection::UP);

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
	std::queue<int> IndicesToCheck;
	//Set the target tile's flow value to 0 and add it to the queue.
	m_FlowFieldCells[IndexTarget].m_FlowDistance = 0;
	IndicesToCheck.push(IndexTarget);

	constexpr int m_NeighboursOffsets[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

	//Loop through the queue and assign distance to each tile.
	while (IndicesToCheck.empty() == false)
	{
		int IndexCurrent = IndicesToCheck.front();
		IndicesToCheck.pop();

		//Check each of the neighbors;
		for (const auto m_NeighboursOffset : m_NeighboursOffsets)
		{
			int NeighborX = m_NeighboursOffset[0] + IndexCurrent % m_FlowFieldColumns;//add offset in x to current index
			int NeighborY = m_NeighboursOffset[1] + IndexCurrent / m_FlowFieldColumns;//add offset in y to current index
			int IndexNeighbor = NeighborX + NeighborY * m_FlowFieldColumns;//compose linear index

			//Ensure that the neighbor exists and isn't a wall.
			if (IndexNeighbor > -1 && IndexNeighbor < m_FlowFieldCells.size() &&
				NeighborX > -1 && NeighborX < m_FlowFieldColumns &&
				NeighborY > -1 && NeighborY < m_FlowFieldRows &&
				m_FlowFieldCells[IndexNeighbor].m_CanBeSteppedOn)
			{

				//Check if the tile has been assigned a distance yet or not.
				if (m_FlowFieldCells[IndexNeighbor].m_FlowDistance == m_FlowDistanceMax)
				{
					//If not the set it's distance and add it to the queue.
					m_FlowFieldCells[IndexNeighbor].m_FlowDistance = m_FlowFieldCells[IndexCurrent].m_FlowDistance + 1;
					IndicesToCheck.push(IndexNeighbor);
				}
			}
		}
	}
}

void Scene::CalculateFlowDirections()
{
	constexpr int m_NeighboursOffsets[][2] = { { -1, 0}, {1, 0}, {0, -1}, {0, 1} };

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

void Scene::SetTargetAndCalculateFlowField(int x, int y)
{
	if (m_FlowFieldTargetX != x || m_FlowFieldTargetY != y) {
		m_FlowFieldTargetX = x;
		m_FlowFieldTargetY = y;

		//Ensure the target is in bounds.
		int IndexTarget = m_FlowFieldTargetX + m_FlowFieldTargetY * m_FlowFieldRows;  // NOLINT(bugprone-narrowing-conversions)
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
