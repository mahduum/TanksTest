// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// ----------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable : 4244)

#include "CollisionUtils.h"

#include <algorithm>
#include <vector>


LineSegment::LineSegment(const Vector2Int& start, const Vector2Int& end) :
	m_Start(start),
	m_End(end)
{
}

Vector2Int LineSegment::PointOnSegment(float t) const
{
	return m_Start + (m_End - m_Start) * t;
}

AABB::AABB(const Vector2Int& min, const Vector2Int& max)
	: m_Min(min)
	, m_Max(max)
{
}

void AABB::UpdateMinMax(const Vector2Int& point)
{
	// Update each component separately
	m_Min.x = MathLib::Min(m_Min.x, point.x);
	m_Min.y = MathLib::Min(m_Min.y, point.y);

	m_Max.x = MathLib::Max(m_Max.x, point.x);
	m_Max.y = MathLib::Max(m_Max.y, point.y);
}

bool AABB::Contains(const Vector2Int& point) const
{
	bool outside =
		point.x < m_Min.x ||
		point.y < m_Min.y ||
		point.x > m_Max.x ||
		point.y > m_Max.y;
	// If none of these are true, the point is inside the box
	return !outside;
}

int AABB::MinDistSq(const Vector2Int& point) const
{
	// Compute differences for each axis
	int dx = MathLib::Max(m_Min.x - point.x, 0);
	dx = MathLib::Max(dx, point.x - m_Max.x);
	int dy = MathLib::Max(m_Min.y - point.y, 0);
	dy = MathLib::Max(dy, point.y - m_Max.y);
	// Distance squared formula
	return dx * dx + dy * dy;
}

bool Intersect(const AABB& a, const AABB& b)
{
	bool no =
		a.m_Max.x < b.m_Min.x ||
		a.m_Max.y < b.m_Min.y ||
		b.m_Max.x < a.m_Min.x ||
		b.m_Max.y < a.m_Min.y;
	// If none of these are true, they must intersect
	return !no;
}

bool TryGetBoxCollisionDelta(const AABB& forBox, const AABB& awayFromBox, Vector2Int& delta)
{
	if (Intersect(forBox, awayFromBox) == false)
	{
		return false;
	}

	const int dx1 = forBox.m_Max.x - awayFromBox.m_Min.x;
	const int dx2 = forBox.m_Min.x - awayFromBox.m_Max.x;
	const int dy1 = forBox.m_Max.y - awayFromBox.m_Min.y;
	const int dy2 = forBox.m_Min.y - awayFromBox.m_Max.y;

	// Set dx to whichever of dx1/dx2 have a lower abs
	const int dx = MathLib::Abs(dx1) < MathLib::Abs(dx2) ?
		                 dx1 : dx2;

	const int dy = MathLib::Abs(dy1) < MathLib::Abs(dy2) ?
		                 dy1 : dy2;

	// Whichever is closest, adjust x/y position
	if (MathLib::Abs(dx) <= MathLib::Abs(dy))
	{
		delta.x = dx;
		delta.y = 0;
	}
	else
	{
		delta.x = 0;
		delta.y = dy;
	}

	return true;
}


bool TestSidePlane(float start, float end, float negated_distance, const Vector2Int& norm,
                   std::vector<std::pair<float, Vector2Int>>& out)
{
	float denominator = end - start;
	if (MathLib::NearZero(denominator))
	{
		return false;
	}

	float numerator = -start + negated_distance;
	float t = numerator / denominator;
	// Test that t is within bounds
	if (t >= 0.0f && t <= 1.0f)
	{
		out.emplace_back(t, norm);
		return true;
	}

	return false;
	
}

bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector2Int& outNorm)
{
	// Vector to save all possible t values, and normals for those sides
	std::vector<std::pair<float, Vector2Int>> tValues;
	// Test the x planes
	TestSidePlane(l.m_Start.x, l.m_End.x, b.m_Min.x, Vector2Int::Left,
		tValues);
	TestSidePlane(l.m_Start.x, l.m_End.x, b.m_Max.x, Vector2Int::Right,
		tValues);
	// Test the y planes
	TestSidePlane(l.m_Start.y, l.m_End.y, b.m_Min.y, Vector2Int::Up,
		tValues);
	TestSidePlane(l.m_Start.y, l.m_End.y, b.m_Max.y, Vector2Int::Down,
		tValues);

	// Sort the t values in ascending order
	std::ranges::sort(tValues, [](
	                  const std::pair<float, Vector2Int>& a,
	                  const std::pair<float, Vector2Int>& b) {
		                  return a.first < b.first;
	                  });
	// Test if the box contains any of these points of intersection
	for (auto& t : tValues)
	{
		Vector2Int point = l.PointOnSegment(t.first);
		if (b.Contains(point))
		{
			outT = t.first;
			outNorm = t.second;
			return true;
		}
	}

	//None of the intersections are within bounds of box
	return false;
}

#pragma warning(pop)