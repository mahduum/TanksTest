#pragma once
#include "CollisionWorld.h"
#include "EntityComponent.h"

class ICollisionHandlerComponent : public EntityComponent
{
public:

	ICollisionHandlerComponent(Entity* Owner);
	ICollisionHandlerComponent();

	virtual void LoadFromConfig(nlohmann::json Config) override;

	virtual std::type_index GetTypeIndex() const override { return typeid(ICollisionHandlerComponent); }
	virtual void OnCollisionImpl(const std::shared_ptr<IColliderComponent>& collisionInfo) = 0;

	void OnCollision(const std::shared_ptr<IColliderComponent>& CollisionInfo);

	CollisionFlags GetCollisionResponse() const { return m_CollisionResponse; }

private:

	CollisionFlags m_CollisionResponse;
};

