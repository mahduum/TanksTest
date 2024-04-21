#include "ProjectileCollisionHandlerComponent.h"

#include <SDL_log.h>

#include "IColliderComponent.h"
#include "Entity.h"
#include "Scene.h"
#include "TextureComponent.h"

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent(Entity* Owner) : ICollisionHandlerComponent(Owner)
{
}

ProjectileCollisionHandlerComponent::ProjectileCollisionHandlerComponent() : ProjectileCollisionHandlerComponent(nullptr)
{
}

void ProjectileCollisionHandlerComponent::OnCollisionImpl(const std::shared_ptr<IColliderComponent>& CollisionInfo)
{
	SDL_Log("Projectile killed");//look on the owner if it has data
	GetOwner()->IsAlive = false;
}
