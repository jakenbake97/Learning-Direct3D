#pragma once
#include "Drawable.h"
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"

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
private:
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<Drawable>> drawables;
	float speedFactor = 1.0f;
	Camera cam;
	static constexpr size_t numDrawables = 180;
};
