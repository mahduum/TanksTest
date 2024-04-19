#pragma once
#include "EntitySpawnerComponent.h"

class EnemyTankSpawnerComponent : public EntitySpawnerComponent
{
public:

	EnemyTankSpawnerComponent(Entity* Owner);
	EnemyTankSpawnerComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<EnemyTankSpawnerComponent>(*this); }

	virtual void DoSpawn() override;
	virtual void SetSpawnPoint(Vector2& OutSpawnPoint) override;
	virtual std::string GetNameToSpawn() const override { return "EnemyTank"; }
};

