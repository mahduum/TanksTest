#include "IBoxColliderComponent.h"
#include "Entity.h"

IBoxColliderComponent::IBoxColliderComponent(Entity* Owner) : IColliderComponent (Owner)
{
}

IBoxColliderComponent::IBoxColliderComponent() : IBoxColliderComponent(nullptr)
{
}
