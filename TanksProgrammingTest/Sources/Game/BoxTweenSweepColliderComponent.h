#pragma once
#include "BoxColliderComponent.h"

class BoxTweenSweepColliderComponent : public BoxColliderComponent
{
public:

	BoxTweenSweepColliderComponent(Entity* Owner);
	BoxTweenSweepColliderComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<BoxTweenSweepColliderComponent>(*this); }
	virtual void OnUpdateSceneTransform() override;
	virtual const AABB& GetBox() const override;

private:

	AABB m_SweepBox;
};