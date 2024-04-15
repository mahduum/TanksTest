#pragma once
#include "DefaultCollisionHandlerComponent.h"

class ProjectileCollisionHandlerComponent : public DefaultCollisionHandlerComponent
{
public:

	ProjectileCollisionHandlerComponent(Entity* Owner);
	ProjectileCollisionHandlerComponent();

	virtual EntityComponent* Clone() const override { return new ProjectileCollisionHandlerComponent(*this); }

	virtual void OnCollision(CollisionInfo collisionInfo) override;
};

