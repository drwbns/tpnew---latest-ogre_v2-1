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

#ifndef InputSystem_H_
#define InputSystem_H_

#define ISYS InputSystem::getSingletonPtr()

#include "OIS\OISKeyboard.h"
#include "OIS\OISMouse.h"

class InputSystem : Ogre::Singleton<InputSystem>, public Ogre::GeneralAllocatedObject, public OIS::KeyListener, public OIS::MouseListener
{
public:
	InputSystem();
	~InputSystem();
	static InputSystem& getSingleton();
	static InputSystem* getSingletonPtr();
	void Initialize();
	void Finalize();

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);
	bool processUnbufferedKeyInput(const Ogre::FrameEvent& evt);
	bool processUnbufferedMouseInput(const Ogre::FrameEvent& evt);
	void Update(const Ogre::FrameEvent& evt);
	
private:
	OIS::InputManager* mInputManager;
	OIS::Mouse*		   mMouse;
	OIS::Keyboard*	   mKeyboard;

};

#endif