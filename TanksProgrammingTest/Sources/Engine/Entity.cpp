#include "Entity.h"
#include "EntityComponent.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "TextureComponent.h"
#include "../Game/BoxColliderComponent.h"
#include "../Game/ICollisionHandlerComponent.h"

void Entity::LoadFromConfig(nlohmann::json Config)
{
	m_Name = Config.value("Name", "");
	m_TransformType = StringToTransformType(Config.value("TransformType", ""));
	m_CanBeSteppedOn = Config.value("CanBeSteppedOn", false);

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	
	if (Config.find("Components") != Config.end())
	{
		for (auto ComponentItem : Config["Components"].items())
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
	for (auto Component : m_Components)
	{
 		Component->Initialize();
	}

	auto TexOpt = GetComponent<::TextureComponent>();
	if(TexOpt.has_value())
	{
		auto TextureRect = TexOpt.value()->GetRectangle();
		SetPosition(TextureRect.x, TextureRect.y);
	}
}

void Entity::Update(float DeltaTime)
{
	//todo: make update groups, first transform group then representation group
	//Transform group: first update predicted transforms with input groups, updated colliders transforms, correct transforms and colliders.
	//Update representation groups transforms -> that way we are sure that representation has correct transform to update to
	UpdateSceneTransform();

	for (auto Component : m_Components)
	{
		Component->Update(DeltaTime);
		//for example, we are updating movement on projectile in update, and movement input too, then on movement we move only the collider
		//
	}

	UpdateSceneTransform();

	if (m_Name == "Player")//todo a nicer way???
	{
		auto ActiveScene = Engine::Get()->GetActiveScene();
		auto BoxColliderOpt = GetComponent<BoxColliderComponent>();
		if (ActiveScene && BoxColliderOpt.has_value())
		{
			auto PositionWithVisualOffset = BoxColliderOpt.value()->GetBox().m_Min;
			ActiveScene->SetTargetAndCalculateFlowField(static_cast<int>(PositionWithVisualOffset.x), static_cast<int>(PositionWithVisualOffset.y));
		}
	}
}

void Entity::Draw()
{
	for (auto Component : m_Components)
	{
		Component->Draw();
	}
}

void Entity::UnInitialize()
{
	for (auto Component : m_Components)
	{
		Component->UnInitialize();
	}
}

void Entity::AddComponent(EntityComponent* Component)//todo add fix
{
	AddComponent<EntityComponent>(std::shared_ptr<EntityComponent>{ Component });
}

void Entity::RemoveComponent(EntityComponent* Component)//todo remove fix
{
	//RemoveComponent<decltype(Component)>();
	//auto RetIt = std::remove(m_Components.begin(), m_Components.end(), Component);
	//todo add new remove
}

void Entity::OnCollision(const CollisionInfo& collisionInfo)
{
	auto CollisionHandlerOption = GetComponent<ICollisionHandlerComponent>();

	if(CollisionHandlerOption.has_value())//todo get components of type
	{
		CollisionHandlerOption.value()->OnCollision(collisionInfo);
	}
	//todo how to call collision implementation??? it may have a marker component but how we know? this component would need to have
	//an inheritance and each entity
	//handle on Abstract collision handler, call on collider class and let it handle it by internally calling the implementor?
	//get implementor it is assumed to come with base
}

void Entity::UpdateSceneTransform()
{
	if (m_UpdateSceneTransform == false) return;

	m_UpdateSceneTransform = false;

	for (auto Comp : m_Components)
	{
		Comp->OnUpdateSceneTransform();
	}
}

void Entity::SetPosition(Vector2 position)
{
	m_ScenePosition.Set(position);
}

void Entity::SetPosition(int x, int y)
{
	m_ScenePosition.Set(static_cast<float>(x), static_cast<float>(y));
}

void Entity::SetFacingDirection(FacingDirection direction)
{
	//todo if there is time use showing different textures in
	switch (direction)//todo make this a matrix
	{
		case FacingDirection::RIGHT:
			m_ForwardDirection.Set(Vector2::Right);
			m_RightDirection.Set(Vector2::Up);
			m_RotationDegrees = 90;
			break;
		case FacingDirection::LEFT:
			m_ForwardDirection.Set(Vector2::Left);
			m_RightDirection.Set(Vector2::Down);
			m_RotationDegrees = -90;
			break;
		case FacingDirection::UP:
			m_ForwardDirection.Set(Vector2::Up);
			m_RightDirection.Set(Vector2::Left);
			m_RotationDegrees = 0;
			break;
		case FacingDirection::DOWN:
			m_ForwardDirection.Set(Vector2::Down);
			m_RightDirection.Set(Vector2::Right);
			m_RotationDegrees = 180;
			break;
	}

	m_FacingDirection = direction;
}

void Entity::SetFacingDirection(Vector2 direction)
{
	if (direction == Vector2::Up)
		SetFacingDirection(FacingDirection::UP);
	else if (direction == Vector2::Down)
		SetFacingDirection(FacingDirection::DOWN);
	else if (direction == Vector2::Right)
		SetFacingDirection(FacingDirection::RIGHT);
	else
		SetFacingDirection(FacingDirection::LEFT);
}

void Entity::SetTranslation(int x, int y)
{
	m_ScenePosition.x += x;
	m_ScenePosition.y += y;
}

auto Entity::GetPositionXY() const -> std::tuple<int, int>
{
	return std::make_tuple( static_cast<int>(m_ScenePosition.x), static_cast<int>(m_ScenePosition.y));
}

Vector2 Entity::GetPosition() const
{
	return m_ScenePosition;
}

Vector2 Entity::GetForwardVector() const
{
	return m_ForwardDirection;
}

Vector2 Entity::GetRightVector() const
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
