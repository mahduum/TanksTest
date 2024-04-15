#pragma once
#include "EntityComponent.h"
class ColliderComponent : public EntityComponent
{

protected:
	ColliderComponent(Entity* Owner);
	ColliderComponent();

public:
	virtual void OnLoaded() override;
};


