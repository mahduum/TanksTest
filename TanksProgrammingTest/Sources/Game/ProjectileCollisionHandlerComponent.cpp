#include "ProjectileCollisionHandlerComponent.h"

#include <SDL_log.h>
#include "Entity.h"
#include "Scene.h"
#include "TextureComponent.h"

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent(Entity* Owner) : DefaultCollisionHandlerComponent(Owner)
{
}

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent() : ProjectileCollisionHandlerComponent(nullptr)
{
}

void ProjectileCollisionHandlerComponent::OnCollision(const CollisionInfo& collisionInfo)
{
	//SDL_Log("Projectile collision handler received collision with entity: %s", collisionInfo.m_OtherEntity->GetName().data());
	Engine::Get()->GetActiveScene()->RemoveEntity(GetOwner());
	//GetOwner()->UnInitialize();
	//delete GetOwner();
}
