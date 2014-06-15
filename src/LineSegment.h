#pragma once

#include "d3dStd.h"

class LineSegment
{
public:
	LineSegment();
	LineSegment(float x1, float y1, float z1, float x2, float y2, float z2);
	LineSegment(const float* point1Arr, const float* point2Arr);
	LineSegment(const XMFLOAT3& point1, const XMFLOAT3& point2);
	LineSegment(const XMVECTOR& point1, const XMVECTOR& point2);
	~LineSegment();

	XMVECTOR point1Vec() const { return XMLoadFloat3(&Point1); }
	XMVECTOR point2Vec() const { return XMLoadFloat3(&Point2); }

	XMFLOAT3 Point1;
	XMFLOAT3 Point2;
};