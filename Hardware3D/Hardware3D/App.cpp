#include "App.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include <cmath>
#include <memory>
#include <algorithm>
#include <iterator>

#include "HWMath.h"

App::App()
	:
App(800, 600, "Half-Way D3D Engine")
{
}

App::App(int width, int height, const char* windowName)
	:
	wnd(width, height, windowName)
{
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
		gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch (typeDist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, aDist, dDist,
					oDist, rDist
					);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, aDist, dDist,
					oDist, rDist, bDist
					);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, aDist, dDist,
					oDist, rDist, longDist, latDist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> aDist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> dDist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> oDist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rDist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bDist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latDist{ 5,20 };
		std::uniform_int_distribution<int> longDist{ 10,40 };
		std::uniform_int_distribution<int> typeDist{ 0,2 };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(numDrawables);
	std::generate_n(std::back_inserter(drawables), numDrawables, f);
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
	for (auto& b : drawables)
	{
		b->Update(dt);
		b->Draw(wnd.Gfx());
	}
	wnd.Gfx().EndFrame();
}
