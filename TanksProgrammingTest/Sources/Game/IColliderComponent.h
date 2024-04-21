#pragma once
#include "CollisionWorld.h"
#include "EntityComponent.h"
class IColliderComponent : public EntityComponent
{
public:
	IColliderComponent(Entity* Owner);
	IColliderComponent();

	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual CollisionFlags GetCollisionObjectType() const { return m_CollisionObjectType; }
	virtual std::type_index GetLookUpTypeIndex() const override { return typeid(IColliderComponent); }

protected:
	CollisionFlags m_CollisionObjectType;
};

