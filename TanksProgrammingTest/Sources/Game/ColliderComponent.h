#pragma once
#include "CollisionWorld.h"
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

	CollisionObjectType m_CollisionObjectType;//todo set from config

public:

	void LoadFromConfig(nlohmann::json Config) override;
	virtual void OnLoaded() override;
	CollisionObjectType GetCollisionObjectType() const { return m_CollisionObjectType; }
};


