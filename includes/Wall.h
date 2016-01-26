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

#ifndef Wall_H_
#define Wall_H_

#include "OGRE\OgrePrerequisites.h"
#include "OgreVector3.h"

class Wall
{
	friend class World;

	Wall(Ogre::Vector3 & from, Ogre::Vector3 & to, bool leftnormal);
	~Wall();
	
public:
	Ogre::Vector3 & GetFrom();
	Ogre::Vector3 & GetTo();
	Ogre::Vector3 & GetNormal();

private:
	Ogre::Vector3 From;
	Ogre::Vector3 To;
	Ogre::Vector3 Normal;
	Ogre::ManualObject* mark;
};

#endif