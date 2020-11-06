#include "App.h"
#include "AssetImportTest.h"
#include <memory>
#include <algorithm>
#include <iterator>
#include "HWMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

GDIPlusManager gdiPM;

App::App()
	:
	App(1200, 800, "Half-Way D3D Engine")
{
}

App::App(int width, int height, const char* windowName)
	:
	wnd(width, height, windowName),
	light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

int App::Start()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto eCode = Window::ProcessMessages())
		{
			// if return optional has value, it means we are quitting so return exit code
			return *eCode;
		}
		FrameUpdate();
	}
}

void App::FrameUpdate()
{
	const auto dt = timer.Mark() * speedFactor;

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	const auto transform = dx::XMMatrixRotationRollPitchYaw(pos.pitch, pos.yaw, pos.roll) * dx::XMMatrixTranslation(
		pos.coord[0], pos.coord[1], pos.coord[2]);
	nanoSuit.Draw(wnd.Gfx(), transform);

	light.Draw(wnd.Gfx());

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowModelWindow()
{
	if (ImGui::Begin("Model"))
	{
		using namespace std::string_literals;

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

		ImGui::DragFloat3("Position", pos.coord, 1.0f, -20.0f, 20.0f);
	}
	ImGui::End();
}
