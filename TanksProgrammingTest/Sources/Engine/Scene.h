#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

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

private:
	void LoadSceneFromLayout(nlohmann::json Content, nlohmann::json Legend);

	std::list<Entity*> m_Entities;
	std::string m_Name;
};