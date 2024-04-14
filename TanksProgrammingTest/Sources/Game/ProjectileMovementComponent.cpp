#include "ProjectileMovementComponent.h"

#include "Entity.h"

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
	//TODO: it sets this vector once but at the time it is not set yet, so it remains 0
	m_Direction.Set(GetOwner()->GetForwardVector());
	SDL_Log("Assigned direction: x: %f, y: %f", m_Direction.x, m_Direction.y);
}

void ProjectileMovementComponent::Update(float DeltaTime)
{
	//m_Direction.Set(GetOwner()->GetForwardVector());
	Move(DeltaTime);
}

void ProjectileMovementComponent::Draw()
{
	EntityComponent::Draw();
}

void ProjectileMovementComponent::UnInitialize()
{
	EntityComponent::UnInitialize();
}

void ProjectileMovementComponent::OnUpdateTransform()
{
	EntityComponent::OnUpdateTransform();
}

void ProjectileMovementComponent::Move(float DeltaTime)
{
	auto [x,y] = m_Direction * (m_Speed / 30);
	GetOwner()->SetTranslation(x, y);//bool if force update components? Or fix collisions is in box collider already?
	GetOwner()->SetComponentsTransformDirty();
	//SDL_Log("Moving projectile by x: %f, y: %f, direction x: %f, y: %f", x, y, m_Direction.x, m_Direction.y);
}
