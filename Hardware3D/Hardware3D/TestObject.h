#pragma once

#include "DrawableBase.h"

template<class T>
class TestObject : public DrawableBase<T>
{
public:
	TestObject(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist)
		:
		r(rDist(rng)),
		dRoll(dDist(rng)),
		dPitch(dDist(rng)),
		dYaw(dDist(rng)),
		dPhi(oDist(rng)),
		dTheta(oDist(rng)),
		dChi(oDist(rng)),
		chi(aDist(rng)),
		theta(aDist(rng)),
		phi(aDist(rng))
	{}
	void Update(float dt) noexcept
	{
		roll += dRoll * dt;
		pitch += dPitch * dt;
		yaw += dYaw * dt;
		theta += dTheta * dt;
		phi += dPhi * dt;
		chi += dChi * dt;
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		namespace dx = DirectX;
		return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
			dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}
private:
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;
	// speed (delta/s)
	float dRoll;
	float dPitch;
	float dYaw;
	float dTheta;
	float dPhi;
	float dChi;
};