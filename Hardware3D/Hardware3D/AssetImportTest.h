#pragma once
#include "TestObject.h"
#include "ConstantBuffers.h"

class AssetImportTest : public TestObject<AssetImportTest>
{
public:
	AssetImportTest(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist,
		DirectX::XMFLOAT3 material, float scale
	);
};
