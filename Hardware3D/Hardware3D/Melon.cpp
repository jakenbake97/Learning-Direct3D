#include "Melon.h"
#include "BindableBase.h"
#include "GraphicsErrorMacros.h"
#include "Sphere.h"

Melon::Melon(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& aDist,
	std::uniform_real_distribution<float>& dDist, std::uniform_real_distribution<float>& oDist,
	std::uniform_real_distribution<float>& rDist, std::uniform_int_distribution<int>& longDist,
	std::uniform_int_distribution<int>& latDist)
		:
r(rDist(rng)),
theta(aDist(rng)),
phi(aDist(rng)),
chi(aDist(rng)),
dRoll(dDist(rng)),
dPitch(dDist(rng)),
dYaw(dDist(rng)),
dPhi(oDist(rng)),
dChi(oDist(rng))
{
	namespace dx = DirectX;

	if( !IsStaticInitialized())
	{
		auto pvs = std::make_unique<VertexShader>(gfx, L"ColorIndexVs.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorIndexPs.cso"));

		struct PixelShaderConstants
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			}faceColors[8];
		};
		const PixelShaderConstants colorBuffer
		{
			{
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, colorBuffer));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inElemDesc =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, inElemDesc, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};

	auto model = Sphere::MakeTessellated<Vertex>(latDist(rng), longDist(rng));

	// deform vertices of model by linear transformation
	model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));

	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Melon::Update(float dt) noexcept
{
	roll += dRoll * dt;
	pitch += dPitch * dt;
	yaw += dYaw * dt;
	theta += dTheta * dt;
	phi += dPhi * dt;
	chi += dChi * dt;
}

DirectX::XMMATRIX Melon::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
