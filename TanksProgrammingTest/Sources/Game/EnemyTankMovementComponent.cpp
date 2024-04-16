#include "EnemyTankMovementComponent.h"

#include "Engine.h"
#include "Scene.h"

EnemyTankMovementComponent::EnemyTankMovementComponent(Entity* Owner) :
	EntityComponent(Owner), m_Speed(0)
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

void EnemyTankMovementComponent::Update(float DeltaTime)
{
	EntityComponent::Update(DeltaTime);

	Move(DeltaTime);
}

void EnemyTankMovementComponent::Move(float DeltaTime)
{
	auto [x, y] = GetOwner()->GetPositionXY();
	auto [dirX, dirY] = Engine::Get()->GetActiveScene()->GetFlowDirectionAtLocation(x, y);
	Vector2 FlowDirection(dirX, dirY);
	GetOwner()->SetFacingDirection(FlowDirection);
	auto [deltaX, deltaY] = FlowDirection * (m_Speed / 30);
	GetOwner()->SetTranslation(deltaX, deltaY);

	//todo stop when we are close
	//on update transforms
	//fix collisions
	GetOwner()->SetComponentsTransformDirty();
}
