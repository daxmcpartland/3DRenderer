#pragma once
#include "BetterWindows.h"
#include "RenderException.h"
#include "Keyboard.h"
#include "Graphics.h"
#include <memory>

class Window {
	// Remember windows needs one or mor window classes and a window's instance.

public:
	// Make an exception class for window that inherits from render exception
	class Exception : public RenderException {
	public:
		// HRESULT is a window's api error
		Exception(int line, const char* file, HRESULT hr);
		const char* what();
		virtual const char* GetType();
		static std::string TranslateErrorCode(HRESULT hr);
		HRESULT GetErrorCode();
		std::string GetErrorString();
	private:
		HRESULT hr;
	};
private: 
	class WindowClass{
	public:
		static const char* GetName() noexcept; // noexcept means it will not throw any errors
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Keyboard keyboard;
	static std::pair<int, bool> ProcessMessages();
	void SetTitle(const std::string title);
	Graphics& Gfx();

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgExtra(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> gfxPtr;
};