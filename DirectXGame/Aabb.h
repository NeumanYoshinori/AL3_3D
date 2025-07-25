#pragma once
#include <KamataEngine.h>

using namespace KamataEngine;

struct AABB {
	Vector3 min;
	Vector3 max;
};

class Aabb {
public:
	bool IsCollision(const AABB& aabb, const AABB& aabb2);
};