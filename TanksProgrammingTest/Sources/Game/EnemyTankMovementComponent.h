#pragma once
#include "EntityComponent.h"

class EnemyTankMovementComponent : public EntityComponent
{
public:

	EnemyTankMovementComponent(Entity* Owner);
	EnemyTankMovementComponent();

	virtual EntityComponent* Clone() const override { return new EnemyTankMovementComponent(*this); }
	void LoadFromConfig(nlohmann::json Config) override;
	virtual void Update(float DeltaTime) override;

	void Move(float DeltaTime);

private:

	int m_Speed;
};

