#pragma once
#include "BetterWindows.h"
#include <wrl.h>
#include <d3d11.h>

class Graphics {
public: 
	Graphics(HWND hWnd);
	~Graphics() = default;
	Graphics(const Graphics&) = delete; // don't need copying or moving graphic objects
	Graphics& operator=(const Graphics&) = delete;

	void EndFrame(); // Does the presentation of flipping the buffers
	void ClearBuffer(float r, float g, float b);
	void DrawTestTriangle(float angle);
private:
	Microsoft::WRL::ComPtr<ID3D11Device> devicePtr; // used for allocating things
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapPtr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> contextPtr; // used to access the drawing pipeline or executing rendering commands
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetPtr;
};
