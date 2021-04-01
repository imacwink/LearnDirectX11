//
//  d3d11_render.cpp
//  LearnDirect3D11
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#include "engine/d3d11_render.hpp"

D3D11Render::D3D11Render(/* args */)
{
	mSwapChain = NULL;
	mDevice = NULL;
	mDeviceContext = NULL;
	mRenderTargetView = NULL;
	mVertexShader = NULL;
	mPixelShader = NULL;
	mInputLayout = NULL;
	mVertexBuffer = NULL;
	mNumVerts = 0;
	mStride = 0;
	mOffset = 0;
	mHwnd = NULL;
}

D3D11Render::D3D11Render(const D3D11Render & /*d3d11Render*/)
{
}

D3D11Render::~D3D11Render()
{
}

bool D3D11Render::CreateDeviceAndContext()
{
	ID3D11Device *baseDevice;
	ID3D11DeviceContext *baseDeviceContext;
	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG_BUILD)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE,
										0, creationFlags,
										featureLevels, ARRAYSIZE(featureLevels),
										D3D11_SDK_VERSION, &baseDevice,
										0, &baseDeviceContext);
	if (FAILED(hResult))
	{
		MessageBoxA(0, "D3D11CreateDevice() failed", "Fatal Error", MB_OK); //GetLastError()
		return false;
	}

	// Get interface of D3D11 Device and Context
	hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device), (void **)&mDevice);
	assert(SUCCEEDED(hResult));
	baseDevice->Release();

	hResult = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext), (void **)&mDeviceContext);
	assert(SUCCEEDED(hResult));
	baseDeviceContext->Release();

#ifdef DEBUG_BUILD
	// Set up debug layer to break on D3D11 errors
	ID3D11Debug *d3dDebug = nullptr;
	mDevice->QueryInterface(__uuidof(ID3D11Debug), (void **)&d3dDebug);
	if (d3dDebug)
	{
		ID3D11InfoQueue *d3dInfoQueue = nullptr;
		if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&d3dInfoQueue)))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
			d3dInfoQueue->Release();
		}
		d3dDebug->Release();
	}
#endif

	return true;
}

bool D3D11Render::CreateSwapChain(int screenWidth, int screenHeight, HWND hwnd)
{
	// Get DXGI Factory (needed to create Swap Chain)
	IDXGIFactory *dxgiFactory;
	{
		IDXGIDevice *dxgiDevice;
		HRESULT hResult = mDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgiDevice);
		assert(SUCCEEDED(hResult));

		IDXGIAdapter *dxgiAdapter;
		hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
		assert(SUCCEEDED(hResult));
		dxgiDevice->Release();

		DXGI_ADAPTER_DESC adapterDesc;
		dxgiAdapter->GetDesc(&adapterDesc);

		OutputDebugStringA("Graphics Device: ");
		OutputDebugStringW(adapterDesc.Description);

		hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgiFactory);
		assert(SUCCEEDED(hResult));
		dxgiAdapter->Release();
	}

	DXGI_SWAP_CHAIN_DESC d3d11SwapChainDesc = {};
	d3d11SwapChainDesc.BufferCount = 1;								   // Set to a single back buffer.
	d3d11SwapChainDesc.BufferDesc.Width = screenWidth;				   // Set the width of the back buffer.
	d3d11SwapChainDesc.BufferDesc.Height = screenHeight;			   // Set the height of the back buffer.
	d3d11SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Set regular 32-bit surface for the back buffer.
	d3d11SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;		   // Set the refresh rate of the back buffer.
	d3d11SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;		   // Set the refresh rate of the back buffer.
	d3d11SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // Set the usage of the back buffer.
	d3d11SwapChainDesc.OutputWindow = hwnd;							   // Set the handle for the window to render to.
	d3d11SwapChainDesc.SampleDesc.Count = 1;						   // Turn multisampling off.
	d3d11SwapChainDesc.SampleDesc.Quality = 0;						   // Turn multisampling off.
	d3d11SwapChainDesc.Windowed = true;								   // Set to full screen or windowed mode.

	// Set the scan line ordering and scaling to unspecified.
	d3d11SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	d3d11SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // Discard the back buffer contents after presenting.
	d3d11SwapChainDesc.Flags = 0;							  // Don't set the advanced flags.
	HRESULT hResult = dxgiFactory->CreateSwapChain(mDevice, &d3d11SwapChainDesc, &mSwapChain);
	assert(SUCCEEDED(hResult));
	dxgiFactory->Release();

	return true;
}

bool D3D11Render::CreateFramebuffer()
{
	ID3D11Texture2D *d3d11FrameBuffer;
	HRESULT hResult = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&d3d11FrameBuffer);
	assert(SUCCEEDED(hResult));

	hResult = mDevice->CreateRenderTargetView(d3d11FrameBuffer, 0, &mRenderTargetView);
	assert(SUCCEEDED(hResult));
	d3d11FrameBuffer->Release();

	return true;
}

void D3D11Render::CreateShader()
{
	// Create Vertex Shader
	ID3DBlob *vsBlob;
	ID3DBlob *vsShaderCompileErrorsBlob;
	HRESULT hResult = D3DCompileFromFile(L"res/shaders/triangle.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, &vsBlob, &vsShaderCompileErrorsBlob);
	if (FAILED(hResult))
	{
		const char *errorString = NULL;
		if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			errorString = "Could not compile shader; file not found";
		}
		else if (vsShaderCompileErrorsBlob)
		{
			errorString = (const char *)vsShaderCompileErrorsBlob->GetBufferPointer();
			vsShaderCompileErrorsBlob->Release();
		}

		MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
	}

	hResult = mDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShader);
	assert(SUCCEEDED(hResult));

	// Create Pixel Shader
	ID3DBlob *psBlob;
	ID3DBlob *psShaderCompileErrorsBlob;
	hResult = D3DCompileFromFile(L"res/shaders/triangle.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &psShaderCompileErrorsBlob);
	if (FAILED(hResult))
	{
		const char *errorString = NULL;
		if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			errorString = "Could not compile shader; file not found";
		}
		else if (psShaderCompileErrorsBlob)
		{
			errorString = (const char *)psShaderCompileErrorsBlob->GetBufferPointer();
			psShaderCompileErrorsBlob->Release();
		}

		MessageBoxA(0, errorString, "Shader Compiler Error", MB_ICONERROR | MB_OK);
	}

	hResult = mDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShader);
	assert(SUCCEEDED(hResult));
	psBlob->Release();

	// Create Input Layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{"POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}};

	hResult = mDevice->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout);
	assert(SUCCEEDED(hResult));
	vsBlob->Release();
}

void D3D11Render::CreateVertexBuffer()
{
	float vertexData[] = {// x, y, r, g, b, a
						  0.0f, 0.5f, 0.f, 1.f, 0.f, 1.f,
						  0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f,
						  -0.5f, -0.5f, 0.f, 0.f, 1.f, 1.f};
	mStride = 6 * sizeof(float);
	mNumVerts = sizeof(vertexData) / mStride;
	mOffset = 0;

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = sizeof(vertexData);
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData = {vertexData};

	HRESULT hResult = mDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &mVertexBuffer);
	assert(SUCCEEDED(hResult));
}

bool D3D11Render::Awake(int screenWidth, int screenHeight, HWND hwnd)
{
	mHwnd = hwnd;

	bool result = true;
	result = CreateDeviceAndContext();
	if (!result)
	{
		return result;
	}

	result = CreateSwapChain(screenWidth, screenHeight, hwnd);
	if (!result)
	{
		return result;
	}

	result = CreateFramebuffer();
	if (!result)
	{
		return result;
	}
	
	CreateShader();
	CreateVertexBuffer();

	return result;
}

void D3D11Render::OnDestroy()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (mSwapChain)
	{
		mSwapChain->SetFullscreenState(false, NULL);
	}

	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
		mRenderTargetView = NULL;
	}

	if (mDeviceContext)
	{
		mDeviceContext->Release();
		mDeviceContext = NULL;
	}

	if (mDevice)
	{
		mDevice->Release();
		mDevice = NULL;
	}

	if (mSwapChain)
	{
		mSwapChain->Release();
		mSwapChain = NULL;
	}

	if (mVertexShader)
	{
		mVertexShader->Release();
		mVertexShader = NULL;
	}

	if (mPixelShader)
	{
		mPixelShader->Release();
		mPixelShader = NULL;
	}

	if (mInputLayout)
	{
		mInputLayout->Release();
		mInputLayout = NULL;
	}

	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = NULL;
	}
}

void D3D11Render::BeginScene(float red, float green, float blue, float alpha)
{
	// Setup the color to clear the buffer to.
	FLOAT backgroundColor[4] = {red, green, blue, alpha};

	// Clear the back buffer.
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, backgroundColor);

	RECT winRect;
	GetClientRect(mHwnd, &winRect);
	D3D11_VIEWPORT viewport = {0.0f, 0.0f, (FLOAT)(winRect.right - winRect.left), (FLOAT)(winRect.bottom - winRect.top), 0.0f, 1.0f};
	mDeviceContext->RSSetViewports(1, &viewport);

	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetInputLayout(mInputLayout);

	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);

	mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &mStride, &mOffset);

	mDeviceContext->Draw(mNumVerts, 0);
}

void D3D11Render::EndScene()
{
	// Present as fast as possible.
	mSwapChain->Present(0, 0);
}