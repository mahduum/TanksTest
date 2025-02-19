#include "ProjectileSpawnerComponent.h"

#include "Entity.h"
#include "ProjectileCollisionHandlerComponent.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TextureComponent.h"

ProjectileSpawnerComponent::ProjectileSpawnerComponent(Entity* Owner) : EntitySpawnerComponent(Owner)
{
}

ProjectileSpawnerComponent::ProjectileSpawnerComponent() : ProjectileSpawnerComponent(nullptr)
{
}

void ProjectileSpawnerComponent::DoSpawn()
{
	SpawnProjectile();
}

void ProjectileSpawnerComponent::SpawnProjectile()
{
	//todo keep cached loaded prefabs
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	Entity* Projectile = ResourceManagerPtr->CreateEntityFromDataTemplate(GetNameToSpawn());//add name to config

	Vector2Int OutSpawnPoint(0,0);
	SetSpawnPoint(OutSpawnPoint);

	Projectile->GetComponent<TextureComponent>()->SetPosition(OutSpawnPoint.x, OutSpawnPoint.y);
	Projectile->SetPosition(OutSpawnPoint.x, OutSpawnPoint.y);

	Projectile->SetFacingDirection(GetOwner()->GetFacingDirection());
	Projectile->Initialize();

	Engine::Get()->GetActiveScene()->AddEntity(Projectile);
}

void ProjectileSpawnerComponent::SetSpawnPoint(Vector2Int& OutSpawnPoint)
{
	if(m_TextureComponent.expired())
	{
		return;
	}

	switch (GetOwner()->GetFacingDirection())
	{
	case FacingDirection::Up:
		GetTopSpawnPoint(OutSpawnPoint);
		break;
	case FacingDirection::Down:
		GetBottomSpawnPoint(OutSpawnPoint);
		break;
	case FacingDirection::Right:
		GetRightSpawnPoint(OutSpawnPoint);
		break;
	case FacingDirection::Left:
		GetLeftSpawnPoint(OutSpawnPoint);
		break;
	}
}

void ProjectileSpawnerComponent::GetTopSpawnPoint(Vector2Int& OutSpawnPoint) const
{
	auto TexRect = m_TextureComponent.lock()->GetRectangle();
	OutSpawnPoint.Set(TexRect.x + TexRect.w / 2 - 4, TexRect.y - 4);//todo set offset in load config or const
}

void ProjectileSpawnerComponent::GetBottomSpawnPoint(Vector2Int& OutSpawnPoint) const
{
	auto TexRect = m_TextureComponent.lock()->GetRectangle();
	OutSpawnPoint.Set(TexRect.x + TexRect.w / 2 - 4, TexRect.y + TexRect.h - 8);//todo set offset in load config or const
}

void ProjectileSpawnerComponent::GetRightSpawnPoint(Vector2Int& OutSpawnPoint) const
{
	auto TexRect = m_TextureComponent.lock()->GetRectangle();
	OutSpawnPoint.Set(TexRect.x + TexRect.w - 8, TexRect.y + TexRect.h/2 - 5);//todo set offset in load config or const
}

void ProjectileSpawnerComponent::GetLeftSpawnPoint(Vector2Int& OutSpawnPoint) const
{
	auto TexRect = m_TextureComponent.lock()->GetRectangle();
	OutSpawnPoint.Set(TexRect.x - 1, TexRect.y + TexRect.h / 2 - 5);//todo set offset in load config or const
}
