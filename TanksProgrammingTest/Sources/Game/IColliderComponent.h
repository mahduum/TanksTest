#pragma once
#include "CollisionWorld.h"
#include "EntityComponent.h"

class IColliderComponent : public EntityComponent
{
public:
	IColliderComponent(Entity* Owner);
	IColliderComponent();

	void LoadFromConfig(nlohmann::json Config) override;
	CollisionFlags GetCollisionObjectType() const { return m_CollisionObjectType; }

protected:
	virtual std::type_index GetTypeIndex() const override { return typeid(IColliderComponent); }//todo rename to get interface type and add the get type index

	CollisionFlags m_CollisionObjectType;
};


