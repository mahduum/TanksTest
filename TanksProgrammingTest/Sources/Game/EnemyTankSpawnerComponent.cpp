#include "EnemyTankSpawnerComponent.h"

#include "Entity.h"
#include "TextureComponent.h"
#include "ResourceManager.h"
#include "Scene.h"

EnemyTankSpawnerComponent::EnemyTankSpawnerComponent(Entity* Owner) : EntitySpawnerComponent(Owner)
{
}

EnemyTankSpawnerComponent::EnemyTankSpawnerComponent() : EnemyTankSpawnerComponent(nullptr)
{
}

void EnemyTankSpawnerComponent::DoSpawn()
{
	if (Engine::Get()->GetActiveScene()->CanSpawnEnemy() == false)
	{
		return;
	}

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	Entity* EnemyTank = ResourceManagerPtr->CreateEntityFromDataTemplate(GetNameToSpawn());//add name to config

	Vector2 OutSpawnPoint(0, 0);
	SetSpawnPoint(OutSpawnPoint);

	EnemyTank->GetComponent<TextureComponent>().value()->SetPosition(OutSpawnPoint.x, OutSpawnPoint.y);
	EnemyTank->SetPosition(GetOwner()->GetPosition());
	EnemyTank->SetFacingDirection(GetOwner()->GetFacingDirection());
	EnemyTank->Initialize();

	Engine::Get()->GetActiveScene()->AddEntity(EnemyTank);
}

void EnemyTankSpawnerComponent::SetSpawnPoint(Vector2& OutSpawnPoint)
{
	auto TexOpt = GetOwner()->GetComponent<TextureComponent>();
	if(TexOpt.has_value())
	{
		auto TexRect = TexOpt.value()->GetRectangle();
		OutSpawnPoint.Set(TexRect.x, TexRect.y);
	}
}
