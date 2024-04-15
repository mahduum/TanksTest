#pragma once
#include "ColliderComponent.h"
#include "EntityComponent.h"
#include "Engine.h"

class TextureComponent;

class BoxColliderComponent : public ColliderComponent
{
public:

	BoxColliderComponent();

	BoxColliderComponent(Entity* Owner);

	virtual EntityComponent* Clone() const override { return new BoxColliderComponent(*this); }

	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;
	virtual void UnInitialize() override;
	virtual void OnUpdateTransform() override;

	bool IntersectsWith(const BoxColliderComponent& other) const;
	bool TryGetCollisionDelta(const BoxColliderComponent& other, Vector2& collisionDelta) const;
	void SetPosition(const Vector2 position);
	void SetScaleOffset(const Vector2 offset);
	void SetBoxWithOffset(const Vector2 boxMinPosition, const Vector2 boxMaxPosition, const Vector2 boxMinOffset, const Vector2 boxMaxOffset);
	const virtual AABB& GetBox() const { return m_Box; }//todo make it virtual
	AABB GetSweepBox() const;//todo make it virtual

private:

	void SetBoxMin(Vector2 boxMin);
	void SetBoxMax(Vector2 boxMax);

	SDL_Rect* GetRectangle() const;

	TextureComponent* m_TextureComponent;
	AABB m_Box;//reduce the box and shift offset
	AABB m_PreviousFrameBox;

	Vector2 m_BoxOffsetMin;
	Vector2 m_BoxOffsetMax;
};

