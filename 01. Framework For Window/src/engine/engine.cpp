//
//  engine.cpp
//  LearnDirect3D11
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#include "engine/engine.hpp"

Engine::Engine(/* args */)
{
    mInput = 0;
	mGfxDevice = 0;
}

Engine::Engine(const Engine& engine) 
{
}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	result = Awake(screenWidth, screenHeight);
	if(!result)
	{
		return false;
	}

	// Create the input object. This object will be used to handle reading the keyboard input from the user.
	mInput = new Input;
	if(!mInput)
	{
		return false;
	}

	// Initialize the input object.
	mInput->Awake();

	// Create the gfxdevice object. This object will handle rendering all the graphics for this application.
	mGfxDevice = new GfxDevice;
	if(!mGfxDevice)
	{
		return false;
	}

	// Initialize the gfxdevice object.
	result = mGfxDevice->Awake(screenWidth, screenHeight, mHwnd);
	if(!result)
	{
		return false;
	}
	
	return true;
}

void Engine::Quit()
{
	// Release the gfxdevice object.
	if(mGfxDevice)
	{
		mGfxDevice->OnDestroy();
		delete mGfxDevice;
		mGfxDevice = NULL;
	}

	// Release the input object.
	if(mInput)
	{
		delete mInput;
		mInput = NULL;
	}

	// Destroy the window.
	OnDestroy();
}

void Engine::RunLoop()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Update();
			if(!result)
			{
				done = true;
			}
		}
	}
}

bool Engine::Update()
{
	bool result;

	// Check if the user pressed escape and wants to exit the application.
	if(mInput->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the render object.
	result = mGfxDevice->Update();
	if(!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Engine::HandleMessage(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;
	switch(umsg)
	{
		case WM_KEYDOWN:
		{
			mInput->KeyDown((unsigned int)wparam);
			break;
		}
		case WM_KEYUP: 
		{
			mInput->KeyUp((unsigned int)wparam);
			break;
		}
		default: 
			result = DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	return result;
}

bool Engine::Awake(int& screenWidth, int& screenHeight)
{
	WNDCLASSEXW winClass;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.	
	sEngineInstance = this;

	// Get the instance of this application.
	mHinstance = GetModuleHandle(NULL);

	// Give the application a name.
	mAppName = L"STDXEngine";

	// Setup the windows class with default settings.
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WndProc;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;
	winClass.hInstance     = mHinstance;
	winClass.hIcon		   = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hIconSm       = winClass.hIcon;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.lpszClassName = mAppName;
	winClass.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	if(!RegisterClassEx(&winClass)) 
	{
        MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK); //GetLastError();
		return false;
    }

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN) 
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	} 
	else 
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth  = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	mHwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
							winClass.lpszClassName, 
							L"01. Framework For Window", 
							WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						    posX,
							posY, 
							screenWidth, 
							screenHeight, 
							NULL, 
							NULL, 
							mHinstance,
							NULL);

	if(!mHwnd) 
	{
        MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK); //GetLastError();
		return false;
    }

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(mHwnd, SW_SHOW);
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);

	return true;
}

void Engine::OnDestroy()
{
	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN) 
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(mHwnd);
	mHwnd = NULL;

	// Remove the application instance.
	UnregisterClass(mAppName, mHinstance);
	mHinstance = NULL;

	// Release the pointer to this class.
	mAppName = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;
    switch(umessage)
    {
        case WM_KEYDOWN:
        {
            if(wparam == VK_ESCAPE)
                DestroyWindow(hwnd);
            break;
        }
		case WM_CLOSE: // Check if the window is being closed.
		{
            PostQuitMessage(0);
            break;
        }
        case WM_DESTROY: // Check if the window is being destroyed.
        {
            PostQuitMessage(0);
            break;
        }
        default: // All other messages pass to the message handler in the system class.
            result = sEngineInstance->HandleMessage(hwnd, umessage, wparam, lparam);
    }
    return result;
}