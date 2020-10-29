#include "Box.h"
#include "BindableBase.h"
#include "GraphicsErrorMacros.h"
#include <DirectXMath.h>

Box::Box(Graphics& gfx, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& aDist, 
	std::uniform_real_distribution<float>& dDist, 
	std::uniform_real_distribution<float>& oDist, 
	std::uniform_real_distribution<float>& rDist)
		:
r(rDist(rng)),
theta(aDist(rng)),
phi(aDist(rng)),
chi(aDist(rng)),
dRoll(dDist(rng)),
dPitch(dDist(rng)),
dYaw(dDist(rng)),
dTheta(oDist(rng)),
dPhi(oDist(rng)),
dChi(oDist(rng))
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			} pos;
		};
		const std::vector<Vertex> vertices =
		{
			{-1.0f, -1.0f, -1.0f},
			{1.0f, -1.0f, -1.0f},
			{-1.0f, 1.0f, -1.0f},
			{1.0f,1.0f,-1.0f},
			{-1.0f,-1.0f,1.0f},
			{1.0f,-1.0f,1.0f},
			{-1.0f,1.0f,1.0f},
			{1.0f,1.0f,1.0f}
		};
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"VertexShader.cso");
		auto pvsbc = pvs->GetBytecode();

		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelShader.cso"));

		const std::vector<unsigned short> indices =
		{
			0,2,1,  2,3,1,
			1,3,5,  3,7,5,
			2,6,3,  3,6,7,
			4,5,7,  4,7,6,
			0,4,2,  2,4,6,
			0,1,4,  1,5,4
		};

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		struct ConstantColorBuffer
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			}faceColors[6];
		};
		const ConstantColorBuffer colorBuf =
		{
			{
				{1.0f, 0.0f, 1.0f},
				{1.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{0.0f, 0.0f, 1.0f},
				{1.0f, 1.0f, 0.0f},
				{0.0f, 1.0f, 1.0f},
			}
		};

		AddStaticBind(std::make_unique<PixelConstantBuffer<ConstantColorBuffer>>(gfx, colorBuf));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElemDesc =
		{
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, inputElemDesc, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	// transforms are unique to each box, so they aren't in the static buffers
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Box::Update(float dt) noexcept
{
	roll += dRoll * dt;
	pitch += dPitch * dt;
	yaw += dYaw * dt;
	theta += dTheta * dt;
	phi += dPhi * dt;
	chi += dChi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}