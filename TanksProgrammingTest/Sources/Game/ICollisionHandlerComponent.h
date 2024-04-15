#pragma once
#include "CollisionWorld.h"
#include "EntityComponent.h"

class ICollisionHandlerComponent : public EntityComponent
{
public:

	ICollisionHandlerComponent(Entity* Owner);
	ICollisionHandlerComponent();

	virtual void OnCollision(CollisionInfo collisionInfo) = 0;//todo give it pointer to collider
};

