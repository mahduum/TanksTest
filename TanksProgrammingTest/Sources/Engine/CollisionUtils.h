// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// ----------------------------------------------------------------

#pragma once
#include "MathLib.h"

struct LineSegment
{
	LineSegment(const Vector2Int& start, const Vector2Int& end);
	// Get point along segment where 0 <= t <= 1
	Vector2Int PointOnSegment(float t) const;
	// Get minimum distance squared between point and line segment
	//float MinDistSq(const Vector2Int& point) const;
	// Get MinDistSq between two line segments
	//static float MinDistSq(const LineSegment& s1, const LineSegment& s2);

	Vector2Int m_Start;
	Vector2Int m_End;
};

struct AABB
{
	AABB(const Vector2Int& min, const Vector2Int& max);
	void UpdateMinMax(const Vector2Int& point);

	bool Contains(const Vector2Int& point) const;//todo make templated for generic vector
	int MinDistSq(const Vector2Int& point) const;//todo make templated for generic vector

	Vector2Int m_Min;
	Vector2Int m_Max;
};

bool Intersect(const AABB& a, const AABB& b);
bool TryGetBoxCollisionDelta(const AABB& forBox, const AABB& awayFromBox, Vector2Int& delta);
bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector2Int& outNorm);