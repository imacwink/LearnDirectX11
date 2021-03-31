//
//  gfxdevice.cpp
//  LearnDirect3D11
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#include "engine/gfxdevice.hpp"

GfxDevice::GfxDevice(/* args */)
{
}

GfxDevice::GfxDevice(const GfxDevice& gfxDevice)
{
}

GfxDevice::~GfxDevice()
{
}

bool GfxDevice::Awake(int screenWidth, int screenHeight, HWND hwnd)
{
	return true;
}

void GfxDevice::OnDestroy()
{
}

bool GfxDevice::Update()
{
	return true;
}

bool GfxDevice::OnRender()
{
	return true;
}