#include "Entity.h"
#include "EntityComponent.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "TextureComponent.h"

void Entity::LoadFromConfig(nlohmann::json Config)
{
	m_Name = Config.value("Name", "");

	ResourceManager* ResourceManagerPtr = Engine::Get()->GetResourceManager();
	
	if (Config.find("Components") != Config.end())
	{
		for (auto ComponentItem : Config["Components"].items())
		{
			nlohmann::json ComponentConfig = ComponentItem.value();
			std::string Type = ComponentConfig["Type"];
			const EntityComponent* ComponentPrototype = ResourceManagerPtr->GetComponentPrototypeByName(Type);
			EntityComponent* NewComponent = ComponentPrototype->Clone();
			NewComponent->SetOwner(this);
			NewComponent->LoadFromConfig(ComponentConfig);
			AddComponent(NewComponent);
		}
	}
}

void Entity::Initialize()
{
	for (EntityComponent* Component : m_Components)
	{
		Component->Initialize();
	}

	auto TextureRect = GetComponent<::TextureComponent>()->GetRectangle();

	m_Position.x = TextureRect.x;
	m_Position.y = TextureRect.y;
	SetRotation(FacingDirection::UP);
}

void Entity::Update(float DeltaTime)
{
	//compute world transform if needed
	//update components
	for (EntityComponent* Component : m_Components)
	{
		Component->Update(DeltaTime);
	}
	//update self if available (assumes entity itself has some functionality)
	//compute world transform
}

void Entity::Draw()
{
	for (EntityComponent* Component : m_Components)
	{
		Component->Draw();
	}
}

void Entity::UnInitialize()
{
	for (EntityComponent* Component : m_Components)
	{
		Component->UnInitialize();
	}
}

void Entity::AddComponent(EntityComponent* Component)
{
	m_Components.push_back(Component);
}

void Entity::RemoveComponent(EntityComponent* Component)
{
	auto RetIt = std::remove(m_Components.begin(), m_Components.end(), Component);
}

void Entity::UpdateWorldTransform()
{
	if (m_UpdateWorldTransform == false) return;

	m_UpdateWorldTransform = false;

	//recalculate position

	for (auto Comp : m_Components)
	{
		Comp->OnUpdateWorldTransform();
	}

}

void Entity::SetPosition(Vector2 position)
{
	m_Position.Set(position);
}

void Entity::SetPosition(const int x, const int y)
{
	m_Position.Set(static_cast<float>(x), static_cast<float>(y));
}

void Entity::SetRotation(FacingDirection direction)
{

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

auto Entity::GetPositionXY() const -> std::tuple<int, int>
{
	return std::make_tuple( static_cast<int>(m_Position.x), static_cast<int>(m_Position.y));
}

Vector2 Entity::GetPosition() const
{
	return m_Position;
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
