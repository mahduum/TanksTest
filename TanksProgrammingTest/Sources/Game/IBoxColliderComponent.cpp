#include "IBoxColliderComponent.h"
#include "Engine.h"
#include "Entity.h"
#include "TextureComponent.h"

IBoxColliderComponent::IBoxColliderComponent(Entity* Owner) : IColliderComponent (Owner)
{
}

IBoxColliderComponent::IBoxColliderComponent() : IBoxColliderComponent(nullptr)
{
}

bool IBoxColliderComponent::TryGetCollisionDelta(const IBoxColliderComponent& other, Vector2& collisionDelta) const
{
    if (TryGetBoxCollisionDelta(GetBox(), other.GetBox(), collisionDelta))
    {
        return true;
    }

    return false;
}

SDL_Rect* IBoxColliderComponent::GetRectangle() const
{
    return &GetOwner()->GetComponent<TextureComponent>()->GetRectangle();
}
