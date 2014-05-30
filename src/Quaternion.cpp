#include "Quaternion.h"

Quaternion::Quaternion(const Vector3& n, float a)
{
	a = a / 360.0f * XM_PI * 2.0f;

	w = cosf(a / 2.0f);

	x = n.x * sinf(a / 2.0f);
	y = n.y * sinf(a / 2.0f);
	z = n.z * sinf(a / 2.0f);
}

Quaternion::~Quaternion()
{

}