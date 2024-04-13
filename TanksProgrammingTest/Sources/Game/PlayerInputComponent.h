#pragma once

#include "EntityComponent.h"

class TextureComponent;

class PlayerInputComponent : public EntityComponent
{

public:
	PlayerInputComponent(Entity* Owner);
	PlayerInputComponent();

	virtual EntityComponent* Clone() const override { return new PlayerInputComponent(*this); }

	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;

	void FixCollisions();
	void FixCollisionsAABB();

private:
	TextureComponent* m_TextureComponent;
};