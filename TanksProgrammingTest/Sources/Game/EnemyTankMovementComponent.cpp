#include "EnemyTankMovementComponent.h"

#include <chrono>

#include "Engine.h"
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

	if (m_NextNodeAlpha >= 1)
	{
		SetNextNodeLocation();
		m_NextNodeAlpha = 0;
	}
	//else
	//{
		Move(DeltaTime);
	//}

}

void EnemyTankMovementComponent::Move(float DeltaTime)
{
	m_NextNodeAlpha += DeltaTime * (m_Speed / 30);

	//SDL_Log("Alpha: %f, incremented by: %f", m_NextNodeAlpha, DeltaTime * (m_Speed / 15));

	auto newPosition = Vector2::Lerp(m_CurrentNodeLocation, m_NextNodeLocation, m_NextNodeAlpha);//todo divide 30
	
	GetOwner()->SetPosition(static_cast<int>(newPosition.x), static_cast<int>(newPosition.y));

	auto [x, y] = GetOwner()->GetPositionXY();
	//SDL_Log("New actor position x: %d, y: %d", x, y);
	//auto [deltaX, deltaY] = NextNodeLocation * (m_Speed / 30);
	//GetOwner()->SetTranslation(deltaX, deltaY);


	/*if (MathLib::NearZero(1.0f - m_NextNodeAlpha))
	{
		SDL_Log("Recursive to next node");
		SetNextNodeLocation();
		m_NextNodeAlpha = 0;
		Move(DeltaTime);
		return;
	}*/

	GetOwner()->SetComponentsTransformDirty();
	//todo stop when we are close
	//on update transforms
	//fix collisions
}

void EnemyTankMovementComponent::SetNextNodeLocation()
{
	Vector2 NewNextNodeLocation{ m_NextNodeLocation.x, m_NextNodeLocation.y };
	Vector2 NewFacingDirection{ 0, 0 };

	auto [x, y] = m_NextNodeLocation;//GetOwner()->GetPositionXY();
	Engine::Get()->GetActiveScene()->GetNextNavNodeLocationFromLocation(m_NextNodeLocation.x, m_NextNodeLocation.y, NewNextNodeLocation, NewFacingDirection);
	m_CurrentNodeLocation = m_NextNodeLocation;
	m_NextNodeLocation = NewNextNodeLocation;

	GetOwner()->SetFacingDirection(NewFacingDirection);
}
