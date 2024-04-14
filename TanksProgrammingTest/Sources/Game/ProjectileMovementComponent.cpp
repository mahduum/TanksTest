#include "ProjectileMovementComponent.h"

ProjectileMovementComponent::ProjectileMovementComponent() : EntityComponent(nullptr), m_Speed(0)
{
}

ProjectileMovementComponent::ProjectileMovementComponent(Entity* Owner) : EntityComponent(Owner), m_Speed(0)
{
}

void ProjectileMovementComponent::LoadFromConfig(nlohmann::json Config)
{
	m_Speed = Config.value("Speed", 1000);
}

void ProjectileMovementComponent::Initialize()
{
	EntityComponent::Initialize();
}

void ProjectileMovementComponent::Update(float DeltaTime)
{
	EntityComponent::Update(DeltaTime);
	//todo update actor's position
}

void ProjectileMovementComponent::Draw()
{
	EntityComponent::Draw();
}

void ProjectileMovementComponent::UnInitialize()
{
	EntityComponent::UnInitialize();
}

void ProjectileMovementComponent::OnUpdateWorldTransform()
{
	EntityComponent::OnUpdateWorldTransform();
}

void ProjectileMovementComponent::Move()
{
}
