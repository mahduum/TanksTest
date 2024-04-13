#include "BoxColliderComponent.h"

#include "CollisionWorld.h"
#include "TextureComponent.h"

#include "Entity.h"
#include "PlayerInputComponent.h"

BoxColliderComponent::BoxColliderComponent(): BoxColliderComponent(nullptr)
{
}

BoxColliderComponent::BoxColliderComponent(Entity* Owner)
    : EntityComponent(Owner), m_TextureComponent(nullptr), m_Box(Vector2::Zero, Vector2::Zero), m_BoxOffsetMin(Vector2::Zero), m_BoxOffsetMax(Vector2::Zero)
{
}

void BoxColliderComponent::LoadFromConfig(nlohmann::json Config)
{
    m_BoxOffsetMin.x = Config.value("OffsetMinX", 0);
    m_BoxOffsetMin.y = Config.value("OffsetMinY", 0);
    m_BoxOffsetMax.x = Config.value("OffsetMaxX", 0);
    m_BoxOffsetMax.y = Config.value("OffsetMaxY", 0);
}

void BoxColliderComponent::Initialize()
{
    m_TextureComponent = GetOwner()->GetComponent<TextureComponent>();//todo make it independent of texture later on
    //for now set from texture, later on set from player, read offset from json to

    OnUpdateWorldTransform();

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
    //set new box size to new position
	EntityComponent::OnUpdateWorldTransform();//todo check collisions
    //todo for now use texture component
    auto TexRect = &m_TextureComponent->GetRectangle();
    m_Box.m_Min.Set(Vector2(TexRect->x, TexRect->y) + m_BoxOffsetMin);
    m_Box.m_Max.Set(Vector2(TexRect->x + TexRect->w, TexRect->y + TexRect->h) + m_BoxOffsetMax);
}

SDL_Rect* BoxColliderComponent::GetRectangle() const
{
    return &m_TextureComponent->GetRectangle();
}

bool BoxColliderComponent::IntersectsWith(const BoxColliderComponent& other) const//todo return delta
{
    return Intersect(m_Box, other.m_Box);
}

bool BoxColliderComponent::TryGetCollisionDelta(const BoxColliderComponent& other, Vector2& collisionDelta) const
{
    if (TryGetBoxCollisionDelta(GetBox(), other.GetBox(), collisionDelta))
    {
        return true;
    }

    return false;
}

void BoxColliderComponent::SetPosition(const Vector2 position)
{

}

void BoxColliderComponent::SetScaleOffset(const Vector2 offset)
{

}

void BoxColliderComponent::SetBoxWithOffset(const Vector2 boxMinPosition, const Vector2 boxMaxPosition, const Vector2 boxMinOffset, const Vector2 boxMaxOffset)//todo or use actor position
{
    m_Box.m_Min = boxMinPosition + boxMinOffset;
    m_Box.m_Max = boxMaxPosition + boxMaxOffset;
}

