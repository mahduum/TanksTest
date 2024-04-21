#pragma once

#include "EntityComponent.h"

class BoxColliderComponent;
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

	void Shoot() const;

private:
	std::shared_ptr<TextureComponent> m_TextureComponent;
	std::shared_ptr<PlayerProjectileSpawnerComponent> m_ProjectileSpawnerComponent;
	std::shared_ptr<BoxColliderComponent> m_BoxColliderComponent;
};