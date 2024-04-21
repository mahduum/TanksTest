#include "EnemyTankCollisionHandlerComponent.h"

#include "EnemyTankMovementComponent.h"
#include "Entity.h"
#include "IBoxColliderComponent.h"
#include "Scene.h"

EnemyTankCollisionHandlerComponent::EnemyTankCollisionHandlerComponent(Entity* Owner) : ICollisionHandlerComponent(Owner)
{
}

EnemyTankCollisionHandlerComponent::EnemyTankCollisionHandlerComponent() : EnemyTankCollisionHandlerComponent(nullptr)
{
}

void EnemyTankCollisionHandlerComponent::OnCollisionImpl(const std::shared_ptr<IBoxColliderComponent>& OtherCollider)
{
	//get whichever cell has bigger value, backtrace or stop
	auto [otherX, otherY] = OtherCollider->GetOwner()->GetPositionXY();
	auto [x, y] = GetOwner()->GetPositionXY();

	if(auto ActiveScene = Engine::Get()->GetActiveScene())
	{
		int OtherFlowDistance = ActiveScene->GetFlowDistanceAtSceneCoords(otherX, otherY);
		int FlowDistance = ActiveScene->GetFlowDistanceAtSceneCoords(x, y);

		if (OtherFlowDistance > FlowDistance)
		{
			return;
		}

		if(auto Movement = GetOwner()->GetComponent<EnemyTankMovementComponent>())
		{
			//retrace back to previous cell or stop
			Movement->InvertSpeed();
		}
	}
}

