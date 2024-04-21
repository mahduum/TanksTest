#pragma once
#include <SDL_rect.h>

#include "CollisionWorld.h"
#include "EntityComponent.h"
#include "IColliderComponent.h"

class IBoxColliderComponent : public IColliderComponent
{
public:
	IBoxColliderComponent(Entity* Owner);
	IBoxColliderComponent();

	bool TryGetCollisionDelta(const IBoxColliderComponent& other, Vector2& collisionDelta) const;
	virtual std::type_index GetLookUpTypeIndex() const override { return typeid(IBoxColliderComponent); }
	const virtual AABB& GetBox() const = 0;

protected:
	SDL_Rect* GetRectangle() const;
};


