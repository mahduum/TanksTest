#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "CollisionWorld.h"
#include "Engine.h"

class EntityComponent;

class Entity
{

public:
	void LoadFromConfig(nlohmann::json Config);
	void Initialize();
	void Update(float DeltaTime);
	void Draw();
	void UnInitialize();

	void AddComponent(EntityComponent* Component);
	void RemoveComponent(EntityComponent* Component);

	std::string GetName() const { return { m_Name.c_str(), m_Name.size() }; }

	template <typename ComponentType>
	ComponentType* GetComponent()
	{
		static_assert(std::is_base_of_v<EntityComponent, ComponentType>, "ComponentType must be a subtype of BoxCollisionComponent");

		for (EntityComponent* Component : m_Components)
		{
			if (ComponentType* TypedComponent = dynamic_cast<ComponentType*>(Component))
			{
				return TypedComponent;
			}
		}
		return nullptr;
	}

	void OnCollision(CollisionInfo collisionInfo);

	void UpdateComponentsTransform();

	void SetPosition(Vector2 position);
	void SetPosition(int x, int y);
	void SetRotation(FacingDirection direction);
	void SetTranslation(int x, int y);

	auto GetPositionXY() const -> std::tuple<int, int>;
	Vector2 GetPosition() const;
	Vector2 GetForwardVector() const;
	Vector2 GetRightVector() const;
	FacingDirection GetFacingDirection() const;

	float GetRotationDegrees() const;

	void SetComponentsTransformDirty();

	
private:
	std::list<EntityComponent*> m_Components;//todo make it unordered set
	std::string m_Name;
	
	Vector2 m_Position;
	Vector2 m_ForwardDirection;
	Vector2 m_RightDirection;
	float m_RotationDegrees = 0;
	FacingDirection m_FacingDirection = FacingDirection::UP;

	bool m_UpdateComponentsTransform = true;
};