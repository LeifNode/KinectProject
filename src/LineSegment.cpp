#include "LineSegment.h"

LineSegment::LineSegment()
{
}

LineSegment::LineSegment(float x1, float y1, float z1, float x2, float y2, float z2)
	:Point1(x1, y1, z1),
	Point2(x2, y2, z2)
{
}

LineSegment::LineSegment(const float* point1, const float* point2)
	:Point1(point1),
	Point2(point2)
{
}

LineSegment::LineSegment(const XMFLOAT3& point1, const XMFLOAT3& point2)
	:Point1(point1),
	Point2(point2)
{
}

LineSegment::LineSegment(const XMVECTOR& point1, const XMVECTOR& point2)
{
	XMStoreFloat3(&Point1, point1);
	XMStoreFloat3(&Point2, point2);
}

LineSegment::~LineSegment()
{
}