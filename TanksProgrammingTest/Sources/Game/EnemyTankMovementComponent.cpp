#include "EnemyTankMovementComponent.h"

#include "BoxColliderComponent.h"
#include "EnemyProjectileSpawnerComponent.h"
#include "Engine.h"
#include "ProjectileSpawnerComponent.h"
#include "Scene.h"

EnemyTankMovementComponent::EnemyTankMovementComponent(Entity* Owner) :
	EntityComponent(Owner), m_Speed(0), m_NextNodeAlpha(0)
{
}

EnemyTankMovementComponent::EnemyTankMovementComponent()  : EnemyTankMovementComponent(nullptr)
{
}

void EnemyTankMovementComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);

	m_Speed = Config.value("Speed", 100);
}

void EnemyTankMovementComponent::Initialize()
{
	EntityComponent::Initialize();
	m_CurrentNodeLocation = GetOwner()->GetPosition();
	m_NextNodeLocation = GetOwner()->GetPosition();
}

void EnemyTankMovementComponent::Update(float DeltaTime)
{
	EntityComponent::Update(DeltaTime);
	
	TargetInfo OutInfo;

	if (TryGetTargetInfo(OutInfo))
	{
		GetOwner()->SetFacingDirection(OutInfo.m_AttackDirection);
	}

	if(auto ProjectileSpawner = GetOwner()->GetComponent<EnemyProjectileSpawnerComponent>())
	{
		ProjectileSpawner->SetActive(OutInfo.m_TargetInSight);
	}

	constexpr int MinApproachDistanceInLOS = 300;
	if (OutInfo.m_TargetInSight && OutInfo.m_TargetDistance.has_value() &&
		MinApproachDistanceInLOS > OutInfo.m_TargetDistance.value())
	{
		//todo set cooldown time
		return;
	}

	if (m_NextNodeAlpha >= 1)
	{
		SetNextNodeLocation();
	}

	Move(DeltaTime);
}

void EnemyTankMovementComponent::Move(float DeltaTime)
{
	m_NextNodeAlpha += DeltaTime * (m_Speed / 30);

	auto newPosition = Vector2Int::Lerp(m_CurrentNodeLocation, m_NextNodeLocation, m_NextNodeAlpha);
	
	GetOwner()->SetPosition(static_cast<int>(newPosition.x), static_cast<int>(newPosition.y));

	auto [x, y] = GetOwner()->GetPositionXY();

	GetOwner()->SetComponentsTransformDirty();

	m_Speed = MathLib::Abs(m_Speed);
}

void EnemyTankMovementComponent::SetNextNodeLocation()
{
	m_NextNodeAlpha = 0;

	Vector2Int NewNextNodeLocation{ m_NextNodeLocation.x, m_NextNodeLocation.y };
	Vector2Int NewFacingDirection{ 0, 0 };

	auto [x, y] = m_NextNodeLocation;
	Engine::Get()->GetActiveScene()->GetNextNavNodeLocationFromLocation(m_NextNodeLocation.x, m_NextNodeLocation.y, NewNextNodeLocation, NewFacingDirection);//set this node as blocked
	m_CurrentNodeLocation = m_NextNodeLocation;
	m_NextNodeLocation = NewNextNodeLocation;

	GetOwner()->SetFacingDirection(NewFacingDirection);
}

bool EnemyTankMovementComponent::TryGetTargetInfo(TargetInfo& Info) const
{
	if (ScanForPlayer(GetOwner()->GetForwardVector(), Info))
	{
		return true;
	}

	/*optional behaviour but needs to be improved*/
	//if (ScanForPlayer(GetOwner()->GetRightVector(), Info))
	//{
	//	return true;
	//}

	//if (ScanForPlayer(GetOwner()->GetRightVector() * -1.0f, Info))
	//{
	//	return true;
	//}

	return false;
}

bool EnemyTankMovementComponent::ScanForPlayer(Vector2Int Direction, TargetInfo& Info) const
{
	if (auto BoxCollider = GetOwner()->GetComponent<BoxColliderComponent>())
	{
		auto FromBox = BoxCollider->GetBox();
		auto QuarterExtentX = (FromBox.m_Max.x - FromBox.m_Min.x) / 4;
		auto QuarterExtentY = (FromBox.m_Max.y - FromBox.m_Min.y) / 4;

		FromBox.m_Min.Set(FromBox.m_Min.x + QuarterExtentX, FromBox.m_Min.y + QuarterExtentY);
		FromBox.m_Max.Set(FromBox.m_Max.x - QuarterExtentX, FromBox.m_Max.y - QuarterExtentY);

		constexpr int ScanDistanceMultiplier = 330;
		Vector2Int ExtentsOffset{ Direction * ScanDistanceMultiplier };

		std::shared_ptr<IBoxColliderComponent> OutIntersection;

		if (Engine::Get()->GetCollisionWorld()->SingleBoxCast(
			GetOwner()->GetPosition(),
			FromBox, ExtentsOffset,
			OutIntersection,
			CollisionFlags::Player | CollisionFlags::WorldStatic) &&
			OutIntersection->GetCollisionObjectType() == CollisionFlags::Player)
		{
			auto colliderPos = OutIntersection->GetBox().m_Min;
			auto enemyColliderPos = BoxCollider->GetBox().m_Min;
			auto diff = colliderPos - enemyColliderPos;
			auto dist = diff.Length();
			Info.m_AttackDirection = Direction;
			Info.m_TargetInSight = true;
			Info.m_TargetDistance = dist;

			return true;
		}
	}

	return false;
}
