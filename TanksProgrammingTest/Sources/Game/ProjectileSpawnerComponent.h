#pragma once
#include "EntityComponent.h"
#include "Engine.h"
#include "EntitySpawnerComponent.h"
class TextureComponent;

class ProjectileSpawnerComponent :public EntitySpawnerComponent
{

public:
	ProjectileSpawnerComponent(Entity* Owner);
	ProjectileSpawnerComponent();

	virtual void DoSpawn() override;
	virtual void SetSpawnPoint(Vector2& OutSpawnPoint) override;
	
private:
	void SpawnProjectile();
	void GetTopSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetBottomSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetRightSpawnPoint(Vector2& OutSpawnPoint) const;
	void GetLeftSpawnPoint(Vector2& OutSpawnPoint) const;
};

