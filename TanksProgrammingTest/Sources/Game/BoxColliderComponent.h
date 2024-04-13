#pragma once
#include "EntityComponent.h"
#include "SDL_rect.h"

class TextureComponent;

class BoxColliderComponent : public EntityComponent
{
public:

	BoxColliderComponent();

	BoxColliderComponent(Entity* Owner);

	virtual EntityComponent* Clone() const override { return new BoxColliderComponent(*this); }

	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;
	virtual void UnInitialize() override;
	virtual void OnUpdateWorldTransform() override;

	bool Overlaps(const BoxColliderComponent& Other) const;
	SDL_Rect* GetRectangle() const;

private:
	TextureComponent* m_TextureComponent;
};

