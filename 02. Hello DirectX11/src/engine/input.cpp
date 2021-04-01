//
//  input.cpp
//  LearnDirect3D11
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#include "engine/input.hpp"

Input::Input(/* args */)
{
}

Input::Input(const Input& /*input*/)
{
}

Input::~Input()
{
}

void Input::Awake()
{
	int i;
	
	// Initialize all the keys to being released and not pressed.
	for(i = 0; i < 256; i++) 
	{
		mKeys[i] = false;
	}

	return;
}

void Input::OnDestroy()
{
}

void Input::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	mKeys[input] = true;
	return;
}

void Input::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	mKeys[input] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return mKeys[key];
}