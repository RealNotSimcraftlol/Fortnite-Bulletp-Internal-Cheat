#include "vector3d.h"

Vector3::Vector3() {
	x = y = z = 0.0f;
}

Vector3::Vector3(const float fx, const float fy, const float fz) {
	x = fx;
	y = fy;
	z = fz;
}

Vector3::~Vector3() = default;