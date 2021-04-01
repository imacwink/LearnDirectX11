//
//  input.hpp
//  LearnDirect3D11
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#ifndef _INPUT_H_
#define _INPUT_H_

#include <windows.h>

class Input
{
private:
    bool mKeys[256];

private:
    /* Func */

public:
    Input(/* args */);
    Input(const Input&);
    ~Input();

public:
    void Awake();
    void OnDestroy();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);
};

#endif