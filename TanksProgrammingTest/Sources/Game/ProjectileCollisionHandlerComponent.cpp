#include "ProjectileCollisionHandlerComponent.h"

#include <SDL_log.h>
#include "Entity.h"
#include "Scene.h"

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent(Entity* Owner) : DefaultCollisionHandlerComponent(Owner)
{
}

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent() : ProjectileCollisionHandlerComponent(nullptr)
{
}

void ProjectileCollisionHandlerComponent::OnCollision(CollisionInfo collisionInfo)
{
	SDL_Log("Projectile collision handler received collision with entity: %s", collisionInfo.m_OhterEntity->GetName().data());
	Engine::Get()->GetActiveScene()->RemoveEntity(GetOwner());
	//GetOwner()->UnInitialize();
	//delete GetOwner();
}
