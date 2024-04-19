#pragma once
#include "DefaultCollisionHandlerComponent.h"

class ProjectileCollisionHandlerComponent : public DefaultCollisionHandlerComponent
{
public:

	ProjectileCollisionHandlerComponent(Entity* Owner);
	ProjectileCollisionHandlerComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<ProjectileCollisionHandlerComponent>(*this); }

	virtual void OnCollision(const CollisionInfo& collisionInfo) override;
};

