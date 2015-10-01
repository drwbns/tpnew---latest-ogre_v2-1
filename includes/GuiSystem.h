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

#ifndef GuiSystem_H_
#define GuiSystem_H_

#include "OgreWindowEventUtilities.h"
#include "OgreSingleton.h"

class TextRenderer;
class Ogre::ManualObject;

#define UISYS GuiSystem::getSingletonPtr()

class GuiSystem : public Ogre::Singleton<GuiSystem>, public Ogre::GeneralAllocatedObject, public Ogre::WindowEventListener
{
public:
	GuiSystem();
	~GuiSystem();
	static GuiSystem& getSingleton();
	static GuiSystem* getSingletonPtr();

	void Initialize();
	void Finalize();
	void Update();
	void LoadLayout(std::string name);
	void UnloadLayout();

	//debug txt
	void InitDebuggers();
	void DeinitDebuggers();
	void UpdateDebuggers();

	//gets
	TextRenderer* GetGUI() { return mTxt; }

	//sets
	void SetCursor(bool show);
	void SetCrossHair(bool show);

private:
	TextRenderer* mTxt;
	Ogre::ManualObject* crossHair;
};

#endif