#include "ProjectileSpawnerComponent.h"

#include "Entity.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TextureComponent.h"

ProjectileSpawnerComponent::ProjectileSpawnerComponent(Entity* Owner) : EntityComponent(Owner), m_TextureComponent(nullptr)
{
}

ProjectileSpawnerComponent::ProjectileSpawnerComponent() : ProjectileSpawnerComponent(nullptr)
{
}

void ProjectileSpawnerComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);
}

void ProjectileSpawnerComponent::Initialize()
{
	m_TextureComponent = GetOwner()->GetComponent<TextureComponent>();
	SDL_Log("Initializing projectile spawner...");
}

void ProjectileSpawnerComponent::Update(float DeltaTime)
{
	/*SDL_Log("IUpdating projectile spawner: %s", GetOwner()->GetName());
	if(m_Spawn)
	{
		m_Spawn = false;
		SpawnProjectile();
	}*/
}

void ProjectileSpawnerComponent::UnInitialize()
{
	EntityComponent::UnInitialize();
}

void ProjectileSpawnerComponent::OnUpdateWorldTransform()
{
	EntityComponent::OnUpdateTransform();
}

void ProjectileSpawnerComponent::SpawnProjectile()
{
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	Entity* Projectile = ResourceManagerPtr->CreateEntityFromDataTemplate("Projectile");//add name to config

	Vector2 SpawnPoint(0,0);
	FacingDirection Facing = GetOwner()->GetFacingDirection();
	switch (Facing)
	{
		case FacingDirection::UP:
			GetTopSpawnPoint(SpawnPoint);
			break;
		case FacingDirection::DOWN:
			GetBottomSpawnPoint(SpawnPoint);
			break;
		case FacingDirection::RIGHT:
			GetRightSpawnPoint(SpawnPoint);
			break;
		case FacingDirection::LEFT:
			GetLeftSpawnPoint(SpawnPoint);
			break;
	}

	Projectile->GetComponent<TextureComponent>()->SetPosition(SpawnPoint.x, SpawnPoint.y);
	Projectile->SetRotation(Facing);
	Projectile->Initialize();
	SDL_Log("Projectile rotation set to: %f, %f", Projectile->GetForwardVector().x, Projectile->GetForwardVector().y);
	Engine::Get()->GetActiveScene()->AddEntity(Projectile);
}

void ProjectileSpawnerComponent::GetTopSpawnPoint(Vector2& OutSpawnPoint) const
{
	auto TexRect = m_TextureComponent->GetRectangle();//always spawn depending on the current texture rect
	OutSpawnPoint.Set(TexRect.x + TexRect.w / 2, TexRect.y - 5);//offset - 5 set in load config or const
}

void ProjectileSpawnerComponent::GetBottomSpawnPoint(Vector2& OutSpawnPoint) const
{
	auto TexRect = m_TextureComponent->GetRectangle();
	OutSpawnPoint.Set(TexRect.x + TexRect.w / 2, TexRect.y + TexRect.h + 5);//offset - 5 set in load config or const
}

void ProjectileSpawnerComponent::GetRightSpawnPoint(Vector2& OutSpawnPoint) const
{
	auto TexRect = m_TextureComponent->GetRectangle();
	OutSpawnPoint.Set(TexRect.x + TexRect.w + 5, TexRect.y + TexRect.h/2);//offset - 5 set in load config or const
}

void ProjectileSpawnerComponent::GetLeftSpawnPoint(Vector2& OutSpawnPoint) const
{
	auto TexRect = m_TextureComponent->GetRectangle();
	OutSpawnPoint.Set(TexRect.x - 5, TexRect.y + TexRect.h / 2);//offset - 5 set in load config or const
}
