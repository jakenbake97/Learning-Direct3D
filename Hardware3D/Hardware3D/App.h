#pragma once
#include "Drawable.h"
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>

class App
{
public:
	App();
	App(int width, int height, const char* windowName);

	// Starts the Game Loop
	int Start();
	~App() = default;
private:
	void FrameUpdate();
	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
private:
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<Drawable>> drawables;
	std::vector<class Box*> boxes;
	float speedFactor = 1.0f;
	Camera cam;
	PointLight light;
	static constexpr size_t numDrawables = 180;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIDs;
};
