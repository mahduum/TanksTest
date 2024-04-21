#pragma once
#include "CollisionWorld.h"
#include "EntityComponent.h"
#include "IColliderComponent.h"

class IBoxColliderComponent : public IColliderComponent
{
public:
	IBoxColliderComponent(Entity* Owner);
	IBoxColliderComponent();
	virtual void BacktraceCollisionsDelta(Vector2& CollisionDelta) = 0;
	virtual std::type_index GetLookUpTypeIndex() const override { return typeid(IBoxColliderComponent); }
	const virtual AABB& GetBox() const = 0;
};


