#pragma once
#include "BoxColliderComponent.h"

class BoxSweepColliderComponent : public BoxColliderComponent
{
public:

	BoxSweepColliderComponent(Entity* Owner);
	BoxSweepColliderComponent();

	virtual void OnUpdateTransform() override;
	virtual const AABB& GetBox() const override;

private:

	AABB m_SweepBox;
};

