#include "Entity.h"
#include "EntityComponent.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "TextureComponent.h"

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

	auto TextureRect = GetComponent<::TextureComponent>()->GetRectangle();
	m_Position.x = TextureRect.x;
	m_Position.y = TextureRect.y;
}

void Entity::Update(float DeltaTime)
{
	//compute world transform if needed
	//update components
	for (EntityComponent* Component : m_Components)
	{
		Component->Update(DeltaTime);
	}
	//update self if available (assumes entity itself has some functionality)
	//compute world transform
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

void Entity::UpdateWorldTransform()
{
	if (m_UpdateWorldTransform == false) return;

	m_UpdateWorldTransform = false;

	//recalculate position

	for (auto Comp : m_Components)
	{
		Comp->OnUpdateWorldTransform();
	}

}
