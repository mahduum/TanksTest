#include "Scene.h"
#include "Entity.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "TextureComponent.h"
#include <fstream>

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
		//::Entity x = **RetIt;
		SDL_Log("Removed entity from scene by name: %s", (*RetIt)->GetName().data());
		//x.UnInitialize();
		//delete* RetIt;
	}
}

void Scene::LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend)
{
	int Row = 0;
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	for (auto Item : Content.items())
	{
		int Column = 0;
		const std::string& Line = Item.value();
		for (char Character : Line)
		{
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
			}
			++Column;
		}
		++Row;
	}
}
