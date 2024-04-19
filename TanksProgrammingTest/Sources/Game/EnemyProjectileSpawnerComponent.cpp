#include "EnemyProjectileSpawnerComponent.h"

EnemyProjectileSpawnerComponent::EnemyProjectileSpawnerComponent(Entity* Owner) : ProjectileSpawnerComponent(Owner)
{
}

EnemyProjectileSpawnerComponent::EnemyProjectileSpawnerComponent() : EnemyProjectileSpawnerComponent(nullptr)
{
}
