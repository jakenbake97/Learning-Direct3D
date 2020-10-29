﻿#pragma once
#include "Drawable.h"
#include "Window.h"
#include "Timer.h"

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
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<Drawable>> drawables;
	static constexpr size_t numDrawables = 180;
};
