#pragma once
#include "TestObject.h"

class Cylinder : public TestObject<Cylinder>
{
public:
	Cylinder(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_real_distribution<float>& bDist,
		std::uniform_int_distribution<int>& tDist);
};
