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

#ifndef AIPerceptor_H_
#define AIPerceptor_H_

#include "OGRE/OgreVector3.h"
#include "OGRE\OgreSingleton.h"

class Agent;
#define PRC AIPerceptor::getSingletonPtr()

class AIPerceptor : Ogre::Singleton<AIPerceptor>
{
public:
	static AIPerceptor& getSingleton();
	static AIPerceptor* getSingletonPtr();
	AIPerceptor();
	~AIPerceptor();
	bool CanSee(Agent* agent1, Agent* agent2) const;
	bool CanSee(Agent* agent1, Ogre::Vector3 position) const;
	void UpdateKnowledge(Agent* agent) const;
	static void FindCovers(Agent* agent);
};

#endif