#include "BoxColliderComponent.h"
#include "CollisionWorld.h"
#include "TextureComponent.h"

#include "Entity.h"

BoxColliderComponent:: BoxColliderComponent(): BoxColliderComponent(nullptr)
{
}

BoxColliderComponent::BoxColliderComponent(Entity* Owner) :
	IBoxColliderComponent(Owner),
	m_TextureComponent(nullptr),
	m_Box(Vector2::Zero, Vector2::Zero),
	m_PreviousFrameBox(Vector2::Zero, Vector2::Zero),
	m_BoxOffsetMin(Vector2::Zero),
	m_BoxOffsetMax(Vector2::Zero)
{
}

void BoxColliderComponent::LoadFromConfig(nlohmann::json Config)
{
    IBoxColliderComponent::LoadFromConfig(Config);

    m_BoxOffsetMin.x = Config.value("OffsetMinX", 0);
    m_BoxOffsetMin.y = Config.value("OffsetMinY", 0);
    m_BoxOffsetMax.x = Config.value("OffsetMaxX", 0);
    m_BoxOffsetMax.y = Config.value("OffsetMaxY", 0);
}

void BoxColliderComponent::Initialize()
{
    m_TextureComponent = GetOwner()->GetComponent<TextureComponent>();//todo add texture back as weak
    if(m_TextureComponent == nullptr)
    {
        SDL_LogError(0, "Box collider requires texture information!!!");
    }

    OnUpdateSceneTransform();

    std::shared_ptr<IBoxColliderComponent> CopyPointer = GetOwner()->GetComponent<IBoxColliderComponent>();//todo add box collider interface

    Engine::Get()->GetCollisionWorld()->AddBox(std::static_pointer_cast<BoxColliderComponent>(CopyPointer));//todo interface and add from elsewhere
}

void BoxColliderComponent::UnInitialize()
{
	EntityComponent::UnInitialize();
}

void BoxColliderComponent::OnUpdateSceneTransform()
{
    auto TexRect = GetRectangle();
    auto [x, y] = GetOwner()->GetPositionXY();

    SetBoxMin(Vector2(x, y) + m_BoxOffsetMin);
    SetBoxMax(Vector2(x + TexRect->w, y + TexRect->h) + m_BoxOffsetMax);
}

bool BoxColliderComponent::IntersectsWith(const BoxColliderComponent& other) const
{
    return Intersect(m_Box, other.m_Box);
}

void BoxColliderComponent::SetBoxWithOffset(const Vector2 boxMinPosition, const Vector2 boxMaxPosition, const Vector2 boxMinOffset, const Vector2 boxMaxOffset)
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

SDL_Rect* BoxColliderComponent::GetRectangle() const
{
	return &m_TextureComponent->GetRectangle();
}

bool BoxColliderComponent::TryGetCollisionDelta(const IBoxColliderComponent& other, Vector2& collisionDelta) const
{
	if (TryGetBoxCollisionDelta(GetBox(), other.GetBox(), collisionDelta))
	{
		return true;
	}

	return false;
}

void BoxColliderComponent::BacktraceCollisionsDelta(Vector2& CollisionDelta)
{
	const std::vector<std::shared_ptr<IBoxColliderComponent>> Colliders = Engine::Get()->GetCollisionWorld()->GetStaticBoxes();

	OnUpdateSceneTransform();

	for (const auto& OtherCollider : Colliders)
	{
		Vector2 TempCollisionDelta(0, 0);

		if (OtherCollider.get() != this && TryGetCollisionDelta(*OtherCollider, TempCollisionDelta))
		{
			if (MathLib::Abs(TempCollisionDelta.x) > MathLib::Abs(CollisionDelta.x))
			{
				CollisionDelta.x = TempCollisionDelta.x;
			}

			if (MathLib::Abs(TempCollisionDelta.y) > MathLib::Abs(CollisionDelta.y))
			{
				CollisionDelta.y = TempCollisionDelta.y;
			}
		}
	}

	int MaxWidth = 0, MaxHeight = 0;

	SDL_GetWindowSize(Engine::Get()->GetWindow(), &MaxWidth, &MaxHeight);

	if (MathLib::NearZero(CollisionDelta.x))
	{
		if (GetBox().m_Max.x > MaxWidth)
		{
			CollisionDelta.x = GetBox().m_Max.x - MaxWidth;
		}

		if (GetBox().m_Min.x < 0)
		{
			CollisionDelta.x = GetBox().m_Min.x;
		}
	}

	if (MathLib::NearZero(CollisionDelta.y))
	{
		if (GetBox().m_Max.y > MaxHeight)
		{
			CollisionDelta.y = GetBox().m_Max.y - MaxHeight;
		}

		if (GetBox().m_Min.y < 0)
		{
			CollisionDelta.y = GetBox().m_Min.y;
		}
	}

	if (MathLib::NearZero(CollisionDelta.x) && MathLib::NearZero(CollisionDelta.y))
	{
		return;
	}

	GetOwner()->SetTranslation(-static_cast<int>(CollisionDelta.x), -static_cast<int>(CollisionDelta.y));

	OnUpdateSceneTransform();
}

