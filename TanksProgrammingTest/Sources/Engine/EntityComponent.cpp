#include "EntityComponent.h"

#include "Engine.h"
#include "Entity.h"
#include "ResourceManager.h"
#include <typeinfo>

EntityComponent::EntityComponent(Entity* Owner)
	: m_Owner(Owner)
{
}

void EntityComponent::LoadFromConfig(nlohmann::json Config)
{
	//load required components
	//if list is empty load default if there is a default one defined
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();

	if (Config.find("RequiredComponents") != Config.end())
	{
		for (auto ComponentItem : Config["RequiredComponents"].items())
		{
			nlohmann::json ComponentConfig = ComponentItem.value();
			std::string Type = ComponentConfig["Type"];
			const EntityComponent* ComponentPrototype = ResourceManagerPtr->GetComponentPrototypeByName(Type);
			EntityComponent* RequiredComponent = ComponentPrototype->Clone();
			RequiredComponent->SetOwner(GetOwner());
			RequiredComponent->LoadFromConfig(ComponentConfig);
			RequiredComponent->Initialize();
			GetOwner()->AddComponent(RequiredComponent);//todo mark this component as being depended on, use its type as a key, so if its dependant is removed we check if anything other depends on it before we remove it too
			SDL_Log("Adding requirement component of type: %s", typeid(*RequiredComponent).name());
		}
	}

	OnLoaded();
}

void EntityComponent::OnLoaded()
{
}

void EntityComponent::Initialize()
{
}

void EntityComponent::Update(float DeltaTime)
{
}

void EntityComponent::Draw()
{
}

void EntityComponent::UnInitialize()
{
}

void EntityComponent::OnUpdateTransform()
{
	//todo maybe should be implemented as scene component if there is time 
}
