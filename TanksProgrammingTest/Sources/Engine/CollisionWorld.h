#pragma once
#include <functional>
#include <memory>
#include <ranges>
#include <string>

#include "CollisionUtils.h"
#include "MathLib.h"

class IBoxColliderComponent;
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

class IBoxColliderComponent;

class CollisionWorld
{
public:

	CollisionWorld() = default;

	void TestSweepAndPrune(const std::function<void(const std::shared_ptr <IBoxColliderComponent>, const std::shared_ptr <IBoxColliderComponent>)>& f);
	bool MultiBoxCast(
		const Vector2Int& FromPosition,
		const AABB& FromBox, const Vector2Int& Direction, std::vector<std::shared_ptr<IBoxColliderComponent>>& OutIntersections,
		CollisionFlags IncludedObjectTypes);
	bool SingleBoxCast(const Vector2Int& FromPosition, const AABB& FromBox, const Vector2Int& Direction, std::shared_ptr<IBoxColliderComponent>& Intersection, CollisionFlags IncludedObjectTypes = CollisionFlags::All);

	void AddBox(const std::shared_ptr<IBoxColliderComponent>& Box);

	static void OnEntitiesCollision(const std::shared_ptr<IBoxColliderComponent>& A, const std::shared_ptr<IBoxColliderComponent>&);
	std::function<void(std::shared_ptr <IBoxColliderComponent>, std::shared_ptr <IBoxColliderComponent>)> CollisionHandler{ &CollisionWorld::OnEntitiesCollision };

	const std::vector<std::shared_ptr<IBoxColliderComponent>>& GetStaticBoxes() const
	{
		return m_StaticBoxes;
	}

private:
	/*Removing box from outside must be scheduled, do not remove directly*/
	void RemoveBox(const std::shared_ptr<IBoxColliderComponent>& Box);

	std::vector<std::shared_ptr<IBoxColliderComponent>> m_StaticBoxes;
	std::vector<std::shared_ptr<IBoxColliderComponent>> m_DynamicBoxes;
};

