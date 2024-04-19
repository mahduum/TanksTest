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

	CollisionFlags m_CollisionObjectType;

public:

	void LoadFromConfig(nlohmann::json Config) override;
	virtual void OnLoaded() override;
	CollisionFlags GetCollisionObjectType() const { return m_CollisionObjectType; }
};


