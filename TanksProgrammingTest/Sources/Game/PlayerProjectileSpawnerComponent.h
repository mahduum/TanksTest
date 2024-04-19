#pragma once
#include "ProjectileSpawnerComponent.h"
class PlayerProjectileSpawnerComponent : public ProjectileSpawnerComponent
{
public:
	PlayerProjectileSpawnerComponent(Entity* Owner);
	PlayerProjectileSpawnerComponent();

	virtual std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<PlayerProjectileSpawnerComponent>(*this); }
	virtual std::string GetNameToSpawn() const override { return "PlayerProjectile"; }
};

