#include "EntityComponent.h"

EntityComponent::EntityComponent(Entity* Owner)
	: m_Owner(Owner)
{
}

void EntityComponent::LoadFromConfig(nlohmann::json Config)
{
}

void EntityComponent::Initialize()
{
}

void EntityComponent::Update(float DeltaTime)
{
}

void EntityComponent::Draw()
{
}

void EntityComponent::UnInitialize()
{
}

void EntityComponent::OnUpdateWorldTransform()
{
	//todo maybe should be implemented as scene component if there is time 
}
