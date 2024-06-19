#pragma once

#include <optional>
#include <typeindex>
#include <vector>
#include <nlohmann/json.hpp>

#include "CollisionWorld.h"
#include "Engine.h"
#include "EntityComponent.h"

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
	Entity();

	void LoadFromConfig(nlohmann::json Config);
	void Initialize();
	void Update(float DeltaTime);
	void Draw();
	void UnInitialize();

	std::string_view GetName() const { return { m_Name.c_str(), m_Name.size() }; }

	TransformType GetTransformType() const { return m_TransformType; }
	bool CanBeSteppedOn() const { return m_CanBeSteppedOn; }

	template<ComponentType T>
	void AddComponent(std::shared_ptr<T> Component)
	{
		m_Components.emplace_back(Component);
		m_ComponentsMap.insert({ Component->GetLookUpTypeIndex(), m_Components.size() - 1 });
	}

	template<ComponentType T>
	void RemoveComponent()
	{
		std::type_index TypeIndex = T().GetLookUpTypeIndex();
		m_ComponentsMap[TypeIndex] = m_Components.size() - 1;
		auto It = m_ComponentsMap.find(TypeIndex);
		if(It != m_ComponentsMap.end())
		{
			auto IteratorToSwap = m_Components.begin() + It->second;
			auto SwapWithIterator = m_Components.size() - 1;
			auto MovedTypeIndex = (*std::prev(m_Components.end()))->GetLookUpTypeIndex();
			//set released index to moved type
			m_ComponentsMap[MovedTypeIndex] = It->second;
			//erase moved type
			m_ComponentsMap.erase(TypeIndex);
			//do the swap
			std::iter_swap(IteratorToSwap, SwapWithIterator);
			//remove from the back
			m_Components.pop_back();
		}
	}

	template<ComponentType T>
	std::shared_ptr<T> GetComponent()
	{
		std::type_index TypeIndex = typeid(T);
		std::type_index TrueTypeIndex = typeid(T);
		//if it is interface ast to the available type
		if constexpr (std::is_abstract_v<T> == false)
		{
			TypeIndex = T().GetLookUpTypeIndex();
			TrueTypeIndex = T().GetTrueTypeIndex();
		}

		auto it = m_ComponentsMap.find(TypeIndex);
		if(it != m_ComponentsMap.end())
		{
			auto Found = m_Components.at(it->second);
			
			if constexpr (std::is_abstract_v<T>)
			{
				return std::dynamic_pointer_cast<T>(m_Components.at(it->second));//todo avoid dynamic and deduce return type different from input Type
			}

			if(Found->GetTrueTypeIndex() == TrueTypeIndex)
			{
				return std::static_pointer_cast<T>(m_Components.at(it->second));
			}
		}

		return nullptr;
	}

	void OnCollision(const std::shared_ptr<IBoxColliderComponent>& CollisionInfo);

	void UpdateSceneTransform();//this should not happen before we calculate all

	void SetPosition(Vector2Int position);
	void SetPosition(int x, int y);
	void SetFacingDirection(FacingDirection direction);
	void SetFacingDirection(Vector2Int direction);
	void SetTranslation(int x, int y);

	auto GetPositionXY() const -> std::tuple<int, int>;
	Vector2Int GetPosition() const;
	Vector2Int GetForwardVector() const;
	Vector2Int GetRightVector() const;
	FacingDirection GetFacingDirection() const;

	float GetRotationDegrees() const;
	void SetComponentsTransformDirty();

	bool IsAlive;

private:
	std::vector<std::shared_ptr<EntityComponent>> m_Components;
	std::unordered_map<std::type_index, int> m_ComponentsMap;
	std::string m_Name;
	
	Vector2Int m_ScenePosition;
	Vector2Int m_ForwardDirection;
	Vector2Int m_RightDirection;
	float m_RotationDegrees = 0;
	FacingDirection m_FacingDirection = FacingDirection::Up;

	//fixed config values
	TransformType m_TransformType = TransformType::Static;
	bool m_CanBeSteppedOn = false;
	bool m_UpdateSceneTransform = true;
};