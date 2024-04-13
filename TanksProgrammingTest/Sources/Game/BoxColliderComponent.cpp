#include "BoxColliderComponent.h"

#include "CollisionWorld.h"
#include "TextureComponent.h"

#include "Entity.h"

BoxColliderComponent::BoxColliderComponent(): BoxColliderComponent(nullptr)
{
}

BoxColliderComponent::BoxColliderComponent(Entity* Owner)
    : EntityComponent(Owner), m_TextureComponent(nullptr)
{
}

void BoxColliderComponent::Initialize()
{
    m_TextureComponent = GetOwner()->GetComponent<TextureComponent>();//todo make it independent of texture later on
    Engine::Get()->GetCollisionWorld()->AddBox(this);
}

void BoxColliderComponent::Update(float DeltaTime)
{
    EntityComponent::Update(DeltaTime);
}

void BoxColliderComponent::UnInitialize()
{
    Engine::Get()->GetCollisionWorld()->RemoveBox(this);
}

void BoxColliderComponent::OnUpdateWorldTransform()
{
	EntityComponent::OnUpdateWorldTransform();//todo check collisions
}

SDL_Rect* BoxColliderComponent::GetRectangle() const
{
    return &m_TextureComponent->GetRectangle();
}

bool BoxColliderComponent::Overlaps(const BoxColliderComponent& Other) const//todo return delta
{
    return SDL_HasIntersection(GetRectangle(), Other.GetRectangle());
}

