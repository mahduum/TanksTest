#pragma once

#include "EntityComponent.h"

class ProjectileSpawnerComponent;
class Vector2;
class TextureComponent;

class PlayerInputComponent : public EntityComponent
{

public:
	PlayerInputComponent(Entity* Owner);
	PlayerInputComponent();

	virtual EntityComponent* Clone() const override { return new PlayerInputComponent(*this); }

	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;

	void FixCollisionsAABB(Vector2& collisionDelta);//TODO PUT IT ON A DIFFERENT COMPONENT, DEFER INPUT TO DIFFERENT COMPONENT

	void Shoot() const;

private:
	std::shared_ptr<TextureComponent> m_TextureComponent;
	std::shared_ptr<ProjectileSpawnerComponent> m_ProjectileSpawnerComponent;
};