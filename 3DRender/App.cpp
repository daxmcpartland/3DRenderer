#include "App.h"
#include <ctime>
#include <sstream>

App::App() : wnd(800, 600, "3DRenderer") {
	timer = clock();
}

int App::Run() {
	while (true) {
		auto ecode = Window::ProcessMessages();
		if (ecode.second) {
			return ecode.first;
		}
		
		OnFrame();
	}
}

void App::OnFrame() {
	double seconds = clock();
	seconds = seconds / 1000;
	const float c = sin(seconds) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	wnd.Gfx().DrawTestTriangle(seconds);
	wnd.Gfx().EndFrame();
}