#pragma once
#include "CollisionWorld.h"
#include "EntityComponent.h"

class ICollisionHandlerComponent : public EntityComponent
{
public:

	ICollisionHandlerComponent(Entity* Owner);
	ICollisionHandlerComponent();

	virtual std::type_index GetTypeIndex() const override { return typeid(ICollisionHandlerComponent); }
	virtual void OnCollision(CollisionInfo collisionInfo) = 0;
};

