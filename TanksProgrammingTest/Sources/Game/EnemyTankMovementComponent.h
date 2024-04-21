#pragma once
#include <optional>

#include "CollisionWorld.h"
#include "EntityComponent.h"
#include "MathLib.h"

struct TargetInfo
{
	bool m_TargetInSight = false;
	std::optional<int> m_TargetDistance;
	Vector2Int m_AttackDirection;
	CollisionFlags m_TargetType = CollisionFlags::None;
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
	bool TryGetTargetInfo(TargetInfo& Info) const;
	bool ScanForPlayer(Vector2Int Direction, TargetInfo& Info) const;

	void InvertSpeed() { m_Speed = -m_Speed; }

private:

	int m_Speed;
	float m_NextNodeAlpha;
	Vector2Int m_CurrentNodeLocation;
	Vector2Int m_NextNodeLocation;
};

