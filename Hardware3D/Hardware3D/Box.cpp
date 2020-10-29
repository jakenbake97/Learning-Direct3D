#include "Box.h"
#include "BindableBase.h"
#include "GraphicsErrorMacros.h"
#include <DirectXMath.h>
#include "Cube.h"

Box::Box(Graphics& gfx, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& aDist, 
	std::uniform_real_distribution<float>& dDist, 
	std::uniform_real_distribution<float>& oDist, 
	std::uniform_real_distribution<float>& rDist,
	std::uniform_real_distribution<float>& bDist)
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
	namespace dx = DirectX;
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
		};
		
		const auto model = Cube::Make<Vertex>();
		
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"ColorIndexVs.cso");
		auto pvsbc = pvs->GetBytecode();

		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPs.cso"));


		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		struct ConstantColorBuffer
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			}faceColors[8];
		};
		const ConstantColorBuffer colorBuf =
		{
			{
				{1.0f, 1.0f, 1.0f},
				{1.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{1.0f, 1.0f, 0.0f},
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{0.0f, 1.0f, 1.0f},
				{0.0f, 0.0f, 0.0f}
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

	// model deformation transform (per instance, not stored as bind)
	dx::XMStoreFloat3x3(&mt, dx::XMMatrixScaling(1.0f, 1.0f, bDist(rng)));
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
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt) *
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}