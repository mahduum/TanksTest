#include "ResourceManager.h"
#include "EngineUtils.h"
#include "Entity.h"
#include "EntityComponent.h"
#include <filesystem>
#include <fstream>
#include <regex>

ResourceManager::ResourceManager(std::string Path)
	: m_Path(Path)
{
}

void ResourceManager::LoadResources()
{
	LoadResourcesFromFolder("Entities", m_Entities);
	LoadResourcesFromFolder("Scenes", m_Scenes);
}

const nlohmann::json& ResourceManager::GetJsonConfig(std::string Name, enum ResourceType Type)
{
	switch (Type)
	{
	case ResourceType::Entity:
		return m_Entities[Name];
	case ResourceType::Scene:
		return m_Scenes[Name];
	}

	static nlohmann::json DefaultJsonConfig;
	return DefaultJsonConfig;
}

void ResourceManager::RegisterComponent(std::string Type, EntityComponent* Component)
{
	m_ComponentsPrototypes[Type] = Component;
}

const EntityComponent* ResourceManager::GetComponentPrototypeByName(std::string Name)
{
	std::map<std::string, EntityComponent*>::iterator ComponentPrototypeIt = m_ComponentsPrototypes.find(Name);
	if (ComponentPrototypeIt != m_ComponentsPrototypes.end())
	{
		return ComponentPrototypeIt->second;
	}

	return nullptr;
}

Entity* ResourceManager::CreateEntityFromDataTemplate(std::string Name)
{
	std::map<std::string, nlohmann::json>::iterator EntityDataTemplateIt = m_Entities.find(Name);
	if (EntityDataTemplateIt != m_Entities.end())
	{
		Entity* NewEntity = new Entity();
		NewEntity->LoadFromConfig(EntityDataTemplateIt->second);
		return NewEntity;
	}

	return nullptr;
}

void ResourceManager::LoadResourcesFromFolder(std::string Folder, std::map<std::string, nlohmann::json>& MapContainer)
{
	std::string FolderPath = m_Path + "/" + Folder;
	for (auto Entry : std::filesystem::directory_iterator(FolderPath))
	{
		std::ifstream InFile(Entry.path());
		nlohmann::json JsonFile;
		InFile >> JsonFile;

		if (!JsonFile.is_null())
		{
			std::wstring WStrFileName = Entry.path().stem();
			std::string FileName = EngineUtils::WstringToString(WStrFileName);
			MapContainer.insert({ FileName, JsonFile });
		}

		InFile.close();
	}
}
