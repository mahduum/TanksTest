#include "DamageConfigComponent.h"

DamageConfigComponent::DamageConfigComponent(Entity* Owner) : EntityComponent(Owner), m_DamageAmount(10)
{
}

DamageConfigComponent::DamageConfigComponent() : DamageConfigComponent(nullptr)
{
}

void DamageConfigComponent::LoadFromConfig(nlohmann::json Config)
{
	EntityComponent::LoadFromConfig(Config);

	m_DamageAmount = Config.value("DamageAmount", 10);
}
