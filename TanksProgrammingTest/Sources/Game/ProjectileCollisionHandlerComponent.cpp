#include "ProjectileCollisionHandlerComponent.h"

#include <SDL_log.h>

#include "ColliderComponent.h"
#include "Entity.h"
#include "Scene.h"
#include "TextureComponent.h"

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent(Entity* Owner) : DefaultCollisionHandlerComponent(Owner)
{
}

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent() : ProjectileCollisionHandlerComponent(nullptr)
{
}

void ProjectileCollisionHandlerComponent::OnCollisionImpl(const CollisionInfo& CollisionInfo)
{
	SDL_Log("Removing projectile..., collided with: %s", CollisionInfo.m_OtherCollider->GetOwner()->GetName().data());
	//Engine::Get()->GetActiveScene()->RemoveEntity(GetOwner());
	GetOwner()->IsValid = false;
}
