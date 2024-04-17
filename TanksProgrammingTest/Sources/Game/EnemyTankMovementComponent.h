#pragma once
#include "EntityComponent.h"
#include "MathLib.h"

class EnemyTankMovementComponent : public EntityComponent
{
public:

	EnemyTankMovementComponent(Entity* Owner);
	EnemyTankMovementComponent();

	virtual EntityComponent* Clone() const override { return new EnemyTankMovementComponent(*this); }
	void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;

	void Move(float DeltaTime);
	void SetNextNodeLocation();

private:

	int m_Speed;
	float m_NextNodeAlpha;
	Vector2 m_CurrentNodeLocation;
	Vector2 m_NextNodeLocation;
};

