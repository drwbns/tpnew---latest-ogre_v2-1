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

#ifndef Moving_H_
#define Moving_H_

class Path;

class Moving
{
	//friend class World;

public:
	enum Behaviour
	{
		brake      = 0x00001,
		seek       = 0x00002,
		arrive     = 0x00004,
		avoid1     = 0x00008,//obstacles
		avoid2     = 0x00010,//walls
		avoid3     = 0x00020,//others
		path       = 0x00040,
		seperate   = 0x00080,
	};

protected:
	Moving(Ogre::Vector3 position, float max_speed, float max_acc, float radius);
	virtual ~Moving();
	virtual void Update();

public:
	Ogre::Vector3 CalculateBrake();
	Ogre::Vector3 CalculateSeek();
	Ogre::Vector3 CalculateArrive();
	Ogre::Vector3 CalculateObstacleAvoidance();
	Ogre::Vector3 CalculateWallAvoidance();
	Ogre::Vector3 CalculateAgentAvoidance();
	Ogre::Vector3 CalculatePathFollow();
	Ogre::Vector3 CalculateSeperate();

	float GetMaxSpeed() { return MaxSpeed; }
	float GetRadius() { return Radius; }
	Ogre::Vector3 GetPosition() { return Position; }
	Ogre::Vector3 GetVelocity() { return Velocity; }
	Ogre::Vector3 GetAcceleration() { return Acceleration; }
	Ogre::Vector3 GetDest() { return Destination; }
	Ogre::Quaternion GetRotation() { return Rotation; }
	Ogre::Quaternion GetDirection() { return Direction; }
	void SetDirection(Ogre::Quaternion q) { Direction = q; }
	void SetMaxSpeed(float v) { MaxSpeed = v; }
	void SetDest(Ogre::Vector3 dest) { Destination = dest; }
	Path* GetPath() { return mPath; }
	void SetPath(Path* p) { mPath = p; }
	Path* GetPatrolPath() { return pPath; }
	bool isOnDest() { return Position == Destination; }

	bool isActive(Behaviour b)  { return (b & behavior)!=0; }
	void resetBehavior()	    { behavior = 0;   }
	void brakeOn()				{ behavior |= brake; }
	void brakeOf()				{ behavior ^= brake; }
	void seekOn()				{ behavior |= seek; }
	void seekOf()				{ behavior ^= seek; }
	void arriveOn()				{ behavior |= arrive; }
	void arriveOf()				{ behavior ^= arrive; }
	void avoid1On()				{ behavior |= avoid1; }
	void avoid1Of()				{ behavior ^= avoid1; }
	void avoid2On()				{ behavior |= avoid2; }
	void avoid2Of()				{ behavior ^= avoid2; }
	void avoid3On()				{ behavior |= avoid3; }
	void avoid3Of()				{ behavior ^= avoid3; }
	void pathOn()				{ behavior |= path; }
	void pathOf()				{ behavior ^= path; }
	void seperateOn()			{ behavior |= seperate; }
	void seperateOf()			{ behavior ^= seperate; }

protected:
	Ogre::Vector3 Position;
	Ogre::Vector3 Destination;
	Ogre::Vector3 Velocity;
	Ogre::Vector3 Acceleration;
	Ogre::Quaternion Direction;
	Ogre::Quaternion Rotation;

	float Radius;
	float MaxSpeed;
	float MaxAcceleration;
	int behavior;

	int PathPosition;
	Path* mPath;
	int PatrolPathPosition;
	Path* pPath;

public:
	static float MINSPD;
};

#endif