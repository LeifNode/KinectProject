#include "PointList.h"
#include "LineSegment.h"

PointList::PointList()
{
}

PointList::PointList(const std::vector<XMFLOAT3>& list)
{
	setPoints(list);
}

PointList::~PointList()
{
}

void PointList::addPoint(const XMFLOAT3& point)
{
	List.push_back(point);
}

void PointList::addPoint(const XMVECTOR& point)
{
	XMFLOAT3 newPoint;
	XMStoreFloat3(&newPoint, point);

	List.push_back(newPoint);
}

void PointList::clear()
{
	List.clear();
}

void PointList::setPoints(const std::vector<XMFLOAT3>& points)
{
	List.clear();

	for (auto it = points.begin(); it != points.end(); ++it)
	{
		List.push_back((*it));
	}
}

void PointList::setPoints(const std::list<XMFLOAT3>& points)
{
	List.clear();

	for (auto it = points.begin(); it != points.end(); ++it)
	{
		List.push_back((*it));
	}
}

void PointList::setPoints(const std::vector<XMVECTOR>& points)
{
	List.clear();

	XMFLOAT3 newPoint;

	for (auto it = points.begin(); it != points.end(); ++it)
	{
		XMStoreFloat3(&newPoint, (*it));

		List.push_back(newPoint);
	}
}

void PointList::setLine(const LineSegment& line)
{
	List.clear();

	List.push_back(line.Point1);
	List.push_back(line.Point2);
}

void PointList::addLine(const LineSegment& line)
{
	List.push_back(line.Point1);
	List.push_back(line.Point2);
}

PointList& PointList::operator+=(const LineSegment& line)
{
	List.push_back(line.Point1);
	List.push_back(line.Point2);

	return *this;
}

PointList& PointList::operator+=(const XMFLOAT3& point)
{
	List.push_back(point);

	return *this;
}