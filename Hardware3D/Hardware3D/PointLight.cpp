#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics& gfx, float radius)
	:
mesh(gfx, radius),
cBuf(gfx)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if(ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		float tempPos[3] = {cbData.pos.x, cbData.pos.y, cbData.pos.z};
		ImGui::DragFloat3("Position", tempPos, 1.0f, -60.0f, 60.0f, "%.1f");

		cbData.pos = { tempPos[0], tempPos[1], tempPos[2] };
		
		ImGui::Text("Appearance");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient Color", &cbData.ambient.x);

		ImGui::Text("Light Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 0.1f, "%.7f", ImGuiSliderFlags_Logarithmic);
		
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData = {
		{0.0f, 0.0f, 0.0f},
		{0.05f, 0.05f, 0.05f},
		{1.0f, 1.0f, 1.0f},
		1.0f,
		1.0f,
		0.045f,
		0.0075f,	
	};
}

void PointLight::Draw(Graphics& gfx) const noxnd
{
	mesh.SetPos(cbData.pos);
	mesh.Draw(gfx);
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	cBuf.Update(gfx, dataCopy);
	cBuf.Bind(gfx);
}
