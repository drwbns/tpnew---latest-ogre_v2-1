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

#ifndef RtsCamController_H_
#define RtsCamController_H_

#include "CamController.h"
#include "OgreSingleton.h"

#define RTSCAM TpCamController::getSingletonPtr()

class RtsCamController : public Ogre::Singleton<RtsCamController>, public CamController
{
public:
	RtsCamController();
	~RtsCamController();
	void Initialize(Ogre::Camera* camera) override;
	void Finalize() override;
	void UpdateLocation() override;
	void UpdateRotation() override;
	Ogre::Real mWalk;
	Ogre::Real mStrafe;
	Ogre::Real mUp;
};

#endif