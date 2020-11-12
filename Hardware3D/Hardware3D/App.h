#pragma once
#include "Drawable.h"
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
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
	static void ShowImguiDemoWindow(bool showDemoWindow);
	void ShowRawInputWindow();
private:
	int x = 0, y = 0;
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	float speedFactor = 1.0f;
	Camera cam;
	PointLight light;
	Model nanoSuit{wnd.Gfx(), "Models\\nano.gltf"};
};
