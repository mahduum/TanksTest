#include "EntitySpawnerComponent.h"
#include "Entity.h"
#include "TextureComponent.h"
#include <SDL_log.h>

EntitySpawnerComponent::EntitySpawnerComponent(Entity* Owner) : EntityComponent(Owner),
																m_TextureComponent(nullptr),
                                                                m_CanSpawn(false),
                                                                m_SpawnRateSeconds(0),
                                                                m_TimeSinceLastSpawn(0)
{
}

EntitySpawnerComponent::EntitySpawnerComponent() : EntitySpawnerComponent(nullptr)
{
}

void EntitySpawnerComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);
	m_SpawnRateSeconds = Config.value("SpawnRateSeconds", 10.0f);
	m_CanSpawn = Config.value("AutoSpawn", false);
	m_TimeSinceLastSpawn = m_SpawnRateSeconds;
}

void EntitySpawnerComponent::Initialize()
{
	EntityComponent::Initialize();
	m_TextureComponent = GetOwner()->GetComponent<TextureComponent>().value();
}

void EntitySpawnerComponent::Update(float DeltaTime)
{
	EntityComponent::Update(DeltaTime);

	if(m_CanSpawn == false)
	{
		return;
	}

	m_TimeSinceLastSpawn += DeltaTime;

	if(m_TimeSinceLastSpawn < m_SpawnRateSeconds)
	{
		return;
	}

	DoSpawn();
	m_TimeSinceLastSpawn = 0;
}

void EntitySpawnerComponent::SetActive(bool IsActive)
{
	m_CanSpawn = IsActive;
}
