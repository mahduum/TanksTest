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

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<BoxColliderComponent>(*this); }

	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void UnInitialize() override;
	virtual void OnUpdateSceneTransform() override;
	const virtual AABB& GetBox() const { return m_Box; }//todo make it virtual

	bool IntersectsWith(const BoxColliderComponent& other) const;
	bool TryGetCollisionDelta(const BoxColliderComponent& other, Vector2& collisionDelta) const;
	void SetPosition(const Vector2 position);
	void SetScaleOffset(const Vector2 offset);
	void SetBoxWithOffset(const Vector2 boxMinPosition, const Vector2 boxMaxPosition, const Vector2 boxMinOffset, const Vector2 boxMaxOffset);
	AABB GetTweenSweepBox() const;//todo make it virtual

	std::shared_ptr<BoxColliderComponent> m_SelfShared;//todo add to all???

protected:

	void SetBoxMin(Vector2 boxMin);
	void SetBoxMax(Vector2 boxMax);

	SDL_Rect* GetRectangle();

	//std::shared_ptr<TextureComponent> m_TextureComponent;
	AABB m_Box;
	AABB m_PreviousFrameBox;

	Vector2 m_BoxOffsetMin;
	Vector2 m_BoxOffsetMax;

};

