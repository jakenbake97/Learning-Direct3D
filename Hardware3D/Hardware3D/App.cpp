#include "App.h"
#include "Box.h"
#include <cmath>
#include <memory>

App::App()
	:
	wnd(800, 600, "Half-Way D3D Engine")
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> aDist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> dDist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> oDist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rDist(6.0f, 20.0f);

	for(auto i = 0; i < 80; ++i)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd.Gfx(), rng, aDist,
			dDist, oDist, rDist
			));
	}
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::App(int width, int height, const char* windowName)
	:
	wnd(width, height, windowName)
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> aDist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> dDist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> oDist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rDist(6.0f, 20.0f);

	for (auto i = 0; i < 80; ++i)
	{
		boxes.push_back(std::make_unique<Box>(
			wnd.Gfx(), rng, aDist,
			dDist, oDist, rDist
			));
	}
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
	const auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	wnd.Gfx().EndFrame();
}
