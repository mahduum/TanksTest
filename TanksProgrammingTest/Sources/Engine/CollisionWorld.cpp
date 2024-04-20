#include "CollisionWorld.h"

#include <algorithm>
#include <ranges>
#include "CollisionUtils.h"
#include "Entity.h"
#include "../Game/BoxColliderComponent.h"

void CollisionWorld::TestSweepAndPrune(const std::function<void(std::shared_ptr<ColliderComponent>, std::shared_ptr<ColliderComponent>)>& f)
{
	std::ranges::sort(m_StaticBoxes,
	                  [](const std::shared_ptr<BoxColliderComponent>& a, const std::shared_ptr<BoxColliderComponent>& b)
	                  {
		                  return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
	                  });

	std::ranges::sort(m_DynamicBoxes,
	                  [](const std::shared_ptr<BoxColliderComponent>& a, const std::shared_ptr<BoxColliderComponent>& b)
	                  {
		                  return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
	                  });

	for (auto a : m_DynamicBoxes)
	{
		float max = a->GetBox().m_Max.x;
		for (size_t j = 0; j < m_StaticBoxes.size(); j++)
		{
			auto b = m_StaticBoxes[j];
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
		float max = a->GetBox().m_Max.x;
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

	std::erase_if(m_DynamicBoxes, [](const std::shared_ptr<BoxColliderComponent>& b)
		{
			return b->GetOwner()->IsAlive == false;//todo optimize, call from scene
		});
}

bool CollisionWorld::MultiBoxCast(const Vector2& FromPosition, const AABB& FromBox, const Vector2& ExtentsOffset, std::vector<std::shared_ptr<BoxColliderComponent>>& OutIntersections, CollisionFlags IncludedObjectTypes)
{
	AABB ExtendedBox(FromBox.m_Min, FromBox.m_Max);
	ExtendedBox.UpdateMinMax(FromBox.m_Min + ExtentsOffset);
	ExtendedBox.UpdateMinMax(FromBox.m_Max + ExtentsOffset);

	std::ranges::sort(m_StaticBoxes,
		[](const std::shared_ptr<BoxColliderComponent>& a, const std::shared_ptr<BoxColliderComponent>& b)
		{
		    return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
		});

	auto RangeStatic = std::ranges::views::filter(m_StaticBoxes,
		[IncludedObjectTypes, &ExtendedBox](const std::shared_ptr<BoxColliderComponent>& BoxColliderComponent)
		{
			return (BoxColliderComponent->GetCollisionObjectType() & IncludedObjectTypes) == BoxColliderComponent->GetCollisionObjectType() &&
				BoxColliderComponent->GetBox().m_Min.x < ExtendedBox.m_Max.x &&
				Intersect(ExtendedBox, BoxColliderComponent->GetBox());
		});

	for (const auto& Collider : RangeStatic)
	{
		OutIntersections.emplace_back(Collider);
	}

	std::ranges::sort(m_DynamicBoxes,
		[](const std::shared_ptr<BoxColliderComponent>& a, const std::shared_ptr<BoxColliderComponent>& b)
		{
			return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
		});

	auto RangeDynamic = std::ranges::views::filter(m_DynamicBoxes,
		[IncludedObjectTypes, &ExtendedBox](const std::shared_ptr<BoxColliderComponent>& BoxColliderComponent)
		{
			return (BoxColliderComponent->GetCollisionObjectType() & IncludedObjectTypes) == BoxColliderComponent->GetCollisionObjectType() &&
				BoxColliderComponent->GetBox().m_Min.x < ExtendedBox.m_Max.x &&
				Intersect(ExtendedBox, BoxColliderComponent->GetBox());
		});

	for (const auto& Collider : RangeDynamic)
	{
		OutIntersections.emplace_back(Collider);
	}

	std::ranges::sort(OutIntersections,
		[&FromPosition](const std::shared_ptr<BoxColliderComponent>& a, const std::shared_ptr<BoxColliderComponent>& b)
		{
			return (FromPosition - a->GetOwner()->GetPosition()).LengthSq() < (FromPosition - b->GetOwner()->GetPosition()).LengthSq();
		});

	return OutIntersections.begin() != OutIntersections.end();
}

bool CollisionWorld::SingleBoxCast(const Vector2& FromPosition, const AABB& FromBox, const Vector2& ExtentsOffset, std::shared_ptr<BoxColliderComponent>& Intersection, CollisionFlags IncludedObjectTypes)
{
	std::vector<std::shared_ptr<BoxColliderComponent>> OutIntersections;
	if(MultiBoxCast(FromPosition, FromBox, ExtentsOffset, OutIntersections, IncludedObjectTypes))
	{
		Intersection = OutIntersections.front();
		return true;
	}
	
	return false;
}

void CollisionWorld::AddBox(const std::shared_ptr<BoxColliderComponent>& Box)
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

void CollisionWorld::RemoveBox(BoxColliderComponent* box)//todo refactor back to shared
{
	if (box->GetOwner()->GetTransformType() == TransformType::Static)
	{
		if (const auto it =
			std::ranges::find_if(m_StaticBoxes,
			                     [box](const std::shared_ptr<BoxColliderComponent>& elem)
			                     {
				                     return box == elem.get();
			                     }); it != m_StaticBoxes.end())
		{
			std::iter_swap(it, m_StaticBoxes.end() - 1);
			m_StaticBoxes.pop_back();
		}
	}
	else
	{
		//std::erase_if(m_DynamicBoxes, [box](const std::shared_ptr<BoxColliderComponent>& b)
		//{
		//	return b.get() == box;
		//});
		if (const auto it =
			std::ranges::find_if(m_DynamicBoxes,
			                     [box](const std::shared_ptr<BoxColliderComponent>& elem)
			                     {
				                     return box == elem.get();
			                     }); it != m_DynamicBoxes.end())
		{
			std::iter_swap(it, m_DynamicBoxes.end() - 1);
			m_DynamicBoxes.pop_back();
		}
	}
}

void CollisionWorld::OnEntitiesCollision(const std::shared_ptr<ColliderComponent>& A, const std::shared_ptr<ColliderComponent>& B)
{
	A->GetOwner()->OnCollision(B);
	B->GetOwner()->OnCollision(A);
}
