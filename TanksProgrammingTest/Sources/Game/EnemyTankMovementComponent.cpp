#include "EnemyTankMovementComponent.h"

#include "BoxColliderComponent.h"
#include "Engine.h"
#include "ProjectileSpawnerComponent.h"
#include "Scene.h"

EnemyTankMovementComponent::EnemyTankMovementComponent(Entity* Owner) :
	EntityComponent(Owner), m_Speed(0), m_NextNodeAlpha(0), m_CoolDownTimerID(0)
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

	if(m_CoolDownTimerID != 0)
	{
		return;
	}

	//todo check if player is in line of sight, if not check left and right, if ok, rotate and start shooting, else set new node location, make event whenever player location changes
	TargetInfo OutInfo;

	if (TryGetTargetInfo(OutInfo))
	{
		GetOwner()->SetFacingDirection(OutInfo.m_AttackDirection);
	}

	constexpr int MinApproachDistanceInLOS = 300;
	if (OutInfo.m_TargetInSight && OutInfo.m_TargetDistance.has_value() &&
		MinApproachDistanceInLOS > OutInfo.m_TargetDistance.value())//todo value from config
	{
		auto callback = [](Uint32 interval, void* param) -> Uint32
			{
				static int counter;
				++counter %= 4;

				auto comp = static_cast<EnemyTankMovementComponent*>(param);

				if (counter >= 3)
				{
					SDL_RemoveTimer(comp->m_CoolDownTimerID);
					comp->m_CoolDownTimerID = 0;
					//comp->m_Sh
				}

				return interval;
			};

		m_CoolDownTimerID = SDL_AddTimer(1000, callback, this);
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

	auto newPosition = Vector2::Lerp(m_CurrentNodeLocation, m_NextNodeLocation, m_NextNodeAlpha);
	
	GetOwner()->SetPosition(static_cast<int>(newPosition.x), static_cast<int>(newPosition.y));

	auto [x, y] = GetOwner()->GetPositionXY();

	GetOwner()->SetComponentsTransformDirty();
	//todo stop when we are close
	//on update transforms
	//fix collisions
}

void EnemyTankMovementComponent::SetNextNodeLocation()
{
	m_NextNodeAlpha = 0;

	Vector2 NewNextNodeLocation{ m_NextNodeLocation.x, m_NextNodeLocation.y };
	Vector2 NewFacingDirection{ 0, 0 };

	auto [x, y] = m_NextNodeLocation;//GetOwner()->GetPositionXY();
	Engine::Get()->GetActiveScene()->GetNextNavNodeLocationFromLocation(m_NextNodeLocation.x, m_NextNodeLocation.y, NewNextNodeLocation, NewFacingDirection);
	m_CurrentNodeLocation = m_NextNodeLocation;
	m_NextNodeLocation = NewNextNodeLocation;

	GetOwner()->SetFacingDirection(NewFacingDirection);
}

bool EnemyTankMovementComponent::TryGetTargetInfo(TargetInfo& Info)
{
	if (ScanForPlayer(GetOwner()->GetForwardVector(), Info))
	{
		return true;
	}

	/*if(ScanForPlayer(GetOwner()->GetRightVector()))
	{
		AttackDirection= GetOwner()->GetRightVector();
		return true;
	}

	if (ScanForPlayer(GetOwner()->GetRightVector() * -1.0f))
	{
		AttackDirection = GetOwner()->GetRightVector() * -1.0f;
		return true;
	}*/
	return false;
}

bool EnemyTankMovementComponent::ScanForPlayer(Vector2 Direction, TargetInfo& Info)
{
	auto BoxOpt = GetOwner()->GetComponent<BoxColliderComponent>();

	if (BoxOpt.has_value())
	{
		auto FromBox = BoxOpt.value()->GetBox();
		auto QuarterExtentX = (FromBox.m_Max.x - FromBox.m_Min.x) / 4;
		auto QuarterExtentY = (FromBox.m_Max.y - FromBox.m_Min.y) / 4;

		FromBox.m_Min.Set(FromBox.m_Min.x + QuarterExtentX, FromBox.m_Min.y + QuarterExtentY);
		FromBox.m_Max.Set(FromBox.m_Max.x - QuarterExtentX, FromBox.m_Max.y - QuarterExtentY);

		Vector2 ExtentsOffset{ Direction * 330 };//todo detection distance and add min distance

		std::shared_ptr<BoxColliderComponent> OutIntersection;

		//scan each direction
		if (Engine::Get()->GetCollisionWorld()->SingleBoxCast(GetOwner()->GetPosition(), FromBox, ExtentsOffset, OutIntersection, CollisionObjectType::Player | CollisionObjectType::WorldStatic) &&
			OutIntersection->GetCollisionObjectType() == CollisionObjectType::Player)
		{
			auto colliderPos = OutIntersection->GetBox().m_Min;
			auto enemyColliderPos = BoxOpt.value()->GetBox().m_Min;
			auto diff = colliderPos - enemyColliderPos;
			auto dist = diff.Length();
			Info.m_AttackDirection = Direction;
			Info.m_TargetInSight = true;
			Info.m_TargetDistance = dist;
			SDL_Log("Enemy sees: %s, distance: %d", OutIntersection->GetOwner()->GetName().data(), Info.m_TargetDistance.value());

			return true;
		}
	}

	return false;
}
