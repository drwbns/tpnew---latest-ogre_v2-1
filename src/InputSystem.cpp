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

#include "InputSystem.h"

#include "GraphicsSystem.h"
#include "StateSystem.h"
#include "GameState.h"
#include "CamController.h"
#include "PhysicsSystem.h"
#include "World.h"
#include "InputController.h"
#include "GlobalVars.h"
#include <OGRE/OgreRenderWindow.h>
#include <OIS/OISInputManager.h>

using namespace Ogre;
using namespace OIS;

template<> InputSystem* Singleton<InputSystem>::msSingleton = nullptr;

InputSystem* InputSystem::getSingletonPtr(void)
{
	return msSingleton;
}

InputSystem& InputSystem::getSingleton(void)
{
	assert(msSingleton);  return (*msSingleton);
}

InputSystem::InputSystem() : mInputManager(nullptr), mMouse(nullptr), mKeyboard(nullptr)
{
}

InputSystem::~InputSystem()
{
	Finalize();
}

void InputSystem::Initialize()
{
	//init input manager
	ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
	GSYS->GetWindow()->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << static_cast<unsigned int>(windowHnd);
	pl.insert(make_pair(std::string("WINDOW"), windowHndStr.str()));
	mInputManager = InputManager::createInputSystem(pl);

	//create all devices
	mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject(OISKeyboard, true));
	mMouse = static_cast<Mouse*>(mInputManager->createInputObject(OISMouse, true));
	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//set mouse clip area
	OIS::MouseState ms = mMouse->getMouseState();
	int w = GSYS->GetWindow()->getWidth();
	int h = GSYS->GetWindow()->getHeight();
}

void InputSystem::Finalize()
{
	mInputManager->destroyInputObject(mKeyboard);
	mInputManager->destroyInputObject(mMouse);
	InputManager::destroyInputSystem(mInputManager);
	mKeyboard = nullptr;
	mMouse = nullptr;
	mInputManager = nullptr;
}

void InputSystem::Update(const FrameEvent& evt) const
{
	mKeyboard->capture();
	mMouse->capture();
	processUnbufferedKeyInput(evt);
	processUnbufferedMouseInput(evt);
}

bool InputSystem::mouseMoved(const OIS::MouseEvent &arg)
{
	//UISYS->GetGUI()->injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
	return true;
}

bool InputSystem::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	//UISYS->GetGUI()->injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));

	if (SSYS->GetCurrentState()->GetName() == "game")
	{
		static_cast<InputController*>(WORLD->getPlayerAgent()->getController())->injectMousePress(id, true);
	}
	return true;
}

bool InputSystem::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	//UISYS->GetGUI()->injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));

	if (SSYS->GetCurrentState()->GetName() == "game")
	{
		static_cast<InputController*>(WORLD->getPlayerAgent()->getController())->injectMouseRelease(id);
	}
	return true;
}

bool InputSystem::keyPressed(const OIS::KeyEvent &arg)
{
	//UISYS->GetGUI()->injectKeyPress(MyGUI::KeyCode::Enum(arg.key), arg.text);

	if (arg.key == OIS::KC_ESCAPE)
	{
		GlobalVars::ShutDown = true;
	}

	if (SSYS->GetCurrentState()->GetName() == "game")
	{
		static_cast<InputController*>(WORLD->getPlayerAgent()->getController())->injectKeyPress(arg.key);

		if (arg.key == KC_C)
		{
			GAMESTATE->NextCamera();
		}
		else if (arg.key == KC_Q)
		{
			PHY->FlipDebug();
		}
		// Set visibility of Nav Mesh (debug purposes)
		else if (arg.key == KC_E)
		{
			GSYS->GetSceneMgr()->getManualObject("mNavMesherDebugger")->getVisible() ?
				GSYS->GetSceneMgr()->getManualObject("mNavMesherDebugger")->setVisible(false)
				:
				GSYS->GetSceneMgr()->getManualObject("mNavMesherDebugger")->setVisible(true);
		}
	}

	return true;
}

bool InputSystem::keyReleased(const OIS::KeyEvent &arg)
{
	//UISYS->GetGUI()->injectKeyRelease(MyGUI::KeyCode::Enum(arg.key));

	if (SSYS->GetCurrentState()->GetName() == "game")
	{
		static_cast<InputController*>(WORLD->getPlayerAgent()->getController())->injectKeyRelease(arg.key);
	}

	return true;
}

bool InputSystem::processUnbufferedKeyInput(const FrameEvent& evt) const
{
	if (SSYS->GetCurrentState()->GetName() == "game")
	{
		static_cast<InputController*>(WORLD->getPlayerAgent()->getController())->injectKeyboardState(mKeyboard);
	}

	if (mKeyboard->isKeyDown(KC_U))GlobalVars::test1 += 0.01;
	if (mKeyboard->isKeyDown(KC_J))GlobalVars::test1 -= 0.01;
	if (mKeyboard->isKeyDown(KC_I))GlobalVars::test2 += 0.01;
	if (mKeyboard->isKeyDown(KC_K))GlobalVars::test2 -= 0.01;
	if (mKeyboard->isKeyDown(KC_O))GlobalVars::test3 += 0.01;
	if (mKeyboard->isKeyDown(KC_L))GlobalVars::test3 -= 0.01;

	return true;
}

bool InputSystem::processUnbufferedMouseInput(const FrameEvent& evt) const
{
	if (SSYS->GetCurrentState()->GetName() == "game")
	{
		MouseState ms = mMouse->getMouseState();
		GAMESTATE->GetCurrentCam()->UpdateRotation();

		if (ms.buttonDown(MB_Middle))
		{
			GAMESTATE->GetCurrentCam()->SetSpeed(32);
		}
		else
		{
			GAMESTATE->GetCurrentCam()->SetSpeed(8);
		}

		if (ms.buttonDown(MB_Left))
		{
			static_cast<InputController*>(WORLD->getPlayerAgent()->getController())->injectMousePress(OIS::MB_Left, false);
		}
	}
	return true;
}