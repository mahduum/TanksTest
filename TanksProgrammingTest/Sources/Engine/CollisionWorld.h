#pragma once
#include <functional>
#include <memory>
#include <ranges>
#include <string>

#include "CollisionUtils.h"
#include "MathLib.h"

class Entity;

enum class CollisionObjectType : unsigned int
{
	None = 0,
	Player = 1 << 0,
	Enemy = 1 << 1,
	Projectile = 1 << 2,
	WorldStatic = 1 << 3,
	WorldDynamic = 1 << 4,
	All = ~static_cast<unsigned int>(0)
};

inline CollisionObjectType operator|(CollisionObjectType lhs, CollisionObjectType rhs)
{
	return static_cast<CollisionObjectType>(
		static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs)
		);
}

inline CollisionObjectType operator&(CollisionObjectType lhs, CollisionObjectType rhs)
{
	return static_cast<CollisionObjectType>(
		static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs)
		);
}

inline CollisionObjectType operator^(CollisionObjectType lhs, CollisionObjectType rhs)
{
	return static_cast<CollisionObjectType>(
		static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs)
		);
}

inline CollisionObjectType operator~(CollisionObjectType obj) {
	return static_cast<CollisionObjectType>(~static_cast<unsigned int>(obj));
}

inline CollisionObjectType& operator|=(CollisionObjectType& lhs, CollisionObjectType rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

inline CollisionObjectType& operator&=(CollisionObjectType& lhs, CollisionObjectType rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

inline CollisionObjectType& operator^=(CollisionObjectType& lhs, CollisionObjectType rhs)
{
	lhs = lhs ^ rhs;
	return lhs;
}

inline bool operator==(CollisionObjectType lhs, CollisionObjectType rhs)
{
	return static_cast<unsigned int>(lhs) == static_cast<unsigned int>(rhs);
}


inline CollisionObjectType StringToCollisionObjectType(const std::string&& EnumName)
{
	if (EnumName == "Player")
		return CollisionObjectType::Player;
	if (EnumName == "Enemy")
		return CollisionObjectType::Enemy;
	if (EnumName == "Projectile")
		return CollisionObjectType::Projectile;
	if (EnumName == "WorldStatic")
		return CollisionObjectType::WorldStatic;
	if (EnumName == "WorldDynamic")
		return CollisionObjectType::WorldDynamic;
	if (EnumName == "All")
		return CollisionObjectType::All;

	return CollisionObjectType::None;
}

class BoxColliderComponent;
struct CollisionInfo
{
	class std::shared_ptr<Entity> m_OtherEntity;//todo shared
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
	bool MultiBoxCast(
		const Vector2& FromPosition,
		const AABB& FromBox, const Vector2& Direction, std::vector<std::shared_ptr<BoxColliderComponent>>& OutIntersections,
		CollisionObjectType IncludedObjectTypes = CollisionObjectType::All);
	bool SingleBoxCast(const Vector2& FromPosition, const AABB& FromBox, const Vector2& Direction, std::shared_ptr<BoxColliderComponent>& Intersection, CollisionObjectType IncludedObjectTypes) ;

	// Add/remove box components from world
	void AddBox(const std::shared_ptr<BoxColliderComponent> box);
	void RemoveBox(const std::shared_ptr<BoxColliderComponent> box);

	size_t GetBoxesCount() const { return m_StaticBoxes.size(); }

	static void OnEntitiesCollision(class Entity* a, class Entity* b);

	std::function<void(class Entity*, class Entity*)> CollisionHandler{ &CollisionWorld::OnEntitiesCollision };


	const std::vector<std::shared_ptr<BoxColliderComponent>>& GetStaticBoxes() const
	{
		return m_StaticBoxes;
	}

private:

	std::vector<std::shared_ptr<BoxColliderComponent>> m_StaticBoxes;
	std::vector<std::shared_ptr<BoxColliderComponent>> m_DynamicBoxes;
};

