#pragma once

#include "d3dStd.h"

class LineSegment;

class PointList
{
public:
	PointList();
	explicit PointList(const std::vector<XMFLOAT3>& list);
	~PointList();

	void addPoint(const XMFLOAT3& point);
	void addPoint(const XMVECTOR& point);
	
	void clear();

	void setPoints(const std::vector<XMFLOAT3>& points);
	void setPoints(const std::list<XMFLOAT3>& points);
	void setPoints(const std::vector<XMVECTOR>& points);
	void setLine(const LineSegment& line);
	void addLine(const LineSegment& line);

	PointList& operator+=(const LineSegment& line);
	PointList& operator+=(const XMFLOAT3& point);

	std::vector<XMFLOAT3> List;
};