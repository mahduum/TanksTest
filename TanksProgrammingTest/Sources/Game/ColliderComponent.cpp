#include "ColliderComponent.h"

#include "DefaultCollisionHandlerComponent.h"
#include "Engine.h"
#include "Entity.h"
#include "ResourceManager.h"

ColliderComponent::ColliderComponent(Entity* Owner) :
	EntityComponent(Owner),
	m_CollisionObjectType(CollisionFlags::All)
{
}

ColliderComponent::ColliderComponent() : ColliderComponent(nullptr)
{
}

void ColliderComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);
	m_CollisionObjectType = StringToCollisionObjectType(Config.value("CollisionObjectType", "All"));
}

void ColliderComponent::OnLoaded()
{
	EntityComponent::OnLoaded();
	if(!m_RequiredComponents.empty())
	{
		for (auto it = m_RequiredComponents.begin(); it != m_RequiredComponents.end(); ++it)
		{
			if (dynamic_cast<DefaultCollisionHandlerComponent*>(*it))
			{
				break;
			}

			if(it == std::prev(m_RequiredComponents.end()))
			{
				ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
				const EntityComponent* ComponentPrototype = ResourceManagerPtr->GetComponentPrototypeByName("DefaultCollisionHandlerComponent");
				auto RequiredComponent = ComponentPrototype->Clone();
				RequiredComponent->SetOwner(GetOwner());
				GetOwner()->AddComponent(RequiredComponent);
				RequiredComponent->Initialize();
			}
		}
	}
}
