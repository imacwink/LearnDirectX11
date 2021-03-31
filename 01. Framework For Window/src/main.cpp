//
//  main.cpp
//  LearnDirect3D11
//
//  Created by macwink on 2021/3/31.
//  Copyright © 2021 StoneUS. All rights reserved.
//

#include "engine/engine.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
	// Create the system object.
	Engine* engine = new Engine;
	if(engine)
	{
        // Initialize and run the system object.
        bool result = engine->Initialize();
        if(result)
        {
            engine->RunLoop();
        }

        // Shutdown and release the system object.
        engine->Quit();
        delete engine;
        engine = NULL;
	}

	return 0;
}