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

#ifndef AgentInfo_H_
#define AgentInfo_H_

class Agent;

class AgentInfo
{
	friend class AIPerceptor;

public:
	enum Flag
	{
		IF_NEW,
		IF_UP2DATE,
		IF_OUTDATED,
		IF_LOST
	};

	AgentInfo(Agent* a);
	~AgentInfo();
	void Update();

	void SetAgent(Agent* a) { agent = a; }
	Agent* GetAgent() { return agent; }
	int GetID() { return agent->getID(); }
	void SetPosition(Ogre::Vector3 p) { position = p; }
	Ogre::Vector3 GetPosition() { return position; }
	void SetHP(int i) { hp = i; }
	int GetHP() { return hp; }
	void SetVisible(bool b) { visible = b; }
	bool IsVisible() { return visible; }
	void SetFlag(Flag f) { flag = f; }
	Flag GetFlag() { return flag; }

private:
	Agent* agent;
	Ogre::Vector3 position;
	int hp;
	bool visible;
	Flag flag;
};

#endif