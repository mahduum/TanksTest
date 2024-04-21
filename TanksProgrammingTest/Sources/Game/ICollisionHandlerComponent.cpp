#include "ICollisionHandlerComponent.h"

#include "IColliderComponent.h"
#include "Engine.h"
#include "Entity.h"

ICollisionHandlerComponent::ICollisionHandlerComponent(Entity* Owner) : EntityComponent(Owner), m_CollisionResponse(CollisionFlags::All)
{
}

ICollisionHandlerComponent::ICollisionHandlerComponent() : ICollisionHandlerComponent(nullptr)
{
}

void ICollisionHandlerComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);
	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();

	if (Config.find("Blocks") != Config.end())
	{
		m_CollisionResponse = CollisionFlags::None;
		std::vector<std::string> Blocks = Config["Blocks"];

		for (const auto& FlagName : Blocks)
		{
			m_CollisionResponse |= StringToCollisionObjectType(FlagName);
		}
	}

	if (Config.find("Ignores") != Config.end())
	{
		std::vector<std::string> Ignores = Config["Ignores"];

		for (const auto& FlagName : Ignores)
		{
			m_CollisionResponse ^= StringToCollisionObjectType(FlagName);
		}
	}
}

void ICollisionHandlerComponent::OnCollision(const std::shared_ptr<IColliderComponent>& CollisionInfo)
{
	auto CollisionObjectType = CollisionInfo->GetCollisionObjectType();
	if(GetOwner()->GetName() == "PlayerProjectile")
	{
		SDL_Log("Collision event received on player projectile, my flag: %d, other obj type: %d, test: %d.", static_cast<int>( m_CollisionResponse), static_cast<int>(CollisionObjectType), (CollisionObjectType & m_CollisionResponse) == CollisionObjectType);
	}
	if((CollisionObjectType & m_CollisionResponse) == CollisionObjectType)
	{
		OnCollisionImpl(CollisionInfo);
	}
}
