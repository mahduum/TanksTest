#pragma once
#include "EntityComponent.h"
class AttributeComponent : public EntityComponent
{
public:
	AttributeComponent(Entity* Owner);
	AttributeComponent();

	std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<AttributeComponent>(); }

	int GetHealth() const { return m_Health; };
	void ApplyHealthChange(int HealthDelta);

private:

	int m_Health;
};

