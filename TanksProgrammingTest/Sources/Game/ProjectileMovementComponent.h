#pragma once
#include "EntityComponent.h"
#include "MathLib.h"

class ProjectileMovementComponent : public EntityComponent
{
public:
	ProjectileMovementComponent();

	ProjectileMovementComponent(Entity* Owner);

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<ProjectileMovementComponent>(*this); }

	void LoadFromConfig(nlohmann::json Config) override;
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Draw() override;
	void UnInitialize() override;
	void OnUpdateSceneTransform() override;

	void Move(float DeltaTime);//update actor transform

private:
	int m_Speed;
	Vector2 m_Direction;
};

