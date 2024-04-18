#include "CollisionWorld.h"

#include <algorithm>

#include "CollisionUtils.h"
#include "Entity.h"
#include "../Game/BoxColliderComponent.h"

void CollisionWorld::TestSweepAndPrune(std::function<void(Entity*, Entity*)>& f)//todo all the boxes with all others, instead use the same but for one collider that sweeps other boxes
{
	std::sort(m_Boxes.begin(), m_Boxes.end(),
		[](const std::shared_ptr<BoxColliderComponent>& a, const std::shared_ptr<BoxColliderComponent>& b)
		{
			return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
		});

	for (size_t i = 0; i < m_Boxes.size(); i++)//todo sweep only dynamic boxes (MUST DO COLLISIONS WITH DYNAMIC SEPARATELY TO AVOID REPETITIONS)
	{
		// Get max.x for current box
		auto a = m_Boxes[i];
		float max = a->GetBox().m_Max.x;
		for (size_t j = i + 1; j < m_Boxes.size(); j++)//todo with all other boxes (dynamic and static) (i+1, so we avoid double events)
		{
			auto b = m_Boxes[j];
			// If AABB[j] min is past the max bounds of AABB[i],
			// then there aren't any other possible intersections
			// against AABB[i]
			if (b->GetBox().m_Min.x > max)
			{
				break;
			}

			if (Intersect(a->GetBox(), b->GetBox()))//todo or use try get delta
			{
				f(a->GetOwner(), b->GetOwner());//todo let decide to the given component what to do with it
			}
		}
	}
}

void CollisionWorld::AddBox(const std::shared_ptr<BoxColliderComponent>& box)
{
	m_Boxes.emplace_back(box);
}

void CollisionWorld::RemoveBox(const std::shared_ptr<BoxColliderComponent>& box)
{
	if (const auto it = std::find(m_Boxes.begin(), m_Boxes.end(), box); it != m_Boxes.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(it, m_Boxes.end() - 1);
		m_Boxes.pop_back();
	}
}

void CollisionWorld::OnEntitiesCollision(Entity* a, Entity* b)
{
	CollisionInfo aCollisionInfo
	{
		b
	};
	CollisionInfo bCollisionInfo
	{
		a
	};
	a->OnCollision(aCollisionInfo);
	b->OnCollision(bCollisionInfo);
}
