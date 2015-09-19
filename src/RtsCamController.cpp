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



#include "RtsCamController.h"
using namespace Ogre;

RtsCamController::RtsCamController() : CamController()
{
	type = CT_RTS;
}

RtsCamController::~RtsCamController()
{
	Finalize();
}

void RtsCamController::Initialize(Ogre::Camera* camera)
{
	CamController::Initialize(camera);
}

void RtsCamController::Finalize()
{
	CamController::Finalize();
}

void RtsCamController::UpdateLocation(float mWalk, float mStrafe, float mUp)
{
	Vector3 mVelocity(0,0,0);
	mVelocity.z = -mSpeed * mWalk * GlobalVars::Tick;
	mVelocity.x = -mSpeed * mStrafe * GlobalVars::Tick;
	mVelocity.y = mSpeed * mUp * GlobalVars::Tick;
	mCamera->move(mVelocity);
}

void RtsCamController::UpdateRotation(float mPitch, float mYaw)
{
	Vector3 mDelta(0,-1,1);
	mCamera->lookAt(mCamera->getPosition() + mDelta);
}