#pragma once
#include "EntityComponent.h"

//enum class ColliderType : unsigned int
//{
//	Static,
//	Dynamic
//};

class ColliderComponent : public EntityComponent
{

protected:

	ColliderComponent(Entity* Owner);
	ColliderComponent();

	//ColliderType m_Type;//todo set from config

public:

	virtual void OnLoaded() override;
	//ColliderType GetType() const { return m_Type; }
};


