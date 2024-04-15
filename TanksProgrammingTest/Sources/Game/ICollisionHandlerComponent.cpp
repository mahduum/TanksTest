#include "ICollisionHandlerComponent.h"

ICollisionHandlerComponent::ICollisionHandlerComponent(Entity* Owner) : EntityComponent(Owner)
{
}

ICollisionHandlerComponent::ICollisionHandlerComponent() : ICollisionHandlerComponent(nullptr)
{
}
