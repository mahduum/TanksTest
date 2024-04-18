#pragma once
#include "EntityComponent.h"
#include "ICollisionHandlerComponent.h"

class DefaultCollisionHandlerComponent : public ICollisionHandlerComponent
{
public:
	DefaultCollisionHandlerComponent(Entity* Owner);
	DefaultCollisionHandlerComponent();

	virtual void OnCollision(CollisionInfo collisionInfo) override;

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<DefaultCollisionHandlerComponent>(*this); }


	//virtual void Initialize() override;
};

