#pragma once
#include "EntityComponent.h"
#include "Engine.h"
class TextureComponent;

class ProjectileSpawnerComponent :public EntityComponent
{

public:

	ProjectileSpawnerComponent(Entity* Owner);
	ProjectileSpawnerComponent();

	virtual EntityComponent* Clone() const override { return new ProjectileSpawnerComponent(*this); }

	void LoadFromConfig(nlohmann::json Config) override;//offsets for spawn points
	void Initialize() override;
	void Update(float DeltaTime) override;
	void UnInitialize() override;
	void OnUpdateWorldTransform() override;

	void SpawnProjectile();
	bool m_Spawn = false;
private:

	void GetTopSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetBottomSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetRightSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetLeftSpawnPoint(Vector2& OutSpawnPoint) const;

	TextureComponent* m_TextureComponent;
};

