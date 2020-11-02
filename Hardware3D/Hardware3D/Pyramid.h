﻿#pragma once
#include "TestObject.h"

class Pyramid : public TestObject<Pyramid>
{
public:
	Pyramid(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_int_distribution<int>& tDist);
};
