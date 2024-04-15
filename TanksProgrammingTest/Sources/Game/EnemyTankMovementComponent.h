#pragma once
#include "EntityComponent.h"

class EnemyTankMovementComponent : public EntityComponent
{
public:

	EnemyTankMovementComponent(Entity* Owner);
	EnemyTankMovementComponent();

	virtual EntityComponent* Clone() const override { return new EnemyTankMovementComponent(*this); }

};

