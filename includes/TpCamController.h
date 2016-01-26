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

#ifndef TpCamController_H_
#define TpCamController_H_

#include "CamController.h"
#include "OgreSingleton.h"
#include <OGRE/OgreVector3.h>

#define TPCAM TpCamController::getSingletonPtr()

class TpCamController : public Ogre::Singleton<TpCamController>, public CamController
{
public:
	TpCamController();
	~TpCamController();
	void Initialize(Ogre::Camera* camera) override;
	void Finalize() override;
	void UpdateLocation() override;
	void UpdateRotation() override;

	void SetFarCamOffset(Ogre::Vector3 v) { farCamOffset = v; }
	void SetFarDestOffset(Ogre::Vector3 v) { farDestOffset = v; }
	void SetCloseCamOffset(Ogre::Vector3 v) { closeCamOffset = v; }
	void SetCloseDestOffset(Ogre::Vector3 v) { closeDestOffset = v; }
	void SetCloseSpeed(float f) { closeSpeed = f; }
	void SetAimMode(bool b) { aim = b; }
	Ogre::Quaternion GetRotation() const
	{ return rotation; }

private:
	Ogre::Quaternion rotation;
	Ogre::Vector3 curCamOffset;
	Ogre::Vector3 farCamOffset;
	Ogre::Vector3 farDestOffset;
	Ogre::Vector3 closeCamOffset;
	Ogre::Vector3 closeDestOffset;
	float yaw;
	float pitch;
	float closeSpeed;
	bool aim;
	float mYaw;
	float mPitch;
};

#endif