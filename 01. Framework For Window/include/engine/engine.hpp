//
//  engine.hpp
//  LearnDirect3D11
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <windows.h>

#include "input.hpp"
#include "gfxdevice.hpp"

class Engine
{
private:
	LPCWSTR mAppName;
	HINSTANCE mHinstance;
	HWND mHwnd;

	Input* mInput;
	GfxDevice* mGfxDevice;

private:
	bool Awake(int&, int&);
	bool Update();
	void OnDestroy();

public:
    Engine(/* args */);
    Engine(const Engine&);
    ~Engine();

public:
	bool Initialize();
	void Quit();
	void RunLoop();
	LRESULT CALLBACK HandleMessage(HWND, UINT, WPARAM, LPARAM);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static Engine* sEngineInstance = 0;

#endif
