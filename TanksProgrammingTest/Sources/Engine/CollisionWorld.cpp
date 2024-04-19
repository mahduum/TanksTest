#include "CollisionWorld.h"

#include <algorithm>
#include <ranges>
#include "CollisionUtils.h"
#include "Entity.h"
#include "../Game/BoxColliderComponent.h"

void CollisionWorld::TestSweepAndPrune(std::function<void(Entity*, Entity*)>& f)
{
	std::sort(m_StaticBoxes.begin(), m_StaticBoxes.end(),
		[](const std::shared_ptr<BoxColliderComponent>& a, const std::shared_ptr<BoxColliderComponent>& b)
		{
			return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
		});

	std::sort(m_DynamicBoxes.begin(), m_DynamicBoxes.end(),
		[](const std::shared_ptr<BoxColliderComponent>& a, const std::shared_ptr<BoxColliderComponent>& b)
		{
			return a->GetBox().m_Min.x < b->GetBox().m_Min.x;
		});

	for (size_t i = 0; i < m_DynamicBoxes.size(); i++)
	{
		auto a = m_DynamicBoxes[i];
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
				if(a->GetOwner() == nullptr || b->GetOwner() == nullptr)
				{
					SDL_Log("Found null pointers in colliders!");

					return;
				}
					//SDL_Log("Collision between: %s and %s", a->GetOwner()->GetName().data(), b->GetOwner()->GetName().data());
				f(a->GetOwner(), b->GetOwner());
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
				if (a->GetOwner() == nullptr || b->GetOwner() == nullptr)
				{
					SDL_Log("Found null pointers in colliders!");

					return;
				}
				//SDL_Log("Collision between: %s and %s", a->GetOwner()->GetName().data(), b->GetOwner()->GetName().data());
				f(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

//add filter with tag/channel
bool CollisionWorld::MultiBoxCast(const Vector2& FromPosition, const AABB& FromBox, const Vector2& ExtentsOffset, std::vector<std::shared_ptr<BoxColliderComponent>>& OutIntersections, CollisionObjectType IncludedObjectTypes)
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



bool CollisionWorld::SingleBoxCast(const Vector2& FromPosition, const AABB& FromBox, const Vector2& ExtentsOffset, std::shared_ptr<BoxColliderComponent>& Intersection, CollisionObjectType IncludedObjectTypes)
{
	std::vector<std::shared_ptr<BoxColliderComponent>> OutIntersections;
	if(MultiBoxCast(FromPosition, FromBox, ExtentsOffset, OutIntersections, IncludedObjectTypes))
	{
		Intersection = OutIntersections.front();
		return true;
	}
	
	return false;
}

void CollisionWorld::AddBox(const std::shared_ptr<BoxColliderComponent> box)
{
	if (box->GetOwner()->GetTransformType() == TransformType::Static)
	{
		m_StaticBoxes.emplace_back(box);
	}
	else
	{
		m_DynamicBoxes.emplace_back(box);
	}
}

void CollisionWorld::RemoveBox(const std::shared_ptr<BoxColliderComponent> box)
{
	if (box->GetOwner()->GetTransformType() == TransformType::Static)
	{
		if (const auto it = std::find(m_StaticBoxes.begin(), m_StaticBoxes.end(), box); it != m_StaticBoxes.end())
		{
			std::iter_swap(it, m_StaticBoxes.end() - 1);
			m_StaticBoxes.pop_back();
		}
	}
	else
	{
		if (const auto it = std::find(m_DynamicBoxes.begin(), m_DynamicBoxes.end(), box); it != m_DynamicBoxes.end())
		{
			std::iter_swap(it, m_StaticBoxes.end() - 1);
			m_DynamicBoxes.pop_back();
		}
	}
}

void CollisionWorld::OnEntitiesCollision(Entity* a, Entity* b)
{
	CollisionInfo aCollisionInfo
	{
		std::make_shared<Entity>(*b)
	};
	CollisionInfo bCollisionInfo
	{
		std::make_shared<Entity>(*a)
	};
	a->OnCollision(aCollisionInfo);
	b->OnCollision(bCollisionInfo);
}
