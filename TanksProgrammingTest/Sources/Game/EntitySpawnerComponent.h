#pragma once
#include "EntityComponent.h"
class TextureComponent;
class Vector2;

class EntitySpawnerComponent : public EntityComponent
{
public:
	EntitySpawnerComponent(Entity* Owner);
	EntitySpawnerComponent();

	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;
	void SetActive(bool IsActive);

protected:
	virtual void DoSpawn() = 0;
	virtual void SetSpawnPoint(Vector2& OutSpawnPoint) = 0;
	virtual std::string GetNameToSpawn() const = 0;

	std::shared_ptr<TextureComponent> m_TextureComponent;

	bool m_CanSpawn;
	float m_SpawnRateSeconds;
	float m_TimeSinceLastSpawn;

	std::string m_NameToSpawn;
};

