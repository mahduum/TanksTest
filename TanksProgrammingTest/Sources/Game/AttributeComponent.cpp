#include "AttributeComponent.h"

#include "Entity.h"
#include "MathLib.h"

AttributeComponent::AttributeComponent(Entity* Owner) : EntityComponent(Owner), m_Health(100)
{
}

AttributeComponent::AttributeComponent() : AttributeComponent(nullptr)
{
}

void AttributeComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);

	m_Health = Config.value("Health", 100);
}

void AttributeComponent::ApplyHealthChange(int HealthDelta)
{
	m_Health = MathLib::Max(m_Health + HealthDelta, 0);
	if( m_Health == 0 )
	{
		GetOwner()->IsAlive = false;
	}
}
