#pragma once

#include <string>
#include <map>
#include <nlohmann/json.hpp>

enum class ResourceType : unsigned int
{
	Entity,
	Scene
};

class Entity;
class EntityComponent;

class ResourceManager
{

public:
	ResourceManager() = delete;
	ResourceManager(std::string Path);

	void LoadResources();
	const nlohmann::json& GetJsonConfig(std::string Name, enum ResourceType Type);
	void RegisterComponent(std::string Type, EntityComponent* Component);
	const EntityComponent* GetComponentPrototypeByName(std::string Name);
	Entity* CreateEntityFromDataTemplate(std::string Name);

private:
	void LoadResourcesFromFolder(std::string Folder, std::map<std::string, nlohmann::json>& MapContainer);
	
	std::string m_Path;
	std::map<std::string, nlohmann::json> m_Entities;
	std::map<std::string, nlohmann::json> m_Scenes;

	std::map<std::string, EntityComponent*> m_ComponentsPrototypes;//todo how to release it???
};