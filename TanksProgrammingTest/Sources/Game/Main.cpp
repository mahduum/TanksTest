#include "AttributeComponent.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "TextureComponent.h"
#include "PlayerInputComponent.h"
#include "BoxColliderComponent.h"
#include "BoxTweenSweepColliderComponent.h"
#include "DamageConfigComponent.h"
#include "EnemyProjectileSpawnerComponent.h"
#include "EnemyTankMovementComponent.h"
#include "EnemyTankSpawnerComponent.h"
#include "PlayerProjectileSpawnerComponent.h"
#include "ProjectileCollisionHandlerComponent.h"
#include "ProjectileMovementComponent.h"

int main(int argc, char* argv[])
{
	Engine::Get()->Initialize();

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	ResourceManagerPtr->RegisterComponent("PlayerInputComponent", new PlayerInputComponent());
	ResourceManagerPtr->RegisterComponent("TextureComponent", new TextureComponent());
	ResourceManagerPtr->RegisterComponent("ProjectileCollisionHandlerComponent", new ProjectileCollisionHandlerComponent());
	ResourceManagerPtr->RegisterComponent("BoxColliderComponent", new BoxColliderComponent());
	ResourceManagerPtr->RegisterComponent("BoxTweenSweepColliderComponent", new BoxTweenSweepColliderComponent());
	ResourceManagerPtr->RegisterComponent("ProjectileMovementComponent", new ProjectileMovementComponent());
	ResourceManagerPtr->RegisterComponent("PlayerProjectileSpawnerComponent", new PlayerProjectileSpawnerComponent());
	ResourceManagerPtr->RegisterComponent("EnemyProjectileSpawnerComponent", new EnemyProjectileSpawnerComponent());
	ResourceManagerPtr->RegisterComponent("EnemyTankMovementComponent", new EnemyTankMovementComponent());
	ResourceManagerPtr->RegisterComponent("EnemyTankSpawnerComponent", new EnemyTankSpawnerComponent());
	ResourceManagerPtr->RegisterComponent("DamageConfigComponent", new DamageConfigComponent());
	ResourceManagerPtr->RegisterComponent("AttributeComponent", new AttributeComponent());

	Engine::Get()->CreateActiveSceneFromTemplate("MainScene");

	Engine::Get()->MainLoop();
	Engine::Get()->ShutDown();

	return 0;
}
