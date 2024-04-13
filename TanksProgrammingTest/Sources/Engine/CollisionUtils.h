// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// ----------------------------------------------------------------

#pragma once
#include "MathLib.h"
#include <vector>

struct LineSegment
{
	LineSegment(const Vector2& start, const Vector2& end);
	// Get point along segment where 0 <= t <= 1
	Vector2 PointOnSegment(float t) const;
	// Get minimum distance squared between point and line segment
	//float MinDistSq(const Vector2& point) const;
	// Get MinDistSq between two line segments
	//static float MinDistSq(const LineSegment& s1, const LineSegment& s2);

	Vector2 m_Start;
	Vector2 m_End;
};

struct AABB
{
	AABB(const Vector2& min, const Vector2& max);
	// Update min and max accounting for this point
	// (used when loading a model)
	void UpdateMinMax(const Vector2& point);
	// Rotated by a quaternion
	//void Rotate(const Quaternion& q);//todo add simple rotation
	bool Contains(const Vector2& point) const;
	float MinDistSq(const Vector2& point) const;

	Vector2 m_Min;
	Vector2 m_Max;
};

bool Intersect(const AABB& a, const AABB& b);
bool TryGetBoxCollisionDelta(const AABB& forBox, const AABB& awayFromBox, Vector2& delta);
bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector2& outNorm);