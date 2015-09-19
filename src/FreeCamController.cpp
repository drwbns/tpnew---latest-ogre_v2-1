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


#include "CamController.h"
#include "FreeCamController.h"
using namespace Ogre;

FreeCamController::FreeCamController() : CamController()
{
	type = CT_FREE;
}

FreeCamController::~FreeCamController()
{
	Finalize();
}

void FreeCamController::Initialize(Ogre::Camera* camera)
{
	CamController::Initialize(camera);
}

void FreeCamController::Finalize()
{
	CamController::Finalize();
}

void FreeCamController::UpdateLocation(float mWalk, float mStrafe, float mUp)
{
	Vector3 mVelocity(0,0,0);
	mVelocity.z = mSpeed * mWalk * GlobalVars::Tick;
	mVelocity.x = mSpeed * mStrafe * GlobalVars::Tick;
	mCamera->moveRelative(mVelocity);
}

void FreeCamController::UpdateRotation(float mPitch, float mYaw)
{
	Radian mRotX = Radian(mPitch * mSensivity);
	Radian mRotY = Radian(mYaw   * mSensivity);
	mCamera->pitch(mRotX);
	mCamera->yaw(mRotY);
}