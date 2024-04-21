#pragma once
#include "ICollisionHandlerComponent.h"
class EnemyTankCollisionHandlerComponent : public ICollisionHandlerComponent
{
public:
	EnemyTankCollisionHandlerComponent(Entity* Owner);
	EnemyTankCollisionHandlerComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<EnemyTankCollisionHandlerComponent>(*this); }
	void OnCollisionImpl(const std::shared_ptr<IBoxColliderComponent>& OtherCollider) override;
};

