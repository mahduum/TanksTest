#include "DefaultCollisionHandlerComponent.h"

#include <SDL_log.h>

#include "Entity.h"

DefaultCollisionHandlerComponent::DefaultCollisionHandlerComponent(Entity* Owner) : ICollisionHandlerComponent(Owner)
{
}

DefaultCollisionHandlerComponent::DefaultCollisionHandlerComponent() : DefaultCollisionHandlerComponent(nullptr)
{
}

void DefaultCollisionHandlerComponent::OnCollision(const CollisionInfo& collisionInfo)
{
	SDL_Log("Default collision handler received collision with entity: %s", collisionInfo.m_OtherEntity->GetName().data());
}
