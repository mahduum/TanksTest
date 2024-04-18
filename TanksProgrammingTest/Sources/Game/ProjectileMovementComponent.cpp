#include "ProjectileMovementComponent.h"

#include "BoxColliderComponent.h"
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

void ProjectileMovementComponent::OnUpdateSceneTransform()
{
	EntityComponent::OnUpdateSceneTransform();
}

void ProjectileMovementComponent::Move(float DeltaTime)
{
	auto [x,y] = m_Direction * (m_Speed / 30);
	GetOwner()->SetTranslation(x, y);//bool if force update components? Or fix collisions is in box collider already?
	//move only collider so collisions can be verified:
	//todo verifiy if exists!!! or add as requirement
	GetOwner()->GetComponent<BoxColliderComponent>().value()->OnUpdateSceneTransform();//TODO: maybe do this all in update physics? Have call only colliders?
	//todo cd... first update intentions that will be invisible, then correct transforms, and only then update what is visible on screen?
	//todo cd... so we update collisions an masse and call collision events (now we can check precise collision info on already known collisions)
	//todo cd... engine renders after all updates have happened, so we can have movement be happening in a different loop
	GetOwner()->SetComponentsTransformDirty(); //SDL_Log("Marked dirty without collision check!!! Moving projectile by x: %f, y: %f, direction x: %f, y: %f", x, y, m_Direction.x, m_Direction.y);
}
