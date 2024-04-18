#include "BoxTweenSweepColliderComponent.h"
#include "TextureComponent.h"

#include "Entity.h"

BoxTweenSweepColliderComponent::BoxTweenSweepColliderComponent(Entity* Owner) :
	BoxColliderComponent(Owner),
	m_SweepBox(Vector2::Zero, Vector2::Zero)
{
}

BoxTweenSweepColliderComponent::BoxTweenSweepColliderComponent() : BoxTweenSweepColliderComponent(nullptr)
{
}

void BoxTweenSweepColliderComponent::OnUpdateSceneTransform()
{
	BoxColliderComponent::OnUpdateSceneTransform();
	m_SweepBox = GetTweenSweepBox();
}

const AABB& BoxTweenSweepColliderComponent::GetBox() const
{
	return m_SweepBox;
}
