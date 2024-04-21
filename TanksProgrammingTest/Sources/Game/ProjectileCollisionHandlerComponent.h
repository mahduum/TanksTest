#pragma once
#include "ICollisionHandlerComponent.h"

class ProjectileCollisionHandlerComponent : public ICollisionHandlerComponent
{
public:

	ProjectileCollisionHandlerComponent(Entity* Owner);
	ProjectileCollisionHandlerComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<ProjectileCollisionHandlerComponent>(*this); }

	virtual void OnCollisionImpl(const std::shared_ptr<IBoxColliderComponent>& CollisionInfo) override;
};

