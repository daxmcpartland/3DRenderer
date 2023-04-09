#pragma once
#include "Window.h"
#include <ctime>
class App {
public:
	App();
	int Run();

private: 
	void OnFrame();
	Window wnd;
	clock_t timer;
};