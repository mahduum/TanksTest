#pragma once
#include <functional>

struct Vector2
{
	int x;
	int y;
};

class CollisionWorld
{
public:

	CollisionWorld() = default;

	// Used to give helpful information about collision results
	struct CollisionInfo
	{
		// Point of collision
		Vector2 m_Point;
		// Component collided with
		class BoxColliderComponent* m_Box;
		// Owning actor of component
		class Entity* m_Entity;
	};

	// Test a line segment against boxes
	// Returns true if it collides against a box
	//todo bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	// Tests collisions using naive pairwise
	void TestPairwise(std::function<void(class Entity*, class Actor*)> f);
	// Test collisions using sweep and prune
	void TestSweepAndPrune(std::function<void(class Entity*, class Entity*)> f);

	// Add/remove box components from world
	void AddBox(class BoxColliderComponent* box);
	void RemoveBox(class BoxColliderComponent* box);

	const std::vector<BoxColliderComponent*>& GetBoxes() const
	{
		return m_Boxes;
	}

private:

	std::vector<BoxColliderComponent*> m_Boxes;
};

