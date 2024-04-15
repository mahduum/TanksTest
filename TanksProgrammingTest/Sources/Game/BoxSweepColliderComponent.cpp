#include "BoxSweepColliderComponent.h"

BoxSweepColliderComponent::BoxSweepColliderComponent(Entity* Owner) :
	BoxColliderComponent(Owner),
	m_SweepBox(Vector2::Zero, Vector2::Zero)
{
}

BoxSweepColliderComponent::BoxSweepColliderComponent() : BoxSweepColliderComponent(nullptr)
{
}

void BoxSweepColliderComponent::OnUpdateTransform()
{
	BoxColliderComponent::OnUpdateTransform();
	m_SweepBox = GetSweepBox();
}

const AABB& BoxSweepColliderComponent::GetBox() const
{
	return m_SweepBox;
}
