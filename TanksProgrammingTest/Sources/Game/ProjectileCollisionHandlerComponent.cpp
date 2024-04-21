#include "ProjectileCollisionHandlerComponent.h"

#include <SDL_log.h>

#include "AttributeComponent.h"
#include "DamageConfigComponent.h"
#include "IColliderComponent.h"
#include "Entity.h"

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent(Entity* Owner) : ICollisionHandlerComponent(Owner)
{
}

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent() : ProjectileCollisionHandlerComponent(nullptr)
{
}

void ProjectileCollisionHandlerComponent::OnCollisionImpl(const std::shared_ptr<IColliderComponent>& CollisionInfo)
{
	auto AttributeComponentOpt = CollisionInfo->GetOwner()->GetComponent<AttributeComponent>();
	auto DamageConfig = GetOwner()->GetComponent<DamageConfigComponent>();
	if (AttributeComponentOpt.has_value() && DamageConfig.has_value())
	{
		AttributeComponentOpt.value()->ApplyHealthChange(-DamageConfig.value()->GetDamageAmount());
	}
	GetOwner()->IsAlive = false;
}
