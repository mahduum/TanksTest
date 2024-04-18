#include "BoxColliderComponent.h"

#include "CollisionWorld.h"
#include "TextureComponent.h"

#include "Entity.h"

BoxColliderComponent:: BoxColliderComponent(): BoxColliderComponent(nullptr)
{
}

BoxColliderComponent::BoxColliderComponent(Entity* Owner) :
	ColliderComponent(Owner),
	m_TextureComponent(nullptr),
	m_Box(Vector2::Zero, Vector2::Zero),
	m_PreviousFrameBox(Vector2::Zero, Vector2::Zero),
	m_BoxOffsetMin(Vector2::Zero),
	m_BoxOffsetMax(Vector2::Zero)
{
}

void BoxColliderComponent::LoadFromConfig(nlohmann::json Config)
{
    ColliderComponent::LoadFromConfig(Config);

    m_BoxOffsetMin.x = Config.value("OffsetMinX", 0);
    m_BoxOffsetMin.y = Config.value("OffsetMinY", 0);
    m_BoxOffsetMax.x = Config.value("OffsetMaxX", 0);
    m_BoxOffsetMax.y = Config.value("OffsetMaxY", 0);

	//m_BoxOffsetMin.x = Config.value("OffsetMinY", 0);
 //   m_BoxOffsetMin.y = Config.value("OffsetMinX", 0);
 //   m_BoxOffsetMax.x = Config.value("OffsetMaxY", 0);
 //   m_BoxOffsetMax.y = Config.value("OffsetMaxX", 0);
}

void BoxColliderComponent::Initialize()
{
    //todo should be added as required

    auto TexOpt = GetOwner()->GetComponent<TextureComponent>();//todo make it independent of texture later on
    if(TexOpt.has_value() == false)
    {
        SDL_LogError(0, "Box collider requires texture information!!!");
        return;
    }

    m_TextureComponent = TexOpt.value();

    OnUpdateSceneTransform();

    //SDL_Log("Adding box of type %d, count before: %d", GetCollisionObjectType(), Engine::Get()->GetCollisionWorld()->GetBoxesCount());

    if(GetCollisionObjectType() == CollisionObjectType::Player)
    {
        SDL_Log("Adding player box, count before: %d", Engine::Get()->GetCollisionWorld()->GetBoxesCount());
    }

    m_SelfShared = std::shared_ptr<BoxColliderComponent>(this);
    Engine::Get()->GetCollisionWorld()->AddBox(m_SelfShared);

    if ((GetCollisionObjectType() & CollisionObjectType::Projectile) == CollisionObjectType::Projectile)
    {
        SDL_Log("Added projectile box, count after: %d", Engine::Get()->GetCollisionWorld()->GetBoxesCount());
    }
}

void BoxColliderComponent::UnInitialize()
{
    if ((GetCollisionObjectType() & CollisionObjectType::Projectile) == CollisionObjectType::Projectile)
    {
        SDL_Log("Removing projectile box, count after: %d", Engine::Get()->GetCollisionWorld()->GetBoxesCount());
    }

	Engine::Get()->GetCollisionWorld()->RemoveBox(m_SelfShared);
    if ((GetCollisionObjectType() & CollisionObjectType::Projectile) == CollisionObjectType::Projectile)
    {
        SDL_Log("Removed projectile box, count after: %d", Engine::Get()->GetCollisionWorld()->GetBoxesCount());
    }
}

void BoxColliderComponent::OnUpdateSceneTransform()
{
    if (m_TextureComponent == nullptr)
    {
        SDL_LogError(0, "Box collider requires texture information!!!");
        return;
    }

    auto TexRect = &m_TextureComponent->GetRectangle();
    auto [x, y] = GetOwner()->GetPositionXY();

    SetBoxMin(Vector2(x, y) + m_BoxOffsetMin);
    SetBoxMax(Vector2(x + TexRect->w, y + TexRect->h) + m_BoxOffsetMax);//todo update also scale from entity
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
    m_Box.m_Min.Set(boxMinPosition + boxMinOffset);
    m_Box.m_Max.Set(boxMaxPosition + boxMaxOffset);
}

AABB BoxColliderComponent::GetTweenSweepBox() const
{
    AABB SweepBox(m_Box.m_Min, m_Box.m_Max);
    SweepBox.UpdateMinMax(m_PreviousFrameBox.m_Min);
    SweepBox.UpdateMinMax(m_PreviousFrameBox.m_Max);
    return SweepBox;
}

void BoxColliderComponent::SetBoxMin(Vector2 boxMin)
{
    m_PreviousFrameBox.m_Min.Set(m_Box.m_Min);
    m_Box.m_Min.Set(boxMin);
}

void BoxColliderComponent::SetBoxMax(Vector2 boxMax)
{
    m_PreviousFrameBox.m_Max.Set(m_Box.m_Max);
    m_Box.m_Max.Set(boxMax);
}

