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

class Entity
{

public:
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

	template<typename ComponentType>
	void AddComponent(std::shared_ptr<EntityComponent> Component)
	{
		static_assert(std::is_base_of_v<EntityComponent, ComponentType>, "ComponentType must be a subtype of EntityComponent");
		SDL_Log("Adding type id: %s, map size: %d", Component->GetTypeIndex().name(), m_ComponentsMap.size());
		m_Components.emplace_back(Component);
		m_ComponentsMap.insert({ Component->GetTypeIndex(), m_Components.size() - 1 });
	}

	template<typename ComponentType>
	void RemoveComponent()//the type is enough?
	{
		static_assert(std::is_base_of_v<EntityComponent, ComponentType>, "ComponentType must be a subtype of EntityComponent");
		//SDL_Log("Removing type id: %s", typeid(ComponentType).name());

		m_ComponentsMap[typeid(ComponentType)] = m_Components.size() - 1;
		std::type_index TypeIndex = typeid(ComponentType);
		auto It = m_ComponentsMap.find(TypeIndex);
		if(It != m_ComponentsMap.end())
		{
			m_Components.end() = std::remove(m_Components.begin(), m_Components.end(), It->second);
			m_ComponentsMap.erase(TypeIndex);
		}
	}

	template<typename ComponentType>
	std::optional<std::shared_ptr<ComponentType>> GetComponent()
	{
		static_assert(std::is_base_of_v<EntityComponent, ComponentType>, "ComponentType must be a subtype of EntityComponent");
		std::type_index index = typeid(ComponentType);
		SDL_Log("Getting type id: %s, map size: %d", index.name(), m_ComponentsMap.size());

		auto it = m_ComponentsMap.find(typeid(ComponentType));
		if(it != m_ComponentsMap.end())
		{
			return std::static_pointer_cast<ComponentType>(m_Components.at(it->second));
		}

		return std::nullopt;
	}


	/*template <typename ComponentType>
	ComponentType* GetComponent()
	{
		auto Component = GetComponentNew<ComponentType>().value();
		ComponentType* Ptr = Component.get();
		return Ptr;
		static_assert(std::is_base_of_v<EntityComponent, ComponentType>, "ComponentType must be a subtype of BoxCollisionComponent");

		for (EntityComponent* Component : m_Components)
		{
			if (ComponentType* TypedComponent = dynamic_cast<ComponentType*>(Component))
			{
				return TypedComponent;
			}
		}
		return nullptr;
	}*/

	void OnCollision(CollisionInfo collisionInfo);

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