#include "BoxColliderComponent.h"

#include "BoxTweenSweepColliderComponent.h"
#include "CollisionWorld.h"
#include "TextureComponent.h"

#include "Entity.h"

BoxColliderComponent:: BoxColliderComponent(): BoxColliderComponent(nullptr)
{
}

BoxColliderComponent::BoxColliderComponent(Entity* Owner) :
	ColliderComponent(Owner),
	//m_TextureComponent(nullptr),
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
}

void BoxColliderComponent::Initialize()
{
    auto TexOpt = GetOwner()->GetComponent<TextureComponent>();
    if(TexOpt.has_value() == false)
    {
        SDL_LogError(0, "Box collider requires texture information!!!");
        return;
    }

    OnUpdateSceneTransform();

    //m_SelfShared = std::shared_ptr<BoxColliderComponent>(this);
    //auto shared_made = std::make_shared<BoxColliderComponent>(*this);
    //SDL_Log("Old this: %d, form made this: %d", m_SelfShared.get(), shared_made.get());
    std::shared_ptr<BoxColliderComponent> copy;
    auto BoxOpt = GetOwner()->GetComponent<BoxColliderComponent>();
    auto BoxSweepOpt = GetOwner()->GetComponent<BoxTweenSweepColliderComponent>();
    copy = BoxOpt.has_value() ? BoxOpt.value() : BoxSweepOpt.value();

    Engine::Get()->GetCollisionWorld()->AddBox(copy);
    //SDL_Log("Added box: %d, self shared: %d to collision with entity: %d, self shared: %d, self shared inside: %d, owner name: %s, use count: %d", this, m_SelfShared, GetOwner(), m_SelfShared->GetOwner(), m_SelfShared.get()->GetOwner(), GetOwner()->GetName().data(), m_SelfShared.use_count());
    SDL_Log("Added box: %d, to collision with entity owner: %d, owner name: %s, box count: %d", this, GetOwner(), GetOwner()->GetName().data(), Engine::Get()->GetCollisionWorld()->GetBoxesCount());
}

void BoxColliderComponent::UnInitialize()
{
    SDL_Log("Before remove box: %d.", Engine::Get()->GetCollisionWorld()->GetBoxesCount());
	//Engine::Get()->GetCollisionWorld()->RemoveBox(this);
    SDL_Log("Removed box: %d, to collision with entity owner: %d, owner name: %s.", this, GetOwner(), GetOwner()->GetName().data());
    SDL_Log("After remove box: %d.", Engine::Get()->GetCollisionWorld()->GetBoxesCount());


	//SDL_Log("Uninitializing box: %d, self shared: %d, to entity: %d, self shared: %d, self shared inside: %d, owner name: %s, count: %d", this, m_SelfShared, GetOwner(), m_SelfShared->GetOwner(), m_SelfShared.get()->GetOwner(), GetOwner()->GetName().data(), m_SelfShared.use_count());
}

void BoxColliderComponent::OnUpdateSceneTransform()
{
    auto TexRect = GetRectangle();
    //auto TexRect = &m_TextureComponent->GetRectangle();
    auto [x, y] = GetOwner()->GetPositionXY();

    SetBoxMin(Vector2(x, y) + m_BoxOffsetMin);
    SetBoxMax(Vector2(x + TexRect->w, y + TexRect->h) + m_BoxOffsetMax);
}

SDL_Rect* BoxColliderComponent::GetRectangle()
{
    return &GetOwner()->GetComponent<TextureComponent>().value()->GetRectangle();
}

bool BoxColliderComponent::IntersectsWith(const BoxColliderComponent& other) const
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

