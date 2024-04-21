#include "BoxColliderComponent.h"
#include "CollisionWorld.h"
#include "TextureComponent.h"

#include "Entity.h"

BoxColliderComponent:: BoxColliderComponent(): BoxColliderComponent(nullptr)
{
}

BoxColliderComponent::BoxColliderComponent(Entity* Owner) :
	IBoxColliderComponent(Owner),
	m_Box(Vector2Int::Zero, Vector2Int::Zero),
	m_PreviousFrameBox(Vector2Int::Zero, Vector2Int::Zero),
	m_BoxOffsetMin(Vector2Int::Zero),
	m_BoxOffsetMax(Vector2Int::Zero)
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
    m_TextureComponent = GetOwner()->GetComponent<TextureComponent>();
    if(m_TextureComponent.expired())
    {
        SDL_LogError(0, "Box collider requires texture information!!!");
    }

	OnUpdateSceneTransform();
}

void BoxColliderComponent::UnInitialize()
{
	EntityComponent::UnInitialize();
}

void BoxColliderComponent::OnUpdateSceneTransform()
{
	if(const auto& Texture = m_TextureComponent.lock())
	{
		const auto& TexRect = Texture->GetRectangle();
		auto [x, y] = GetOwner()->GetPositionXY();

		SetBoxMin(Vector2Int(x, y) + m_BoxOffsetMin);
		SetBoxMax(Vector2Int(x + TexRect.w, y + TexRect.h) + m_BoxOffsetMax);
	}
}

bool BoxColliderComponent::IntersectsWith(const BoxColliderComponent& other) const
{
    return Intersect(m_Box, other.m_Box);
}

void BoxColliderComponent::SetBoxWithOffset(const Vector2Int boxMinPosition, const Vector2Int boxMaxPosition, const Vector2Int boxMinOffset, const Vector2Int boxMaxOffset)
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

void BoxColliderComponent::SetBoxMin(Vector2Int boxMin)
{
    m_PreviousFrameBox.m_Min.Set(m_Box.m_Min);
    m_Box.m_Min.Set(boxMin);
}

void BoxColliderComponent::SetBoxMax(Vector2Int boxMax)
{
    m_PreviousFrameBox.m_Max.Set(m_Box.m_Max);
    m_Box.m_Max.Set(boxMax);
}

bool BoxColliderComponent::TryGetCollisionDelta(const IBoxColliderComponent& other, Vector2Int& collisionDelta) const
{
	if (TryGetBoxCollisionDelta(GetBox(), other.GetBox(), collisionDelta))
	{
		return true;
	}

	return false;
}

void BoxColliderComponent::BacktraceCollisionsDelta(Vector2Int& CollisionDelta)
{
	const std::vector<std::shared_ptr<IBoxColliderComponent>> Colliders = Engine::Get()->GetCollisionWorld()->GetStaticBoxes();

	OnUpdateSceneTransform();

	for (const auto& OtherCollider : Colliders)
	{
		Vector2Int TempCollisionDelta(0, 0);

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

	if (CollisionDelta.x == 0)
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

	if (CollisionDelta.y == 0)
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

	if (CollisionDelta.x == 0 && CollisionDelta.y == 0)
	{
		return;
	}

	GetOwner()->SetTranslation(-CollisionDelta.x,CollisionDelta.y);

	OnUpdateSceneTransform();
}

