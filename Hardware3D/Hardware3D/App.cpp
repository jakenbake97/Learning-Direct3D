#include "App.h"
#include <cmath>

App::App()
	:
	wnd(800, 600, "Half-Way D3D Engine")
{
}

App::App(int width, int height, const char* windowName)
	:
	wnd(width, height, windowName)
{
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
	wnd.Gfx().ClearBuffer(0.25f, 0.25f, 0.25f);
	wnd.Gfx().DrawTestTriangle(-timer.Peek(), 0.0f, 0.0f);
	wnd.Gfx().DrawTestTriangle(timer.Peek(), wnd.mouse.GetPosX() / 400.0f - 1.0f, 0.0f,-wnd.mouse.GetPosY() / 300.0f + 1.0f);
	wnd.Gfx().EndFrame();
}
