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

void ProjectileSpawnerComponent::SetSpawnPoint(Vector2& OutSpawnPoint)
{
	switch (GetOwner()->GetFacingDirection())
	{
	case FacingDirection::UP:
		GetTopSpawnPoint(OutSpawnPoint);
		break;
	case FacingDirection::DOWN:
		GetBottomSpawnPoint(OutSpawnPoint);
		break;
	case FacingDirection::RIGHT:
		GetRightSpawnPoint(OutSpawnPoint);
		break;
	case FacingDirection::LEFT:
		GetLeftSpawnPoint(OutSpawnPoint);
		break;
	}
}

void ProjectileSpawnerComponent::SpawnProjectile()
{
	//todo keep a pool of them
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	Entity* Projectile = ResourceManagerPtr->CreateEntityFromDataTemplate(GetNameToSpawn());//add name to config

	Vector2 OutSpawnPoint(0,0);
	SetSpawnPoint(OutSpawnPoint);

	Projectile->GetComponent<TextureComponent>().value()->SetPosition(OutSpawnPoint.x, OutSpawnPoint.y);
	Projectile->SetPosition(OutSpawnPoint.x, OutSpawnPoint.y);

	Projectile->SetFacingDirection(GetOwner()->GetFacingDirection());
	Projectile->Initialize();

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
