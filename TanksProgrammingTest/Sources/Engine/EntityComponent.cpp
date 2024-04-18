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
			const EntityComponent* ComponentPrototype = ResourceManagerPtr->GetComponentPrototypeByName(Type);//todo only load prototypes but spawn them in Initialize
			auto RequiredComponent = ComponentPrototype->Clone();
			RequiredComponent->LoadFromConfig(ComponentConfig);
			RequiredComponent->SetOwner(GetOwner());
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
	//todo if there is already a required component then unload the prototype, else instantiate it, use post initialize, so we know what components there are
	//all components that have been defined in json are by this time created, so if there are any required components still needed they can be created and
	//initialiased in PostInitialize but when any component is attempted to be added twice from config definition will spit an error. Only default components allowed
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

void EntityComponent::OnUpdateSceneTransform()
{
	//todo maybe should be implemented as scene component if there is time 
}
