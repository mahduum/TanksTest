#include "PlayerProjectileSpawnerComponent.h"

PlayerProjectileSpawnerComponent::PlayerProjectileSpawnerComponent(Entity* Owner) : ProjectileSpawnerComponent(Owner)
{
}

PlayerProjectileSpawnerComponent::PlayerProjectileSpawnerComponent() : PlayerProjectileSpawnerComponent(nullptr)
{
}
