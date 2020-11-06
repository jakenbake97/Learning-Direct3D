#include "Camera.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const auto pos = dx::XMVectorSet(coord[0], coord[1], coord[2], 1.0f);

	return dx::XMMatrixLookAtLH(pos, dx::XMVectorAdd(pos, dx::XMVectorSet(0.0f, 0.0f, 1.0f,1.0f)),
		dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) *
		dx::XMMatrixRotationRollPitchYaw(pitch, -yaw, roll
		);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::DragFloat3("Position", coord);

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}
