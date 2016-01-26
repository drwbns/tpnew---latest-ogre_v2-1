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

#ifndef AIState_h_
#define AIState_h_

#include <vector>

class AIStateMachine;
class Agent;

class AIState
{
public:
	AIState(int id);
	virtual ~AIState();
	virtual void Enter(Agent * agent);
	virtual void Execute(Agent * agent);
	virtual void Exit(Agent * agent);
	virtual bool isReady() { return false; }
	void GetAvailables(AIStateMachine* sm, Agent* agent, std::vector<AIState*> &list);

	int GetId() const
	{ return id; }
	void AddChild(AIState* s) { children.push_back(s);s->SetParent(this); }
	AIState* GetChild(int i) { return children[i]; }
	void ClearChildren() { children.clear(); }
	int GetTotalChildren() const
	{ return (int)children.size(); }
	void SetParent(AIState* s) { parent = s; }
	AIState* GetParent() const
	{ return parent; }
	bool IsLeaf() const
	{ return children.empty(); }

protected:
	int id;
	AIState* parent;
	std::vector<AIState*> children;
};

#endif