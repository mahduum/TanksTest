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
	virtual std::type_index GetLookUpTypeIndex() const override { return typeid(IColliderComponent); }//todo use it as interface only for boxes

protected:
	CollisionFlags m_CollisionObjectType;
};


