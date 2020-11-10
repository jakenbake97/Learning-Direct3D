#include "App.h"
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
	App(1920, 1080, "Half-Way D3D Engine")
{
}

App::App(int width, int height, const char* windowName)
	:
	wnd(width, height, windowName),
	light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));
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

	nanoSuit.Draw(wnd.Gfx());

	light.Draw(wnd.Gfx());

	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow(false);
	nanoSuit.ShowWindow();

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow(bool showDemoWindow)
{
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);
}
