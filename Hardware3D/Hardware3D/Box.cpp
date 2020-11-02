#include "Box.h"
#include "BindableBase.h"
#include "GraphicsErrorMacros.h"
#include <DirectXMath.h>
#include "Cube.h"
#include "imgui/imgui.h"

Box::Box(Graphics& gfx, 
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& aDist, 
	std::uniform_real_distribution<float>& dDist, 
	std::uniform_real_distribution<float>& oDist, 
	std::uniform_real_distribution<float>& rDist,
	std::uniform_real_distribution<float>& bDist,
	DirectX::XMFLOAT3 material)
		:
TestObject(gfx, rng, aDist, dDist, oDist, rDist)
{
	namespace dx = DirectX;
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};
		
		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();

		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));


		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElemDesc =
		{
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

	materialConstants.color = material;
	AddBind(std::make_unique<MaterialCbuf>(gfx, materialConstants, 1u));

	// model deformation transform (per instance, not stored as bind)
	dx::XMStoreFloat3x3(&mt, dx::XMMatrixScaling(1.0f, 1.0f, bDist(rng)));
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt) * TestObject::GetTransformXM();
}

bool Box::SpawnControlWindow(int id, Graphics& gfx) noexcept
{
	using namespace std::string_literals;

	bool hasChanges = false;
	bool open = true;
	if(ImGui::Begin(("Box "s + std::to_string(id)).c_str(), &open))
	{
		ImGui::Text("Material Properties");
		const auto color = ImGui::ColorEdit3("Material Color", &materialConstants.color.x);
		const auto specIntensity= ImGui::SliderFloat("Specular Intensity", &materialConstants.specularIntensity, 0.05f, 4.0f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat);
		const auto specPower = ImGui::SliderFloat("Specular Power", &materialConstants.specularPower, 1.0f, 200.0f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat);

		hasChanges = color || specIntensity || specPower;

		ImGui::Text("Position");
		ImGui::SliderFloat("Radius", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
	}
	ImGui::End();

	if (hasChanges)
	{
		SyncMaterial(gfx);
	}
	
	return open;
}

void Box::SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG)
{
	auto pConstPS = QueryBindable<MaterialCbuf>();
	assert(pConstPS != nullptr);
	pConstPS->Update(gfx, materialConstants);
}
