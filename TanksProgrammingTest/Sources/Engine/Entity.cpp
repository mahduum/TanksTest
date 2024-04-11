#include "Entity.h"
#include "EntityComponent.h"
#include "Engine.h"
#include "ResourceManager.h"

void Entity::LoadFromConfig(nlohmann::json Config)
{
	m_Name = Config.value("Name", "");

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	
	if (Config.find("Components") != Config.end())
	{
		for (auto ComponentItem : Config["Components"].items())
		{
			nlohmann::json ComponentConfig = ComponentItem.value();
			std::string Type = ComponentConfig["Type"];
			const EntityComponent* ComponentPrototype = ResourceManagerPtr->GetComponentPrototypeByName(Type);
			EntityComponent* NewComponent = ComponentPrototype->Clone();
			NewComponent->SetOwner(this);
			NewComponent->LoadFromConfig(ComponentConfig);
			AddComponent(NewComponent);
		}
	}
}

void Entity::Initialize()
{
	for (EntityComponent* Component : m_Components)
	{
		Component->Initialize();
	}
}

void Entity::Update(float DeltaTime)
{
	for (EntityComponent* Component : m_Components)
	{
		Component->Update(DeltaTime);
	}
}

void Entity::Draw()
{
	for (EntityComponent* Component : m_Components)
	{
		Component->Draw();
	}
}

void Entity::UnInitialize()
{
	for (EntityComponent* Component : m_Components)
	{
		Component->UnInitialize();
	}
}

void Entity::AddComponent(EntityComponent* Component)
{
	m_Components.push_back(Component);
}

void Entity::RemoveComponent(EntityComponent* Component)
{
	auto RetIt = std::remove(m_Components.begin(), m_Components.end(), Component);
}
