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


#include "MenuState.h"

#include "StateSystem.h"
#include "GraphicsSystem.h"
#include "GuiSystem.h"
#include "zzzSndSystem.h"
#include "GlobalVars.h"

using namespace Ogre;

MenuState::MenuState()
{
}

MenuState::~MenuState()
{
}

void MenuState::Initialize()
{
	UISYS->LoadLayout("main");
	UISYS->SetCursor(true);
	UISYS->SetCrossHair(true);
	//button callbacks
	//MyGUI::ButtonPtr btnPlay = UISYS->GetGUI()->findWidget<MyGUI::Button>("btnPlay");
	//MyGUI::ButtonPtr btnExit = UISYS->GetGUI()->findWidget<MyGUI::Button>("btnExit");
	//btnPlay->eventMouseButtonClick = MyGUI::newDelegate(this, &MenuState::OnPlay);
	//btnExit->eventMouseButtonClick = MyGUI::newDelegate(this, &MenuState::OnExit);
	//SSYS->LoadState("game");
}

void MenuState::Finalize()
{
	UISYS->UnloadLayout();
}

void MenuState::Update()
{
}

void MenuState::OnPlay()
{
	ZSND->PlaySound("tick");
	SSYS->LoadState("game");
}

void MenuState::OnExit()
{
	ZSND->PlaySound("tick");
	GlobalVars::ShutDown = true;
}