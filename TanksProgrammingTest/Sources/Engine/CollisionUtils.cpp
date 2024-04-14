#include "CollisionUtils.h"

#include <algorithm>


LineSegment::LineSegment(const Vector2& start, const Vector2& end)
	:m_Start(start)
	, m_End(end)
{
}

Vector2 LineSegment::PointOnSegment(float t) const
{
	return m_Start + (m_End - m_Start) * t;
}

AABB::AABB(const Vector2& min, const Vector2& max)
	: m_Min(min)
	, m_Max(max)
{
}

void AABB::UpdateMinMax(const Vector2& point)
{
	// Update each component separately
	m_Min.x = MathLib::Min(m_Min.x, point.x);
	m_Min.y = MathLib::Min(m_Min.y, point.y);

	m_Max.x = MathLib::Max(m_Max.x, point.x);
	m_Max.y = MathLib::Max(m_Max.y, point.y);
}

bool AABB::Contains(const Vector2& point) const
{
	bool outside =
		point.x < m_Min.x ||
		point.y < m_Min.y ||
		point.x > m_Max.x ||
		point.y > m_Max.y;
	// If none of these are true, the point is inside the box
	return !outside;
}

float AABB::MinDistSq(const Vector2& point) const
{
	// Compute differences for each axis
	float dx = MathLib::Max(m_Min.x - point.x, 0.0f);
	dx = MathLib::Max(dx, point.x - m_Max.x);
	float dy = MathLib::Max(m_Min.y - point.y, 0.0f);
	dy = MathLib::Max(dy, point.y - m_Max.y);
	// Distance squared formula
	return dx * dx + dy * dy;
}
//
//void AABB::Rotate(const Quaternion& q)
//{
//	// Construct the 8 points for the corners of the box
//	std::array<Vector3, 8> points;
//	// Min point is always a corner
//	points[0] = m_Min;
//	// Permutations with 2 min and 1 max
//	points[1] = Vector3(m_Max.x, m_Min.y, m_Min.z);
//	points[2] = Vector3(m_Min.x, m_Max.y, m_Min.z);
//	points[3] = Vector3(m_Min.x, m_Min.y, m_Max.z);
//	// Permutations with 2 max and 1 min
//	points[4] = Vector3(m_Min.x, m_Max.y, m_Max.z);
//	points[5] = Vector3(m_Max.x, m_Min.y, m_Max.z);
//	points[6] = Vector3(m_Max.x, m_Max.y, m_Min.z);
//	// Max point corner
//	points[7] = Vector3(m_Max);
//
//	// Rotate first point
//	Vector3 p = Vector3::Transform(points[0], q);
//	// Reset min/max to first point rotated
//	m_Min = p;
//	m_Max = p;
//	// Update min/max based on remaining points, rotated
//	for (size_t i = 1; i < points.size(); i++)
//	{
//		p = Vector3::Transform(points[i], q);
//		UpdateMinMax(p);
//	}
//}

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

bool TryGetBoxCollisionDelta(const AABB& forBox, const AABB& awayFromBox, Vector2& delta)
{
	if (Intersect(forBox, awayFromBox) == false)
	{
		return false;
	}

	const float dx1 = forBox.m_Max.x - awayFromBox.m_Min.x;
	const float dx2 = forBox.m_Min.x - awayFromBox.m_Max.x;
	const float dy1 = forBox.m_Max.y - awayFromBox.m_Min.y;
	const float dy2 = forBox.m_Min.y - awayFromBox.m_Max.y;

	// Set dx to whichever of dx1/dx2 have a lower abs
	const float dx = MathLib::Abs(dx1) < MathLib::Abs(dx2) ?
		                 dx1 : dx2;

	const float dy = MathLib::Abs(dy1) < MathLib::Abs(dy2) ?
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


bool TestSidePlane(float start, float end, float negated_distance, const Vector2& norm,
                   std::vector<std::pair<float, Vector2>>& out)
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
	Vector2& outNorm)
{
	// Vector to save all possible t values, and normals for those sides
	std::vector<std::pair<float, Vector2>> tValues;
	// Test the x planes
	TestSidePlane(l.m_Start.x, l.m_End.x, b.m_Min.x, Vector2::Left,
		tValues);
	TestSidePlane(l.m_Start.x, l.m_End.x, b.m_Max.x, Vector2::Right,
		tValues);
	// Test the y planes
	TestSidePlane(l.m_Start.y, l.m_End.y, b.m_Min.y, Vector2::Up,
		tValues);
	TestSidePlane(l.m_Start.y, l.m_End.y, b.m_Max.y, Vector2::Down,
		tValues);

	// Sort the t values in ascending order
	std::sort(tValues.begin(), tValues.end(), [](
		const std::pair<float, Vector2>& a,
		const std::pair<float, Vector2>& b) {
			return a.first < b.first;
		});
	// Test if the box contains any of these points of intersection
	for (auto& t : tValues)
	{
		Vector2 point = l.PointOnSegment(t.first);
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