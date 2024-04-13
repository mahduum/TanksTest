#include "Engine.h"
#include "ResourceManager.h"
#include "TextureComponent.h"
#include "PlayerInputComponent.h"
#include "BoxColliderComponent.h"

int main(int argc, char* argv[])
{
	Engine::Get()->Initialize();

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	ResourceManagerPtr->RegisterComponent("PlayerInputComponent", new PlayerInputComponent());
	ResourceManagerPtr->RegisterComponent("TextureComponent", new TextureComponent());
	ResourceManagerPtr->RegisterComponent("BoxColliderComponent", new BoxColliderComponent());

	Engine::Get()->CreateActiveSceneFromTemplate("MainScene");

	Engine::Get()->MainLoop();
	Engine::Get()->ShutDown();

	return 0;
}