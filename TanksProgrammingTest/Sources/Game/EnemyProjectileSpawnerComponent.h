#pragma once
#include "ProjectileSpawnerComponent.h"
class EnemyProjectileSpawnerComponent : public ProjectileSpawnerComponent
{
public:
	EnemyProjectileSpawnerComponent(Entity* Owner);
	EnemyProjectileSpawnerComponent();

	virtual std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<EnemyProjectileSpawnerComponent>(*this); }
	virtual std::string GetNameToSpawn() const override { return "EnemyProjectile"; }
};

