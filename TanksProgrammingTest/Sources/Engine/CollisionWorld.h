#pragma once
#include <functional>
#include <memory>

#include "MathLib.h"

class BoxColliderComponent;
//struct Vector2
//{
//	int x;
//	int y;
//};
struct CollisionInfo
{
	class Entity* m_OhterEntity;
	// Point of collision
	//Vector2 m_Point;
	// Component collided with
	//class BoxColliderComponent* m_OtherCollider;

	//todo backtrace collision delta
	// Owning actor of component
};

class CollisionWorld
{
public:

	CollisionWorld() = default;

	// Test a line segment against boxes
	// Returns true if it collides against a box
	//todo bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	// Tests collisions using naive pairwise
	void TestPairwise(std::function<void(class Entity*, class Actor*)> f);
	// Test collisions using sweep and prune
	void TestSweepAndPrune(std::function<void(class Entity*, class Entity*)>& f);

	// Add/remove box components from world
	void AddBox(const std::shared_ptr<BoxColliderComponent>& box);
	void RemoveBox(const std::shared_ptr<BoxColliderComponent>& box);

	size_t GetBoxesCount() const { return m_Boxes.size(); }

	static void OnEntitiesCollision(class Entity* a, class Entity* b);

	std::function<void(class Entity*, class Entity*)> CollisionHandler{ &CollisionWorld::OnEntitiesCollision };


	const std::vector<std::shared_ptr<BoxColliderComponent>>& GetBoxes() const
	{
		return m_Boxes;
	}

private:

	std::vector<std::shared_ptr<BoxColliderComponent>> m_Boxes;
};

