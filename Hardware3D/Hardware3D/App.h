#pragma once
#include "Window.h"
#include "Timer.h"

class App
{
public:
	App();
	App(int width, int height, const char* windowName);

	// Starts the Game Loop
	int Start();
private:
	void FrameUpdate();
private:
	Window wnd;
	Timer timer;
};
