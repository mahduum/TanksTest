#include "ProjectileMovementComponent.h"

#include "BoxColliderComponent.h"
#include "BoxTweenSweepColliderComponent.h"
#include "Entity.h"

ProjectileMovementComponent::ProjectileMovementComponent() : EntityComponent(nullptr), m_Speed(0)
{
}

ProjectileMovementComponent::ProjectileMovementComponent(Entity* Owner) : EntityComponent(Owner), m_Speed(0)
{
}

void ProjectileMovementComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);

	m_Speed = Config.value("Speed", 200);
}

void ProjectileMovementComponent::Initialize()
{
	m_Direction.Set(GetOwner()->GetForwardVector());
}

void ProjectileMovementComponent::Update(float DeltaTime)
{
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

void ProjectileMovementComponent::OnUpdateSceneTransform()
{
	EntityComponent::OnUpdateSceneTransform();
}

void ProjectileMovementComponent::Move(float DeltaTime) const
{
	auto [x,y] = m_Direction * (m_Speed / 30);
	GetOwner()->SetTranslation(x, y);

	auto Collider = GetOwner()->GetComponent<IColliderComponent>();
	if(Collider == nullptr)
	{
		return;
	}

	Collider->OnUpdateSceneTransform();//TODO: maybe do this all in update physics? Have call only colliders?
	//todo cd... first update intentions that will be invisible, then correct transforms, and only then update what is visible on screen?
	//todo cd... so we update collisions an masse and call collision events (now we can check precise collision info on already known collisions)
	//todo cd... engine renders after all updates have happened, so we can have movement be happening in a different loop
	GetOwner()->SetComponentsTransformDirty(); //SDL_Log("Marked dirty without collision check!!! Moving projectile by x: %f, y: %f, direction x: %f, y: %f", x, y, m_Direction.x, m_Direction.y);
}