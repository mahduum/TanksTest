#pragma once

#include "EntityComponent.h"

class PlayerProjectileSpawnerComponent;
class ProjectileSpawnerComponent;
class Vector2;
class TextureComponent;

class PlayerInputComponent : public EntityComponent
{

public:
	PlayerInputComponent(Entity* Owner);
	PlayerInputComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<PlayerInputComponent>(*this); }

	virtual void Initialize() override;
	virtual void Update(float DeltaTime) override;

	void FixCollisionsAABB(Vector2& collisionDelta);//TODO PUT IT ON A DIFFERENT COMPONENT, DEFER INPUT TO DIFFERENT COMPONENT

	void Shoot() const;

private:
	std::shared_ptr<TextureComponent> m_TextureComponent;
	std::shared_ptr<PlayerProjectileSpawnerComponent> m_ProjectileSpawnerComponent;
};