#pragma once
#include "EntityComponent.h"
#include "ICollisionHandlerComponent.h"

class DefaultCollisionHandlerComponent : public ICollisionHandlerComponent
{
public:
	DefaultCollisionHandlerComponent(Entity* Owner);
	DefaultCollisionHandlerComponent();

	virtual void OnCollisionImpl(const std::shared_ptr<ColliderComponent>& collisionInfo) override;

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<DefaultCollisionHandlerComponent>(*this); }


	//virtual void Initialize() override;
};

