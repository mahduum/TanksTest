#pragma once
#include "EntityComponent.h"
class DamageConfigComponent : public EntityComponent
{
public:
	DamageConfigComponent(Entity* Owner);
	DamageConfigComponent();

	virtual std::shared_ptr<EntityComponent> Clone() const override { return std::make_shared<DamageConfigComponent>(*this); }
	virtual void LoadFromConfig(nlohmann::json Config) override;

	int GetDamageAmount() const { return m_DamageAmount; }

private:
	int m_DamageAmount;
};

