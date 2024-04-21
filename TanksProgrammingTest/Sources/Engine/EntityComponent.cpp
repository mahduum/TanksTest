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
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();

	if (Config.find("RequiredComponents") != Config.end())
	{
		for (const auto& ComponentItem : Config["RequiredComponents"].items())
		{
			nlohmann::json ComponentConfig = ComponentItem.value();
			std::string Type = ComponentConfig["Type"];
			const EntityComponent* ComponentPrototype = ResourceManagerPtr->GetComponentPrototypeByName(Type);
			auto RequiredComponent = ComponentPrototype->Clone();
			RequiredComponent->LoadFromConfig(ComponentConfig);
			RequiredComponent->SetOwner(GetOwner());
			RequiredComponent->Initialize();
			GetOwner()->AddComponent(RequiredComponent);
		}
	}
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

void EntityComponent::OnUpdateSceneTransform()
{
}
