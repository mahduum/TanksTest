#pragma once
#include <optional>
#include <SDL_timer.h>
#include <unordered_set>
#include <atomic>

#include "EntityComponent.h"
#include "MathLib.h"

struct TargetInfo
{
	bool m_TargetInSight;
	std::optional<int> m_TargetDistance;
	Vector2 m_AttackDirection;
};

class EnemyTankMovementComponent : public EntityComponent
{
public:

	EnemyTankMovementComponent(Entity* Owner);
	EnemyTankMovementComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<EnemyTankMovementComponent>(*this); }
	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;

	void Move(float DeltaTime);
	void SetNextNodeLocation();
	bool TryGetTargetInfo(TargetInfo& Info);
	bool ScanForPlayer(Vector2 Direction, TargetInfo& Info);

private:

	int m_Speed;
	float m_NextNodeAlpha;
	Vector2 m_CurrentNodeLocation;
	Vector2 m_NextNodeLocation;

	SDL_TimerID m_CoolDownTimerID;
};

