#pragma once
#include "EntityComponent.h"
#include "Engine.h"
class TextureComponent;

class ProjectileSpawnerComponent :public EntityComponent
{

public:

	ProjectileSpawnerComponent(Entity* Owner);
	ProjectileSpawnerComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<ProjectileSpawnerComponent>(*this); }

	void LoadFromConfig(nlohmann::json Config) override;//offsets for spawn points
	void Initialize() override;
	void Update(float DeltaTime) override;
	void UnInitialize() override;

	void SpawnProjectile();
private:

	void GetTopSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetBottomSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetRightSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetLeftSpawnPoint(Vector2& OutSpawnPoint) const;

	std::shared_ptr<TextureComponent> m_TextureComponent;
};

