#include "Window.h"
#include <iostream>
#include <sstream>
//Window Class stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr)) {
	// Register the window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
	UnregisterClass(GetName(), GetInstance());
}

const char* Window::WindowClass::GetName() noexcept {
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {
	return wndClass.hInst;
}

Window::Window(int width, int height, const char* name) noexcept : width(width), height(height){
	// Calculate the window size based off the given width and height
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	//Create the window 
	hWnd = CreateWindow(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left,
		wr.bottom - wr.top, nullptr, nullptr, WindowClass::GetInstance(), this);

	ShowWindow(hWnd, SW_SHOWDEFAULT);

	gfxPtr = std::make_unique<Graphics>(hWnd);
}

Window::~Window() {
	DestroyWindow(hWnd);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	
	// This is a bunch of bs, but basically if we get WM_NCCREATE then we can use our custom msg handler
	if (msg == WM_NCCREATE) {
		// Used to set up pointer to instance that we can use custom message handling on
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgExtra));

		// Forward message to message handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam); // this is default windows behavior
}

// Used to invoke the custom message handler 
LRESULT CALLBACK Window::HandleMsgExtra(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
	// Get the pointer to the window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	// Forward message to message handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept{
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	case WM_KEYUP:
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR: // WM_CHAR is used for like typing in a chat client
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;
	case WM_KILLFOCUS: // Clear the keystate when window loses focus
		keyboard.ClearState();
		break;
	case WM_LBUTTONDOWN: // When the left mouse button is clicked
		POINTS point = MAKEPOINTS(lParam);
		point.x; // Mouse x value
		point.y; // Mouse y value
	}
	return DefWindowProc(hWnd, msg, wParam, lParam); // this is default windows behavior
}

// Window exception stuff
Window::Exception::Exception(int line, const char* file, HRESULT hr) : RenderException(line, file), hr(hr) {}

const char* Window::Exception::what() {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) {
	char* msgBufPtr = nullptr;
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&msgBufPtr), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = msgBufPtr;
	// free windows buffer
	LocalFree(msgBufPtr);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() {
	return hr;
}

const char* Window::Exception::GetType() {
	return "Render Window Exception";
}

std::string Window::Exception::GetErrorString() {
	return TranslateErrorCode(hr);
}

void Window::SetTitle(const std::string title) {
	SetWindowText(hWnd, title.c_str());
}

Graphics& Window::Gfx(){
	return *gfxPtr;
}

// second will be false if no message in queue and not qutting app
std::pair<int, bool> Window::ProcessMessages() {
	MSG msg;
	std::pair<int, bool > p;
	p.first = 0;
	p.second = false;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		
		if (msg.message == WM_QUIT) {
			p.first = msg.wParam;
			p.second = true;
			return p;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return pair false if not quitting app and no messages left
	return p;
}

