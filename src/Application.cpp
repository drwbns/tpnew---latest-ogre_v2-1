/*
Copyright (c) 2010 Yunus Kara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "Application.h"
#include "GlobalVars.h"

#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "StateSystem.h"
#include "GuiSystem.h"
#include "zzzSndSystem.h"
#include "ProjectileManager.h"
#include "PhysicsSystem.h"
#include "BillboardSystem.h"
#include "ParticleManager.h"

template<> Application* Ogre::Singleton<Application>::msSingleton = nullptr;

Application* Application::getSingletonPtr(void)
{
	return msSingleton;
}

Application& Application::getSingleton(void)
{
	assert(msSingleton);  return (*msSingleton);
}

Application::Application() : GS(nullptr), IS(nullptr), SS(nullptr), GU(nullptr), SN(nullptr), PM(nullptr), PS(nullptr), BB(nullptr), PAM(nullptr)
{
}

Application::~Application()
{
	Finalize();
}

void Application::Initialize()
{
	GS = new GraphicsSystem();
	IS = new InputSystem();
	SS = new StateSystem();
	GU = new GuiSystem();
	SN = new zzzSndSystem();
	PM = new ProjectileManager();
	PS = new PhysicsSystem();
	BB = new BillboardSystem();
	PAM = new ParticleManager();

	GS->Initialize();
	IS->Initialize();
	GU->Initialize();
	SN->Initialize();
	PM->Initialize();
	//PS->Initialize();//do it in states
	//BB->Initialize();//do it in states
	//PAM->Initialize();//do it in states

	//go
	SS->LoadState("game");
	GS->Start();
}

void Application::Finalize() const
{
	//PS->Finalize();//do it in states
	//BB->Finalize();//do it in states
	//PAM->Finalize();//do it in states
	SS->UnloadCurrentState();
	PM->Finalize();
	IS->Finalize();
	GU->Finalize();
	SN->Finalize();
	GS->Finalize();
}

bool Application::frameStarted(const Ogre::FrameEvent& evt)
{
	//ticks
	GlobalVars::Tick = evt.timeSinceLastFrame;
	if (GlobalVars::Tick > 1.0)
	{
		GlobalVars::Tick = 1.0;
	}

	//4 fps tick
	GlobalVars::Tick_4 += GlobalVars::Tick;
	if (GlobalVars::Tick_4 > 0.25)
	{
		GlobalVars::Tick_4 = 0;
	}

	//10 fps tick
	GlobalVars::Tick_10 += GlobalVars::Tick;
	if (GlobalVars::Tick_10 > 0.10)
	{
		GlobalVars::Tick_10 = 0;
	}

	//PS->Update();//do it in states
	//BB->Update();//do it in states
	//PAM->Update();//do it in states
	SS->Update();
	IS->Update(evt);
	GU->Update();
	SN->Update();
	PM->Update();
	return true;
}

bool Application::frameEnded(const Ogre::FrameEvent& evt)
{
	return !GlobalVars::ShutDown;
}