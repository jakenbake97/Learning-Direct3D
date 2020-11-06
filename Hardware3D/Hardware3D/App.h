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
	void ShowModelWindow();
private:
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	float speedFactor = 1.0f;
	Camera cam;
	PointLight light;
	Model nanoSuit{wnd.Gfx(), "Models\\nanosuit.obj"};

	struct 
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float coord[3] = {0.0f, 0.0f, 0.0f};
	} pos;
};
