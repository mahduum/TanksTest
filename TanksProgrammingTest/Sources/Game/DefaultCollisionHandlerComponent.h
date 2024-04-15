#pragma once
#include "EntityComponent.h"
#include "ICollisionHandlerComponent.h"

class DefaultCollisionHandlerComponent : public ICollisionHandlerComponent
{
public:
	DefaultCollisionHandlerComponent(Entity* Owner);
	DefaultCollisionHandlerComponent();

	virtual void OnCollision(CollisionInfo collisionInfo) override;

	virtual EntityComponent* Clone() const override { return new DefaultCollisionHandlerComponent(*this); }

	//virtual void Initialize() override;
};

