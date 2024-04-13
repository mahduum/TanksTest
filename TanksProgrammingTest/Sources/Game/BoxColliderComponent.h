#pragma once
#include "EntityComponent.h"
#include "Engine.h"

class TextureComponent;

class BoxColliderComponent : public EntityComponent
{
public:

	BoxColliderComponent();

	BoxColliderComponent(Entity* Owner);

	virtual EntityComponent* Clone() const override { return new BoxColliderComponent(*this); }

	virtual void LoadFromConfig(nlohmann::json Config) override;
	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;
	virtual void UnInitialize() override;
	virtual void OnUpdateWorldTransform() override;

	bool IntersectsWith(const BoxColliderComponent& other) const;
	bool TryGetCollisionDelta(const BoxColliderComponent& other, Vector2& collisionDelta) const;
	void SetPosition(const Vector2 position);//
	void SetScaleOffset(const Vector2 offset);
	void SetBoxWithOffset(const Vector2 boxMinPosition, const Vector2 boxMaxPosition, const Vector2 boxMinOffset, const Vector2 boxMaxOffset);
	SDL_Rect* GetRectangle() const;
	const AABB& GetBox() const { return m_Box; }

private:
	TextureComponent* m_TextureComponent;
	AABB m_Box;//reduce the box and shift offset

	Vector2 m_BoxOffsetMin;
	Vector2 m_BoxOffsetMax;
};

