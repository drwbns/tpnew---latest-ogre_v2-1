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

#ifndef CamController_H_
#define CamController_H_

#include "OGRE\OgreMemoryAllocatorConfig.h"
#include "OGRE\OgreCamera.h"

class CamController : public Ogre::GeneralAllocatedObject
{
public:
	enum Type
	{
		CT_FREE,
		CT_RTS,
		CT_TPS
	};

	CamController();
	virtual ~CamController();
	virtual void Initialize(Ogre::Camera* camera);
	virtual void Finalize();
	virtual void UpdateLocation() = 0;
	virtual void UpdateRotation() = 0;

	void SetSpeed(float f)     { mSpeed = f; }
	void SetSensivity(float f) { mSensivity = f; }
	Type GetType() const
	{ return type; }
	Ogre::Camera* GetCamera() const
	{ return mCamera; }

protected:
	Type type;
	Ogre::Camera* mCamera;
	float mSpeed;
	float mSensivity;

};

#endif