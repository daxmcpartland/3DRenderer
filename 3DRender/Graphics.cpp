#include "Graphics.h"
#include <D3DCompiler.h>
#include <string>
#include <cmath>
#pragma comment(lib, "d3d11.lib") //sets linker to link the 3d311 library
#pragma comment(lib, "D3DCompiler.lib")
Graphics::Graphics(HWND hWnd){

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0; // Use whatever width and height
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0; // Use whatever width and height
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1; // One front buffer and one Back buffer
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // Basically swapping buffers then discarding it
	sd.Flags = 0;


	// create device and front/back buffers, and swap chain and rendering context
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &swapPtr, &devicePtr, nullptr, &contextPtr);

	Microsoft::WRL::ComPtr<ID3D11Resource> backBufferPtr = nullptr;
	swapPtr->GetBuffer(0, __uuidof(ID3D11Resource), &backBufferPtr);
	devicePtr->CreateRenderTargetView(backBufferPtr.Get(), nullptr, &targetPtr);
	backBufferPtr->Release();
}

void Graphics::EndFrame(){
	swapPtr->Present(1u, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b){
	const float color[] = { r, g, b, 1.0f };
	contextPtr->ClearRenderTargetView(targetPtr.Get(), color);
}

void Graphics::DrawTestTriangle(float angle) {

		struct Vertex {
			float x;
			float y;
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};

		// Create a Vertex buffer (1 2D triangle at the center of screen)
		const Vertex vertices[] = { 
			{-0.5f, 0.5f, 255, 0, 255, 0}, 
			{0.5f, 0.5f, 0, 0, 255, 0},
			{-0.5f, -0.5f, 255, 0, 0, 0},
			{0.5f, -0.5f, 0, 255, 0, 0},
			{-0.7f, -0.3f, 0, 255, 255, 0},
			{-0.7f, 0.8f, 255, 255, 0, 0},
			{0.3f, 0.8f, 255, 0, 255, 0},
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBufferPtr;
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.CPUAccessFlags = 0u;
		bufferDesc.MiscFlags = 0u;
		bufferDesc.ByteWidth = sizeof(vertices);
		bufferDesc.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA srcData = {};
		srcData.pSysMem = vertices;
		devicePtr->CreateBuffer(&bufferDesc, &srcData, &vertexBufferPtr);

		// Bind the vertex to the pipeline 
		UINT stride = sizeof(Vertex);
		UINT offset = 0u;
		contextPtr->IASetVertexBuffers(0u, 1u, vertexBufferPtr.GetAddressOf(), &stride, &offset);

		const unsigned short indices[] = { 
			0, 1, 3, 
			0, 3, 2, 
			5, 2, 4,  
			5, 0, 2,
			5, 6, 0,
			6, 1, 0,
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBufferPtr;
		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		devicePtr->CreateBuffer(&ibd, &isd, &indexBufferPtr);

		//bind index buffer
		contextPtr->IASetIndexBuffer(indexBufferPtr.Get(), DXGI_FORMAT_R16_UINT, 0u);

		struct ConstantBuffer {
			struct {
				float element[4][4];
			} transformation;
		};

		const ConstantBuffer constantBuff = {
			// Create a transformation matrix that spins on the z axis
			// We multiply the x coordinates by 3/4 because our screen is 800x600 
			{
				(3.0f / 4.0f) * std::cos(angle),  std::sin(angle), 0.0f, 0.0f,
				(3.0f / 4.0f) * -std::sin(angle), std::cos(angle), 0.0f, 0.0f,
				0.0f,								0.0f,		   1.0f, 0.0f,
				0.0f,								0.0f,		   0.0f, 1.0f,
			}
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferPtr;
		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(constantBuff);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &constantBuff;
		devicePtr->CreateBuffer(&cbd, &csd, &constantBufferPtr);

		//bind constant buffer to shader
		contextPtr->VSSetConstantBuffers(0u, 1u, constantBufferPtr.GetAddressOf());

		// create the pixel shader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaderPtr;
		Microsoft::WRL::ComPtr<ID3DBlob> blobPtr;
		D3DReadFileToBlob(L"PixelShader.cso", &blobPtr);
		devicePtr->CreatePixelShader(blobPtr->GetBufferPointer(), blobPtr->GetBufferSize(), nullptr, &pixelShaderPtr);
		contextPtr->PSSetShader(pixelShaderPtr.Get(), 0, 0);

		// create the vertex shader
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShaderPtr;
		D3DReadFileToBlob(L"VertexShader.cso", &blobPtr);
		devicePtr->CreateVertexShader(blobPtr->GetBufferPointer(), blobPtr->GetBufferSize(), nullptr, &vertexShaderPtr);
		contextPtr->VSSetShader(vertexShaderPtr.Get(), 0, 0);


		// input (vertex) layout (2d position only)
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayoutPtr;
		const D3D11_INPUT_ELEMENT_DESC ied[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		devicePtr->CreateInputLayout(ied, (UINT)std::size(ied), blobPtr->GetBufferPointer(), blobPtr->GetBufferSize(), &inputLayoutPtr);

		// bind that input layout
		contextPtr->IASetInputLayout(inputLayoutPtr.Get());

		// bind the render target
		contextPtr->OMSetRenderTargets(1u, targetPtr.GetAddressOf(), nullptr);

		// set primitive topology to triangle list 
		contextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// configure viewport
		D3D11_VIEWPORT vp;
		vp.Width = 800;
		vp.Height = 600;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		contextPtr->RSSetViewports(1u, &vp);

		//contextPtr->Draw((UINT)std::size(vertices), 0u);
		contextPtr->DrawIndexed((UINT)std::size(indices), 0u, 0u);
}
