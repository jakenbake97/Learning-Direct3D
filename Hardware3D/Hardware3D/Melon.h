#pragma once
#include "DrawableBase.h"

class Melon : public DrawableBase<Melon>
{
public:
	Melon(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_int_distribution<int>& longDist,
		std::uniform_int_distribution<int>& latDist);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;

	// speed (delta / s)
	float dRoll;
	float dPitch;
	float dYaw;
	float dTheta;
	float dPhi;
	float dChi;
};
