#pragma once
#include "IBoxColliderComponent.h"
#include "EntityComponent.h"
#include "Engine.h"

class TextureComponent;

class BoxColliderComponent : public IBoxColliderComponent
{
public:
	BoxColliderComponent(Entity* Owner);
	BoxColliderComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<BoxColliderComponent>(*this); }

	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void UnInitialize() override;
	virtual void OnUpdateSceneTransform() override;

	const virtual AABB& GetBox() const override { return m_Box; }
	virtual void BacktraceCollisionsDelta(Vector2Int& CollisionDelta) override;
	bool IntersectsWith(const BoxColliderComponent& other) const;
	void SetBoxWithOffset(const Vector2Int boxMinPosition, const Vector2Int boxMaxPosition, const Vector2Int boxMinOffset, const Vector2Int boxMaxOffset);
	AABB GetTweenSweepBox() const;

protected:
	bool TryGetCollisionDelta(const IBoxColliderComponent& other, Vector2Int& collisionDelta) const;

	void SetBoxMin(Vector2Int boxMin);
	void SetBoxMax(Vector2Int boxMax);

	AABB m_Box;
	AABB m_PreviousFrameBox;

	Vector2Int m_BoxOffsetMin;
	Vector2Int m_BoxOffsetMax;

	std::weak_ptr<TextureComponent> m_TextureComponent;
};
