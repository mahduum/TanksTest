#include "CollisionWorld.h"

#include <algorithm>

void CollisionWorld::AddBox(BoxColliderComponent* box)
{
	m_Boxes.emplace_back(box);
}

void CollisionWorld::RemoveBox(BoxColliderComponent* box)
{
	if (auto iter = std::find(m_Boxes.begin(), m_Boxes.end(), box); iter != m_Boxes.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, m_Boxes.end() - 1);
		m_Boxes.pop_back();
	}
}
