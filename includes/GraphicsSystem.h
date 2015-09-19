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

#ifndef GraphicsSystem_H_
#define GraphicsSystem_H_

#include "OgreWindowEventUtilities.h"
#include "OgreSingleton.h"

#define GSYS GraphicsSystem::getSingletonPtr()

class GraphicsSystem : public Ogre::Singleton<GraphicsSystem>, public Ogre::GeneralAllocatedObject, public Ogre::WindowEventListener
{
public:
	GraphicsSystem();
	~GraphicsSystem();
	static GraphicsSystem& getSingleton();
	static GraphicsSystem* getSingletonPtr();

	void Initialize();
	void Finalize();
	void Start();

	//windowEvent overrides
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);
	virtual void windowFocusChange(Ogre::RenderWindow* rw);
	virtual void eventOccurred(const Ogre::String& eventName, const Ogre::NameValuePairList* parameters);

	Ogre::Root* GetRoot()             { return mRoot; }
	Ogre::SceneManager* GetSceneMgr() { return mSceneMgr; }
	Ogre::RenderWindow* GetWindow()   { return mWindow; }
	Ogre::Camera* GetCamera()		  { return mCamera; }
	
private:
	Ogre::Root*			mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera*		mCamera;
};

#endif