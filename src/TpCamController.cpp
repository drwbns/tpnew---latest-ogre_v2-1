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

#include "StdAfx.h"
#include "CamController.h"
#include "TpCamController.h"
#include "GameState.h"
#include "StateSystem.h"
#include "World.h"
#include "PhysicsSystem.h"
using namespace Ogre;

TpCamController::TpCamController() : CamController()
{
	type = CT_TPS;
	curCamOffset = Vector3(0,0,-1);
	farCamOffset = Vector3(0,0,-1);
	farDestOffset = Vector3::ZERO;
	closeCamOffset = Vector3(0,0,-1);
	closeDestOffset = Vector3::ZERO;
	rotation = Quaternion::IDENTITY;
	yaw = 0;
	pitch = 0;
	closeSpeed = 4.0;
	aim = false;
}

TpCamController::~TpCamController()
{
	Finalize();
}

void TpCamController::Initialize(Ogre::Camera* camera)
{
	CamController::Initialize(camera);
}

void TpCamController::Finalize()
{
	CamController::Finalize();
}

void TpCamController::UpdateLocation(float mWalk, float mStrafe, float mUp)
{
	Vector3 targetPosition = Vector3::ZERO;
	Quaternion targetRotation = Quaternion::IDENTITY;

	if (PLAYERID > -1)
	{
		targetPosition = PLAYER->GetPosition();
		if (aim)
		{
			targetRotation = PLAYER->GetRotation();
		}
		else
		{
			targetRotation.FromAngleAxis(Radian(yaw), Vector3::UNIT_Y);
		}
	}

	//position
	Vector3 mPos =  targetPosition;
	mPos.y += 1.0;
	//float inc = GlobalVars::Tick * 4;
	//if (inc > 1)inc = 1;
	//rotation = Quaternion::Slerp(inc, rotation, targetRotation, true);
	rotation = targetRotation;
	
	if (aim && curCamOffset.squaredDistance(closeCamOffset) > 0.1)
	{
		Vector3 delta = closeCamOffset - curCamOffset;
		curCamOffset += delta * closeSpeed * GlobalVars::Tick;
	}
	else if (!aim && curCamOffset.squaredDistance(farCamOffset) > 0.1)
	{
		Vector3 delta = farCamOffset - curCamOffset;
		curCamOffset += delta * closeSpeed * GlobalVars::Tick;
	}

	Quaternion q;
	q.FromAngleAxis(Radian(pitch), Vector3::UNIT_X);
	Vector3 addPos = rotation * q * curCamOffset;
	Vector3 castPos = PHY->CastRay1(mPos, addPos.normalisedCopy());
	float dist = castPos.distance(mPos);
	float skinWidth2 = 0.25;
	if (dist - skinWidth2 < addPos.length())
	{
		//keep y
		float ty = mPos.y + addPos.y;
		mPos += addPos.normalisedCopy() * (dist-skinWidth2);
		mPos.y = ty;
	}
	else
	{
		mPos += addPos;
	}

	mCamera->setPosition(mPos);

	//target
	Vector3 nDest;
	if (aim)
	{
		nDest = targetPosition + rotation * q * closeDestOffset;
	}
	else
	{
		nDest = targetPosition + rotation * q * farDestOffset;
	}
	mCamera->lookAt(nDest);
}

void TpCamController::UpdateRotation(float mPitch, float mYaw)
{
	if (PLAYERID > -1)
	{
		yaw += mYaw * mSensivity;
		Quaternion q;
		q.FromAngleAxis(Radian(yaw), Vector3::UNIT_Y);
		if (aim)PLAYER->SetDirection(q);
		else if (PLAYER->GetVelocity().length() > Moving::MINSPD)
		{
			PLAYER->SetDirection(PLAYER->GetVelocity().getRotationTo(Vector3::UNIT_Z).Inverse());
		}

		pitch -= mPitch * mSensivity * 0.5;
		if (pitch < -0.70)pitch = -0.70;
		else if (pitch > 0.5)pitch = 0.5;
	}
}