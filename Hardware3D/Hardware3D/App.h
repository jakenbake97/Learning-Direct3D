#pragma once
#include "Window.h"

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
};
