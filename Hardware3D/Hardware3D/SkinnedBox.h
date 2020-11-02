#pragma once

#include "TestObject.h"

class SkinnedBox : public TestObject<SkinnedBox>
{
public:
	SkinnedBox(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist);
};
