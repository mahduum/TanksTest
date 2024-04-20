#include "DefaultCollisionHandlerComponent.h"

#include <SDL_log.h>

#include "Entity.h"

DefaultCollisionHandlerComponent::DefaultCollisionHandlerComponent(Entity* Owner) : ICollisionHandlerComponent(Owner)
{
}

DefaultCollisionHandlerComponent::DefaultCollisionHandlerComponent() : DefaultCollisionHandlerComponent(nullptr)
{
}

void DefaultCollisionHandlerComponent::OnCollisionImpl(const std::shared_ptr<ColliderComponent>& collisionInfo)
{
}
