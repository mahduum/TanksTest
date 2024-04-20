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

void ProjectileCollisionHandlerComponent::OnCollisionImpl(const std::shared_ptr<ColliderComponent>& CollisionInfo)
{
	SDL_Log("Projectile killed");
	GetOwner()->IsAlive = false;
}
