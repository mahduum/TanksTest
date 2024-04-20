#pragma once

#include <optional>
#include <typeindex>
#include <vector>
#include <nlohmann/json.hpp>

#include "CollisionWorld.h"
#include "Engine.h"
#include "EntityComponent.h"
#include "../Game/ICollisionHandlerComponent.h"

class EntityComponent;

enum class TransformType : unsigned int
{
	Static,
	Dynamic
};

inline TransformType StringToTransformType(const std::string&& EnumName)
{
	if(EnumName == "Static")
		return TransformType::Static;
	if(EnumName == "Dynamic")
		return TransformType::Dynamic;

	return TransformType::Static;
}

template<typename T>
concept ComponentType = std::is_base_of_v<EntityComponent, T>;

class Entity
{

public:
	bool IsValid = true;
	Entity()
	{
		m_ComponentsMap = std::unordered_map<std::type_index, int>();
		m_Components = std::vector<std::shared_ptr<EntityComponent>>();
	}

	void LoadFromConfig(nlohmann::json Config);
	void Initialize();
	void Update(float DeltaTime);
	void Draw();
	void UnInitialize();

	std::string_view GetName() const { return { m_Name.c_str(), m_Name.size() }; }

	TransformType GetTransformType() const { return m_TransformType; }
	bool CanBeSteppedOn() const { return m_CanBeSteppedOn; }

	void AddComponent(EntityComponent* Component);
	void RemoveComponent(EntityComponent* Component);

	template<ComponentType T>
	void AddComponent(std::shared_ptr<T> Component)
	{
		m_Components.emplace_back(Component);
		m_ComponentsMap.insert({ Component->GetTypeIndex(), m_Components.size() - 1 });
	}

	template<ComponentType T>
	void RemoveComponent()//the type is enough?//todo finish remove
	{
		m_ComponentsMap[typeid(T)] = m_Components.size() - 1;
		std::type_index TypeIndex = typeid(T);
		auto It = m_ComponentsMap.find(TypeIndex);
		if(It != m_ComponentsMap.end())
		{
			m_Components.end() = std::remove(m_Components.begin(), m_Components.end(), It->second);
			m_ComponentsMap.erase(TypeIndex);
		}
	}

	template<ComponentType T>
	std::optional<std::shared_ptr<T>> GetComponent()
	{
		std::type_index index = typeid(T);

		auto it = m_ComponentsMap.find(typeid(T));
		if(it != m_ComponentsMap.end())
		{
			return std::static_pointer_cast<T>(m_Components.at(it->second));
		}

		return std::nullopt;
	}

	void OnCollision(const CollisionInfo& CollisionInfo);

	void UpdateSceneTransform();

	void SetPosition(Vector2 position);
	void SetPosition(int x, int y);
	void SetFacingDirection(FacingDirection direction);
	void SetFacingDirection(Vector2 direction);
	void SetTranslation(int x, int y);

	auto GetPositionXY() const -> std::tuple<int, int>;
	Vector2 GetPosition() const;
	Vector2 GetForwardVector() const;
	Vector2 GetRightVector() const;
	FacingDirection GetFacingDirection() const;

	float GetRotationDegrees() const;

	void SetComponentsTransformDirty();

	
private:
	std::vector<std::shared_ptr<EntityComponent>> m_Components;// = std::vector<std::shared_ptr<EntityComponent>>();//todo make it unordered set
	std::unordered_map<std::type_index, int> m_ComponentsMap;// {};
	std::string m_Name;
	
	Vector2 m_ScenePosition;
	Vector2 m_ForwardDirection;
	Vector2 m_RightDirection;
	float m_RotationDegrees = 0;
	FacingDirection m_FacingDirection = FacingDirection::UP;

	//fixed config values
	TransformType m_TransformType = TransformType::Static;
	bool m_CanBeSteppedOn = false;

	bool m_UpdateSceneTransform = true;
};