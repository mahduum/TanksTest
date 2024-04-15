#include "Engine.h"
#include "ResourceManager.h"
#include "TextureComponent.h"
#include "PlayerInputComponent.h"
#include "BoxColliderComponent.h"
#include "BoxSweepColliderComponent.h"
#include "DefaultCollisionHandlerComponent.h"
#include "EnemyTankMovementComponent.h"
#include "ProjectileCollisionHandlerComponent.h"
#include "ProjectileMovementComponent.h"
#include "ProjectileSpawnerComponent.h"

int main(int argc, char* argv[])
{
	Engine::Get()->Initialize();

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	ResourceManagerPtr->RegisterComponent("PlayerInputComponent", new PlayerInputComponent());
	ResourceManagerPtr->RegisterComponent("TextureComponent", new TextureComponent());
	ResourceManagerPtr->RegisterComponent("DefaultCollisionHandlerComponent", new DefaultCollisionHandlerComponent());
	ResourceManagerPtr->RegisterComponent("ProjectileCollisionHandlerComponent", new ProjectileCollisionHandlerComponent());
	//ResourceManagerPtr->RegisterComponent("ColliderComponent", new ColliderComponent());
	ResourceManagerPtr->RegisterComponent("BoxColliderComponent", new BoxColliderComponent());
	ResourceManagerPtr->RegisterComponent("BoxSweepColliderComponent", new BoxSweepColliderComponent());
	ResourceManagerPtr->RegisterComponent("ProjectileMovementComponent", new ProjectileMovementComponent());
	ResourceManagerPtr->RegisterComponent("ProjectileSpawnerComponent", new ProjectileSpawnerComponent());
	ResourceManagerPtr->RegisterComponent("EnemyTankMovementComponent", new EnemyTankMovementComponent());

	Engine::Get()->CreateActiveSceneFromTemplate("MainScene");

	Engine::Get()->MainLoop();
	Engine::Get()->ShutDown();

	return 0;
}
