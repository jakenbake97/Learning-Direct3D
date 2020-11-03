#include "App.h"
#include "Pyramid.h"
#include "Box.h"
#include "Cylinder.h"
#include "SkinnedBox.h"
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
	
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{
		}

		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = {cDist(rng), cDist(rng), cDist(rng)};
			switch (sDist(rng))
			{
			case 0:
				return std::make_unique<Box>(gfx, rng, aDist, dDist, oDist, rDist, bDist, mat);
			case 1:
				return std::make_unique<Cylinder>(gfx, rng, aDist, dDist, oDist, rDist, bDist, tDist);
			case 2:
				return std::make_unique<Pyramid>(gfx, rng, aDist, dDist, oDist, rDist, tDist);
			case 3:
				return std::make_unique<SkinnedBox>(gfx, rng, aDist, dDist, oDist, rDist);
			case 4:
				return std::make_unique<AssetImportTest>(gfx, rng, aDist, dDist, oDist, rDist, mat, 1.5f);
			default:
				assert(false && "impossible drawable option in factory");
				return {};
			}
		}

	private:
		Graphics& gfx;
		std::mt19937 rng{std::random_device{}()};
		std::uniform_int_distribution<int> sDist{0, 4};
		std::uniform_real_distribution<float> aDist{0.0f, PI * 2.0f};
		std::uniform_real_distribution<float> dDist{0.0f, PI * 0.5f};
		std::uniform_real_distribution<float> oDist{0.0f, PI * 0.08f};
		std::uniform_real_distribution<float> rDist{6.0f, 20.0f};
		std::uniform_real_distribution<float> bDist{0.4f, 3.0f};
		std::uniform_real_distribution<float> cDist{0.0f, 1.0f};
		std::uniform_int_distribution<int> tDist{3, 30};
	};

	drawables.reserve(numDrawables);
	std::generate_n(std::back_inserter(drawables), numDrawables, Factory(wnd.Gfx()));

	// inti box pointers for editing instance parameters
	for (auto& pd : drawables)
	{
		if (auto pb = dynamic_cast<Box*>(pd.get()))
		{
			boxes.push_back(pb);
		}
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
	const auto dt = timer.Mark() * speedFactor;

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);

	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	for (auto& mesh : drawables)
	{
		mesh->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		mesh->Draw(wnd.Gfx());
	}
	light.Draw(wnd.Gfx());

	// imgui windows
	SpawnSimulationWindow();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	SpawnBoxWindowManagerWindow();
	SpawnBoxWindows();

	// present
	wnd.Gfx().EndFrame();
}

void App::SpawnSimulationWindow() noexcept
{
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speedFactor, 0.0f, 6.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		            ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", wnd.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold space to pause)");
	}
	ImGui::End();
}

void App::SpawnBoxWindowManagerWindow() noexcept
{
	// imgui window to open box windows
	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < boxes.size(); ++i)
			{
				const bool selected = *comboBoxIndex == i;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
				{
					comboBoxIndex = i;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIDs.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
}

void App::SpawnBoxWindows() noexcept
{
	// imgui box attribute control window
	for (auto i = boxControlIDs.begin(); i != boxControlIDs.end();)
	{
		if (!boxes[*i]->SpawnControlWindow(*i, wnd.Gfx()))
		{
			i = boxControlIDs.erase(i);
		}
		else
		{
			++i;
		}
	}
}
