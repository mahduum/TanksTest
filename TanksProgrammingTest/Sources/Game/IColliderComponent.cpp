#include "IColliderComponent.h"


IColliderComponent::IColliderComponent(Entity* Owner) : EntityComponent(Owner), m_CollisionObjectType(CollisionFlags::All)
{
}

IColliderComponent::IColliderComponent() : IColliderComponent(nullptr)
{
}

void IColliderComponent::LoadFromConfig(nlohmann::json Config)//todo to base
{
	EntityComponent::LoadFromConfig(Config);
	m_CollisionObjectType = StringToCollisionObjectType(Config.value("CollisionObjectType", "All"));
}
