#include "Entity.h"
#include "EntityComponent.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TextureComponent.h"
#include "../Game/BoxColliderComponent.h"
#include "../Game/ICollisionHandlerComponent.h"

Entity::Entity() : IsAlive(true), m_ScenePosition({-1, -1}){}

void Entity::LoadFromConfig(nlohmann::json Config)
{
	m_Name = Config.value("Name", "");
	m_TransformType = StringToTransformType(Config.value("TransformType", ""));
	m_CanBeSteppedOn = Config.value("CanBeSteppedOn", false);

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	
	if (Config.find("Components") != Config.end())
	{
		for (const auto& ComponentItem : Config["Components"].items())
		{
			nlohmann::json ComponentConfig = ComponentItem.value();
			std::string Type = ComponentConfig["Type"];
			const EntityComponent* ComponentPrototype = ResourceManagerPtr->GetComponentPrototypeByName(Type);
			auto NewComponent = ComponentPrototype->Clone();
			NewComponent->SetOwner(this);
			NewComponent->LoadFromConfig(ComponentConfig);
			AddComponent(NewComponent);
		}
	}
}

void Entity::Initialize()
{
	for (const auto& Component : m_Components)
	{
 		Component->Initialize();
	}

	if (auto ColliderComponent = GetComponent<IBoxColliderComponent>())
	{
		Engine::Get()->GetCollisionWorld()->AddBox(ColliderComponent);
	}

	if (m_ScenePosition.x < 0 || m_ScenePosition.y < 0)
	{
		if (auto TextureComponent = GetComponent<::TextureComponent>())
		{
			auto TextureRect = TextureComponent->GetRectangle();
			SetPosition(TextureRect.x, TextureRect.y);
		}
	}
}

void Entity::Update(float DeltaTime)
{
	UpdateSceneTransform();

	for (const auto& Component : m_Components)
	{
		Component->Update(DeltaTime);
	}

	UpdateSceneTransform();

	if (m_Name == "Player")//Could be done in nicer way, but I run out of time :)
	{
		auto ActiveScene = Engine::Get()->GetActiveScene();
		if (auto BoxCollider = GetComponent<IBoxColliderComponent>(); ActiveScene != nullptr)
		{
			auto PositionWithVisualOffset = BoxCollider->GetBox().m_Min;
			ActiveScene->SetTargetAndCalculateFlowField(PositionWithVisualOffset.x, PositionWithVisualOffset.y);
		}
	}
}

void Entity::Draw()
{
	for (const auto& Component : m_Components)
	{
		Component->Draw();
	}
}

void Entity::UnInitialize()
{
	for (const auto& Component : m_Components)
	{
		Component->UnInitialize();
	}
}

void Entity::OnCollision(const std::shared_ptr<IBoxColliderComponent>& CollisionInfo)
{
	if (auto CollisionHandler = GetComponent<ICollisionHandlerComponent>())
	{
		CollisionHandler->OnCollision(CollisionInfo);
	}
}

void Entity::UpdateSceneTransform()
{
	if (m_UpdateSceneTransform == false)
	{
		return;
	}

	m_UpdateSceneTransform = false;

	if (GetName() == "Player") {
		SDL_Log("Unmarked components transform dirty");
	}

	for (const auto& Comp : m_Components)
	{
		Comp->OnUpdateSceneTransform();
	}
}

void Entity::SetPosition(Vector2Int position)
{
	m_ScenePosition.Set(position);
}

void Entity::SetPosition(int x, int y)
{
	m_ScenePosition.Set(x, y);
}

void Entity::SetFacingDirection(FacingDirection direction)
{
	switch (direction)
	{
		case FacingDirection::Right:
			m_ForwardDirection.Set(Vector2Int::Right);
			m_RightDirection.Set(Vector2Int::Up);
			m_RotationDegrees = 90;
			break;
		case FacingDirection::Left:
			m_ForwardDirection.Set(Vector2Int::Left);
			m_RightDirection.Set(Vector2Int::Down);
			m_RotationDegrees = -90;
			break;
		case FacingDirection::Up:
			m_ForwardDirection.Set(Vector2Int::Up);
			m_RightDirection.Set(Vector2Int::Left);
			m_RotationDegrees = 0;
			break;
		case FacingDirection::Down:
			m_ForwardDirection.Set(Vector2Int::Down);
			m_RightDirection.Set(Vector2Int::Right);
			m_RotationDegrees = 180;
			break;
	}

	m_FacingDirection = direction;
}

void Entity::SetFacingDirection(Vector2Int direction)
{
	if (direction == Vector2Int::Up)
		SetFacingDirection(FacingDirection::Up);
	else if (direction == Vector2Int::Down)
		SetFacingDirection(FacingDirection::Down);
	else if (direction == Vector2Int::Right)
		SetFacingDirection(FacingDirection::Right);
	else
		SetFacingDirection(FacingDirection::Left);
}

void Entity::SetTranslation(int x, int y)
{
	m_ScenePosition.x += x;
	m_ScenePosition.y += y;

	//todo on update scene transform should happen here
}

auto Entity::GetPositionXY() const -> std::tuple<int, int>
{
	return std::make_tuple(static_cast<int>(m_ScenePosition.x), static_cast<int>(m_ScenePosition.y));
}

Vector2Int Entity::GetPosition() const
{
	return m_ScenePosition;
}

Vector2Int Entity::GetForwardVector() const
{
	return m_ForwardDirection;
}

Vector2Int Entity::GetRightVector() const
{
	return m_RightDirection;
}

FacingDirection Entity::GetFacingDirection() const
{
	return m_FacingDirection;
}

float Entity::GetRotationDegrees() const
{
	return m_RotationDegrees;
}

void Entity::SetComponentsTransformDirty()
{
	m_UpdateSceneTransform = true;
}
