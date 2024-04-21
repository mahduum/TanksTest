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
	virtual void BacktraceCollisionsDelta(Vector2& CollisionDelta) override;
	bool IntersectsWith(const BoxColliderComponent& other) const;
	void SetBoxWithOffset(const Vector2 boxMinPosition, const Vector2 boxMaxPosition, const Vector2 boxMinOffset, const Vector2 boxMaxOffset);
	AABB GetTweenSweepBox() const;

protected:
	bool TryGetCollisionDelta(const IBoxColliderComponent& other, Vector2& collisionDelta) const;

	void SetBoxMin(Vector2 boxMin);
	void SetBoxMax(Vector2 boxMax);

	AABB m_Box;
	AABB m_PreviousFrameBox;

	Vector2 m_BoxOffsetMin;
	Vector2 m_BoxOffsetMax;

	std::weak_ptr<TextureComponent> m_TextureComponent;
};
