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


#include "InputController.h"
#include "GuiSystem.h"
#include "CamController.h"
#include "TpCamController.h"
#include "StateSystem.h"
#include "GameState.h"

#include "OIS\OISMouse.h"

using namespace Ogre;

InputController::InputController() : BaseController()
{
}

InputController::~InputController()
{
}

void InputController::Update()
{
}

void InputController::injectMousePress(int id, bool buffered)
{
	if (id == OIS::MB_Right && GST->GetCurrentCam()->GetType() == CamController::CT_TPS)
	{
		TPCAM->SetAimMode(true);
		UISYS->SetCrossHair(true);
		WORLD->getPlayerAgent()->SetAimMode(true);
	}
	else if (id == OIS::MB_Left)
	{
		WORLD->getPlayerAgent()->Shoot(buffered);
	}
}

void InputController::injectMouseRelease(int id)
{
	if (id == OIS::MB_Right && GST->GetCurrentCam()->GetType() == CamController::CT_TPS)
	{
		TPCAM->SetAimMode(false);
		UISYS->SetCrossHair(false);
		WORLD->getPlayerAgent()->SetAimMode(false);
	}
}

void InputController::injectKeyPress(int key)
{
}

void InputController::injectKeyRelease(int key)
{
	if (key == OIS::KC_UP || key == OIS::KC_DOWN || key == OIS::KC_LEFT || key == OIS::KC_RIGHT || key == OIS::KC_RSHIFT || key == OIS::KC_LSHIFT || key == OIS::KC_A || key == OIS::KC_S || key == OIS::KC_D || key == OIS::KC_W)
	{
		WORLD->getPlayerAgent()->orderBrake();
	}
}

void InputController::injectKeyboardState(OIS::Keyboard* mKeyboard)
{
	//camera
	float walk = 0,strafe = 0, up = 0;
	/*
	if (mKeyboard->isKeyDown(OIS::KC_A))
	{
		strafe = -1;
	}
	else if (mKeyboard->isKeyDown(OIS::KC_D))
	{
		strafe = 1;
	}
	if (mKeyboard->isKeyDown(OIS::KC_W))
	{
		walk = -1;
	}
	else if (mKeyboard->isKeyDown(OIS::KC_S))
	{
		walk = 1;
	}
	if (mKeyboard->isKeyDown(OIS::KC_PGUP))
	{
		up = 1;
	}
	else if (mKeyboard->isKeyDown(OIS::KC_PGDOWN))
	{
		up = -1;
	}
	*/
	GST->GetCurrentCam()->UpdateLocation(walk, strafe, up);

	//player movement
	walk = 0, strafe = 0;

	//Sprint if SHIFT is held
	if ((mKeyboard->isKeyDown(OIS::KC_LSHIFT) || mKeyboard->isKeyDown(OIS::KC_RSHIFT)) && (mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_A) || mKeyboard->isKeyDown(OIS::KC_S) || mKeyboard->isKeyDown(OIS::KC_D) || mKeyboard->isKeyDown(OIS::KC_W))) {
		WORLD->getPlayerAgent()->SetMaxSpeed(10);
	}
	else if(!mKeyboard->isKeyDown(OIS::KC_LSHIFT) || !mKeyboard->isKeyDown(OIS::KC_RSHIFT)) { WORLD->getPlayerAgent()->SetMaxSpeed(6); } // Run if not Sprinting

	if (mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) && (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) || !mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
	{
		walk = 1;
	}
	else if (mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) && (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) || !mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
	{
		walk = -1;
	}
	if (mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A) && (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) || !mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
	{
		strafe = 1;
	}
	else if (mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D) && (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) || !mKeyboard->isKeyDown(OIS::KC_RSHIFT)))
	{
		strafe = -1;
	}

	if (walk != 0 || strafe != 0)
	{
		Quaternion q = TPCAM->GetRotation().Inverse();
		Ogre::Vector3 send(walk,0,strafe);
		send = q * send;
		WORLD->getPlayerAgent()->orderMove(send.x,send.z);
	}
}