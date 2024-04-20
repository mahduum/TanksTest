#pragma once
#include <functional>
#include <memory>
#include <ranges>
#include <string>

#include "CollisionUtils.h"
#include "MathLib.h"

class ColliderComponent;
class Entity;

enum class CollisionFlags : unsigned int
{
	None = 0,
	Player = 1 << 0,
	Enemy = 1 << 1,
	Projectile = 1 << 2,
	WorldStatic = 1 << 3,
	WorldDynamic = 1 << 4,
	All = ~0u,
};

inline CollisionFlags operator|(CollisionFlags lhs, CollisionFlags rhs)
{
	return static_cast<CollisionFlags>(
		static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs)
		);
}

inline CollisionFlags operator&(CollisionFlags lhs, CollisionFlags rhs)
{
	return static_cast<CollisionFlags>(
		static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs)
		);
}

inline CollisionFlags operator^(CollisionFlags lhs, CollisionFlags rhs)
{
	return static_cast<CollisionFlags>(
		static_cast<unsigned int>(lhs) ^ static_cast<unsigned int>(rhs)
		);
}

inline CollisionFlags operator~(CollisionFlags obj) {
	return static_cast<CollisionFlags>(~static_cast<unsigned int>(obj));
}

inline CollisionFlags& operator|=(CollisionFlags& lhs, CollisionFlags rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

inline CollisionFlags& operator&=(CollisionFlags& lhs, CollisionFlags rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

inline CollisionFlags& operator^=(CollisionFlags& lhs, CollisionFlags rhs)
{
	lhs = lhs ^ rhs;
	return lhs;
}

inline bool operator==(CollisionFlags lhs, CollisionFlags rhs)
{
	return static_cast<unsigned int>(lhs) == static_cast<unsigned int>(rhs);
}


inline CollisionFlags StringToCollisionObjectType(const std::string& EnumName)
{
	if (EnumName == "Player")
		return CollisionFlags::Player;
	if (EnumName == "Enemy")
		return CollisionFlags::Enemy;
	if (EnumName == "Projectile")
		return CollisionFlags::Projectile;
	if (EnumName == "WorldStatic")
		return CollisionFlags::WorldStatic;
	if (EnumName == "WorldDynamic")
		return CollisionFlags::WorldDynamic;
	if (EnumName == "All")
		return CollisionFlags::All;

	return CollisionFlags::None;
}

class BoxColliderComponent;
struct CollisionInfo
{

	std::weak_ptr<ColliderComponent> m_OtherCollider;
};

class CollisionWorld
{
public:

	CollisionWorld() = default;

	void TestSweepAndPrune(const std::function<void(const std::shared_ptr <ColliderComponent>, const std::shared_ptr <ColliderComponent>)>& f);
	bool MultiBoxCast(
		const Vector2& FromPosition,
		const AABB& FromBox, const Vector2& Direction, std::vector<std::shared_ptr<BoxColliderComponent>>& OutIntersections,
		CollisionFlags IncludedObjectTypes);
	bool SingleBoxCast(const Vector2& FromPosition, const AABB& FromBox, const Vector2& Direction, std::shared_ptr<BoxColliderComponent>& Intersection, CollisionFlags IncludedObjectTypes = CollisionFlags::All) ;

	// Add/remove Box components from world
	void AddBox(const std::shared_ptr<BoxColliderComponent>& Box);
	void RemoveBox(BoxColliderComponent* box);

	size_t GetBoxesCount() const { return m_StaticBoxes.size() + m_DynamicBoxes.size(); }

	static void OnEntitiesCollision(const std::shared_ptr<ColliderComponent>& A, const std::shared_ptr<ColliderComponent>
	                                &);

	std::function<void(std::shared_ptr <ColliderComponent>, std::shared_ptr <ColliderComponent>)> CollisionHandler{ &CollisionWorld::OnEntitiesCollision };


	const std::vector<std::shared_ptr<BoxColliderComponent>>& GetStaticBoxes() const
	{
		return m_StaticBoxes;
	}

private:

	std::vector<std::shared_ptr<BoxColliderComponent>> m_StaticBoxes;
	std::vector<std::shared_ptr<BoxColliderComponent>> m_DynamicBoxes;
};

