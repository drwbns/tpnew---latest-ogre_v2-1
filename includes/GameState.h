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

#ifndef GameState_H_
#define GameState_H_

#include "State.h"

#include "CamController.h"
#include "World.h"
#include "Level.h"

#include <vector>
#include "OgreSingleton.h"

#define GAMESTATE GameState::getSingletonPtr()

class GameState : public Singleton<GameState>, public State
{
public:
	GameState();
	~GameState();

	void Initialize();
	void Finalize();
	void Update();

	//gets & sets
	static String GetName()
	{ return "game"; }
	CamController* GetCurrentCam() { return cams[ccam]; }
	void NextCamera() { ++ccam %= cams.size(); }
	World* getWorld() const 
	{ return mWorld; }
	Level* getLevel() const 
	{ return mLevel; }

private:
	int ccam;
	std::vector<CamController*> cams;
	World* mWorld;
	Level* mLevel;
};

#endif