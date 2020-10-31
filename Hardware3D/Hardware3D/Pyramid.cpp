#include "Pyramid.h"
#include "Cone.h"
#include "BindableBase.h"
#include "GraphicsErrorMacros.h"

Pyramid::Pyramid(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& aDist,
                 std::uniform_real_distribution<float>& dDist, std::uniform_real_distribution<float>& oDist,
                 std::uniform_real_distribution<float>& rDist)
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

	if(!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;

			struct 
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} color;
		};

		auto model = Cone::MakeTessellated<Vertex>(4);
		// set vertex colors for mesh
		model.vertices[0].color = { 255,255,0 };
		model.vertices[1].color = { 255,255,0 };
		model.vertices[2].color = { 255,255,0 };
		model.vertices[3].color = { 255,255,0 };
		model.vertices[4].color = { 255,255,80 };
		model.vertices[5].color = { 255,10,0 };
		// deform mesh linearly
		model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"ColorBlendVS.cso");
		auto pvsbc = pvs->GetBytecode();

		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"ColorBlendPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inElemDesc =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, inElemDesc, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Pyramid::Update(float dt) noexcept
{
	roll += dRoll * dt;
	pitch += dPitch * dt;
	yaw += dYaw * dt;
	theta += dTheta * dt;
	phi += dPhi * dt;
	chi += dChi * dt;
}

DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
