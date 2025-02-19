#include "CollisionWorld.h"

#include <algorithm>
#include <ranges>
#include "CollisionUtils.h"
#include "Entity.h"
#include "../Game/IBoxColliderComponent.h"

void CollisionWorld::TestSweepAndPrune(const std::function<void(std::shared_ptr<IBoxColliderComponent>, std::shared_ptr<IBoxColliderComponent>)>& f)
{
	std::erase_if(m_DynamicBoxes, [](const std::shared_ptr<IBoxColliderComponent>& b)
		{
			return b->GetOwner()->IsAlive == false;
		});

	std::ranges::sort(m_StaticBoxes,
	                  [](const std::shared_ptr<IBoxColliderComponent>& a, const std::shared_ptr<IBoxColliderComponent>& b)
	                  {
		                  return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
	                  });

	std::ranges::sort(m_DynamicBoxes,
	                  [](const std::shared_ptr<IBoxColliderComponent>& a, const std::shared_ptr<IBoxColliderComponent>& b)
	                  {
		                  return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
	                  });

	for (auto a : m_DynamicBoxes)
	{
		int max = a->GetBox().m_Max.x;
		for (auto b : m_StaticBoxes)
		{
			if (b->GetBox().m_Min.x > max)
			{
				break;
			}

			if (Intersect(a->GetBox(), b->GetBox()))
			{
				f(a, b);
			}
		}
	}

	for (size_t i = 0; i < m_DynamicBoxes.size(); i++)
	{
		auto a = m_DynamicBoxes[i];
		int max = a->GetBox().m_Max.x;
		for (size_t j = i + 1; j < m_DynamicBoxes.size(); j++)
		{
			auto b = m_DynamicBoxes[j];
	
			if (b->GetBox().m_Min.x > max)
			{
				break;
			}

			if (Intersect(a->GetBox(), b->GetBox()))
			{
				f(a, b);
			}
		}
	}
}

bool CollisionWorld::MultiBoxCast(const Vector2Int& FromPosition, const AABB& FromBox, const Vector2Int& ExtentsOffset, std::vector<std::shared_ptr<IBoxColliderComponent>>& OutIntersections, CollisionFlags IncludedObjectTypes)
{
	AABB ExtendedBox(FromBox.m_Min, FromBox.m_Max);
	ExtendedBox.UpdateMinMax(FromBox.m_Min + ExtentsOffset);
	ExtendedBox.UpdateMinMax(FromBox.m_Max + ExtentsOffset);

	std::ranges::sort(m_StaticBoxes,
		[](const std::shared_ptr<IBoxColliderComponent>& a, const std::shared_ptr<IBoxColliderComponent>& b)
		{
		    return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
		});

	auto RangeStatic = std::ranges::views::filter(m_StaticBoxes,
		[IncludedObjectTypes, &ExtendedBox](const std::shared_ptr<IBoxColliderComponent>& IBoxColliderComponent)
		{
			return (IBoxColliderComponent->GetCollisionObjectType() & IncludedObjectTypes) == IBoxColliderComponent->GetCollisionObjectType() &&
				IBoxColliderComponent->GetBox().m_Min.x < ExtendedBox.m_Max.x &&
				Intersect(ExtendedBox, IBoxColliderComponent->GetBox());
		});

	for (const auto& Collider : RangeStatic)
	{
		OutIntersections.emplace_back(Collider);
	}

	std::ranges::sort(m_DynamicBoxes,
		[](const std::shared_ptr<IBoxColliderComponent>& a, const std::shared_ptr<IBoxColliderComponent>& b)
		{
			return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
		});

	auto RangeDynamic = std::ranges::views::filter(m_DynamicBoxes,
		[IncludedObjectTypes, &ExtendedBox](const std::shared_ptr<IBoxColliderComponent>& IBoxColliderComponent)
		{
			return (IBoxColliderComponent->GetCollisionObjectType() & IncludedObjectTypes) == IBoxColliderComponent->GetCollisionObjectType() &&
				IBoxColliderComponent->GetBox().m_Min.x < ExtendedBox.m_Max.x &&
				Intersect(ExtendedBox, IBoxColliderComponent->GetBox());
		});

	for (const auto& Collider : RangeDynamic)
	{
		OutIntersections.emplace_back(Collider);
	}

	std::ranges::sort(OutIntersections,
		[&FromPosition](const std::shared_ptr<IBoxColliderComponent>& a, const std::shared_ptr<IBoxColliderComponent>& b)
		{
			return (FromPosition - a->GetOwner()->GetPosition()).LengthSq() < (FromPosition - b->GetOwner()->GetPosition()).LengthSq();
		});

	return OutIntersections.begin() != OutIntersections.end();
}

bool CollisionWorld::SingleBoxCast(const Vector2Int& FromPosition, const AABB& FromBox, const Vector2Int& ExtentsOffset, std::shared_ptr<IBoxColliderComponent>& Intersection, CollisionFlags IncludedObjectTypes)
{
	std::vector<std::shared_ptr<IBoxColliderComponent>> OutIntersections;
	if(MultiBoxCast(FromPosition, FromBox, ExtentsOffset, OutIntersections, IncludedObjectTypes))
	{
		Intersection = OutIntersections.front();
		return true;
	}
	
	return false;
}

void CollisionWorld::AddBox(const std::shared_ptr<IBoxColliderComponent>& Box)
{
	if (Box->GetOwner()->GetTransformType() == TransformType::Static)
	{
		m_StaticBoxes.emplace_back(Box);
	}
	else
	{
		m_DynamicBoxes.emplace_back(Box);
	}
}

void CollisionWorld::RemoveBox(const std::shared_ptr<IBoxColliderComponent>& Box)
{
	if (Box->GetOwner()->GetTransformType() == TransformType::Static)
	{
		const auto It = std::ranges::find(m_StaticBoxes, Box);
		if(It != m_StaticBoxes.end())
		{
			std::iter_swap(It, m_StaticBoxes.end() - 1);
			m_StaticBoxes.pop_back();
		}
	}
	else
	{
		const auto It = std::ranges::find(m_DynamicBoxes, Box);
		if (It != m_DynamicBoxes.end())
		{
			std::iter_swap(It, m_StaticBoxes.end() - 1);
			m_DynamicBoxes.pop_back();
		}
	}
}

void CollisionWorld::OnEntitiesCollision(const std::shared_ptr<IBoxColliderComponent>& A, const std::shared_ptr<IBoxColliderComponent>& B)
{
	A->GetOwner()->OnCollision(B);
	B->GetOwner()->OnCollision(A);
}
