//
//  gfxdevice.cpp
//  LearnDirect3D11
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#include "engine/gfxdevice.hpp"
#include "engine/d3d11_render.hpp"

GfxDevice::GfxDevice(/* args */)
{
	mD3D11Render = NULL;
}

GfxDevice::GfxDevice(const GfxDevice& /*gfxDevice*/)
{
}

GfxDevice::~GfxDevice()
{
}

bool GfxDevice::Awake(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	mD3D11Render = new D3D11Render;
	if(!mD3D11Render)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = mD3D11Render->Awake(screenWidth, screenHeight, hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GfxDevice::OnDestroy()
{
	// Release the D3D object.
	if(mD3D11Render)
	{
		mD3D11Render->OnDestroy();
		delete mD3D11Render;
		mD3D11Render = NULL;
	}
}

bool GfxDevice::Update()
{
	return OnRender();
}

bool GfxDevice::OnRender()
{	
	// Clear the buffers to begin the scene.
	mD3D11Render->BeginScene(1.0f, 0.0f, 0.0f, 1.0f);
	
	// Present the rendered scene to the screen.
	mD3D11Render->EndScene();
	return true;
}