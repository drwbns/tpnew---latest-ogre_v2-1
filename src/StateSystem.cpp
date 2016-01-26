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


#include "MenuState.h"
#include "GameState.h"
#include "StateSystem.h"

template<> StateSystem* Singleton<StateSystem>::msSingleton = nullptr;

StateSystem* StateSystem::getSingletonPtr(void)
{
	return msSingleton;
}

StateSystem& StateSystem::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );
}

StateSystem::StateSystem() : mState(nullptr)
{
}

StateSystem::~StateSystem()
{
	UnloadCurrentState();
}

void StateSystem::LoadState(String Name)
{
	UnloadCurrentState();

	if (Name == "menu")
	{
		mState = OGRE_NEW MenuState();
		mState->Initialize();
	}
	else if (Name == "game")
	{
		mState = OGRE_NEW GameState();
		mState->Initialize();
	}
}
	
void StateSystem::UnloadCurrentState()
{
	if (mState)
	{
		mState->Finalize();
		delete mState;
		mState = nullptr;
	}
}
	
void StateSystem::Update() const
{
	if (mState)
	{
		mState->Update();
	}
}
