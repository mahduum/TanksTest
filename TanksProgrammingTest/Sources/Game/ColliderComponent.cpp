#include "ColliderComponent.h"

#include "DefaultCollisionHandlerComponent.h"
#include "Engine.h"
#include "Entity.h"
#include "ResourceManager.h"

ColliderComponent::ColliderComponent(Entity* Owner) : EntityComponent(Owner)
{
}

ColliderComponent::ColliderComponent() : ColliderComponent(nullptr)
{
}

void ColliderComponent::OnLoaded()//todo move it to base class and make generic the required component
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
				EntityComponent* RequiredComponent = ComponentPrototype->Clone();
				RequiredComponent->SetOwner(GetOwner());
				GetOwner()->AddComponent(RequiredComponent);
				RequiredComponent->Initialize();
				SDL_Log("Adding requirement component of type: %s", typeid(*RequiredComponent).name());
			}
		}
	}
}
