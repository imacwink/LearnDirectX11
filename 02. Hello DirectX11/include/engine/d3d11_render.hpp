//
//  d3d11_render.cpp
//  LearnDirect3D11
//
// #pragma comment(lib, "dxgi.lib")
// #pragma comment(lib, "d3d11.lib")
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#ifndef _D3D11_H_
#define _D3D11_H_

#include <d3d11.h>
#include <assert.h>

class D3D11Render
{
private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;

private:
	bool CreateDeviceAndContext();
	bool CreateSwapChain(int, int, HWND);
	bool CreateFramebuffer();

public:
    D3D11Render(/* args */);
    D3D11Render(const D3D11Render&);
    ~D3D11Render();

public:
	bool Awake(int, int, HWND);
	void OnDestroy();

	void BeginScene(float, float, float, float);
	void EndScene();
};

#endif
