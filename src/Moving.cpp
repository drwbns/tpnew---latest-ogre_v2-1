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

#include "stdafx.h"
#include "World.h"
#include "PtfNode.h"
#include "GameState.h"
#include "StateSystem.h"
using namespace Ogre;

float Moving::MINSPD = 0.1f;

Moving::Moving(Vector3 position, float max_speed, float max_acc, float radius)
{
	Position = position;
	Destination = position;
	Velocity = Vector3::ZERO;
	Acceleration = Vector3::ZERO;
	Direction = Quaternion::IDENTITY;
	Rotation = Quaternion::IDENTITY;
	MaxSpeed = max_speed;
	MaxAcceleration = max_acc;
	Radius = radius;
	PathPosition = 0;
	PatrolPathPosition = 0;
	mPath = new Path();
	pPath = new Path();
	resetBehavior();
}

Moving::~Moving()
{
	delete mPath;
	mPath = NULL;
	delete pPath;
	pPath = NULL;
}

void Moving::Update()
{
	MINSPD = GlobalVars::Tick * 16;

	Acceleration = Vector3::ZERO;
	float power = 0;
	float mcoef = 2;
	if (power < mcoef*MaxAcceleration && isActive(avoid2))
		Acceleration += 4.0 * CalculateWallAvoidance();

	power = Acceleration.length();
	if (power < mcoef*MaxAcceleration && isActive(avoid1))
		Acceleration += 1.5 * CalculateObstacleAvoidance();

	power = Acceleration.length();
	if (power < mcoef*MaxAcceleration && isActive(avoid3))
		Acceleration += 1.0 * CalculateAgentAvoidance();

	power = Acceleration.length();
	if (power < mcoef*MaxAcceleration && isActive(seperate))
		Acceleration += 1.5 * CalculateSeperate();

	power = Acceleration.length();
	if (power < mcoef*MaxAcceleration && isActive(seek))
		Acceleration += 1.0 * CalculateSeek();

	power = Acceleration.length();
	if (power < mcoef*MaxAcceleration && isActive(arrive))
		Acceleration += 1.0 * CalculateArrive();

	power = Acceleration.length();
	if (power < mcoef*MaxAcceleration && isActive(path))
		Acceleration += 1.0 * CalculatePathFollow();

	power = Acceleration.length();
	if (power < mcoef*MaxAcceleration && isActive(brake))
		Acceleration += 2.0 * CalculateBrake();

	Acceleration.y = 0;
	Velocity += Acceleration * GlobalVars::Tick;

	float cspd = Velocity.length();
	if (cspd > MaxSpeed)
	{
		Velocity.normalise();
		Velocity *= MaxSpeed;
	}
	else if (cspd < MINSPD)
	{
		Velocity = Vector3::ZERO;
		Destination = Position;
	}

	if (cspd >= MINSPD)
	{
		Position += Velocity * GlobalVars::Tick;
		//Rotation = Velocity.getRotationTo(Vector3::UNIT_Z).Inverse();
	}

	//smooth out rotation changes
	float inc = GlobalVars::Tick * 8;
	if (inc > 1)inc = 1;
	Rotation = Quaternion::Slerp(inc, Rotation, Direction, true);
}

Vector3 Moving::CalculateSeek()
{
	Vector3 delta = Destination - Position;
	return delta.normalisedCopy() * MaxAcceleration;
}

Vector3 Moving::CalculateBrake()
{
	if (Velocity.length() > MINSPD)
	{
		return Velocity.normalisedCopy() * -0.2 * MaxAcceleration;
	}
	return Vector3::ZERO;
}

Vector3 Moving::CalculateArrive()
{
	Vector3 delta = Destination - Position;
	float distance = delta.length();
	if (distance > 0)
	{
		float slowRadius = 1.0;
		if (distance > slowRadius)
		{
			return delta.normalisedCopy() * MaxAcceleration;
		}
		else
		{
			float targetspeed = MaxSpeed * distance / slowRadius;
			Vector3 targetVel = delta.normalisedCopy() * targetspeed;
			Vector3 deltaVel = targetVel - Velocity;
			return deltaVel / GlobalVars::Tick;
		}
	}
	return Vector3::ZERO;
}

Vector3 Moving::CalculateObstacleAvoidance()
{
	Vector3 ret = Vector3::ZERO;

	//prepare ray
	Ogre::Ray r;
	Vector3 tpos = Position;
	r.setDirection(Velocity.normalisedCopy());

	//for each obstacle
	for (int i=0;i<WORLD->getObstacleTotal();i++)
	{
		//if close enough
		float dist = Position.distance(WORLD->getObstacle(i)->GetPos());
		float mindist = Radius + WORLD->getObstacle(i)->GetRadius();

		if (dist < mindist)
		{
			tpos.y = WORLD->getObstacle(i)->GetPos().y;
			r.setOrigin(tpos);
			Vector3 intersection;
			bool result = AIUtility::IntersectLineCircle(WORLD->getObstacle(i)->GetPos(),WORLD->getObstacle(i)->GetRadius(), r.getOrigin(), r.getOrigin()+dist*r.getDirection(), intersection);
			if (result)
			{
				//find if we need to turn left or right to avoid
				//from agent to obstacle center
				Vector3 v1 = WORLD->getObstacle(i)->GetPos() - r.getOrigin();
				//from collision point to obstacle center
				Vector3 v2 = WORLD->getObstacle(i)->GetPos() - intersection;
				//find angle between v1 & v2
				float angle = v1.getRotationTo(v2).getYaw().valueDegrees();
				//left
				if (angle <= 0)
				{
					Quaternion q;
					q.FromAngleAxis(Radian(Math::fDeg2Rad * 90), Vector3::UNIT_Y);
					ret = q * r.getDirection();
				}
				//right
				else if (angle > 0)
				{
					Quaternion q;
					q.FromAngleAxis(Radian(Math::fDeg2Rad * -90), Vector3::UNIT_Y);
					ret = q * r.getDirection();
				}
				break;
			}
		}
	}

	return ret.normalisedCopy() * MaxAcceleration;
}

Vector3 Moving::CalculateWallAvoidance()
{
	Vector3 ret = Vector3::ZERO;

	//for each wall
	for (int i=0;i<WORLD->getWallTotal();i++)
	{
		//test intersection
		Vector3 line1start = Position;
		Vector3 line1end = Position + Velocity;

		Vector3 line2start = WORLD->getWall(i)->GetFrom();
		Vector3 line2end = WORLD->getWall(i)->GetTo();

		Vector3 d1 = line1end - line1start;
		Vector3 d2 = line2end - line2start;
		float mangle = d1.getRotationTo(d2).getYaw().valueDegrees();

		//test intersection only if angle between velocity and wall normal is positive
		if (mangle > 0)
		{
			Vector3 result = AIUtility::LineIntersect(line1start,line1end,line2start,line2end);
			float dist = result.distance(Position);
			float mindist = 2 * Radius;

			if (result != Vector3(-1,-1,-1) && dist < mindist)
			{
				//find if we need to turn left or right to avoid
				//from agent to intersection
				Vector3 v1 = result - Position;
				//wall normal
				Vector3 v2 = WORLD->getWall(i)->GetNormal();
				//find angle between v1 & v2
				float angle = v1.getRotationTo(v2).getYaw().valueDegrees();
				//left
				if (angle > 0)
				{
					//Quaternion q;
					//q.FromAngleAxis(Radian(Math::fDeg2Rad * mangle), Vector3::UNIT_Y);
					//ret = q * Velocity;
					Quaternion q;
					q.FromAngleAxis(Radian(Math::fDeg2Rad * 90), Vector3::UNIT_Y);
					ret = q * d2;
					ret.y = 0;
					//ret = d2;
				}
				//right
				else
				{
					//Quaternion q;
					//q.FromAngleAxis(Radian(Math::fDeg2Rad * -mangle), Vector3::UNIT_Y);
					//ret = q * Velocity;
					Quaternion q;
					q.FromAngleAxis(Radian(Math::fDeg2Rad * 90), Vector3::UNIT_Y);
					ret = q * d2;
					ret.y = 0;
					//ret = (-d2);
				}
				break;
			}
		}
	}

	return ret.normalisedCopy() * MaxAcceleration;
}

Vector3 Moving::CalculateAgentAvoidance()
{
	Vector3 ret = Vector3::ZERO;

	//prepare ray
	Ogre::Ray r;
	Vector3 tpos = Position;
	r.setDirection(Velocity.normalisedCopy());

	//for each obstacle
	for (int i=0;i<WORLD->getAgentTotal();i++)
	{
		//if close enough
		float dist = Position.distance(WORLD->getAgent(i)->GetPosition());
		float mindist = Radius + WORLD->getAgent(i)->GetRadius() + AIConsts::PlayerTouchDistance2;

		if (dist > 0 && dist < mindist)
		{
			tpos.y = WORLD->getAgent(i)->GetPosition().y;
			r.setOrigin(tpos);
			Vector3 intersection;
			bool result = AIUtility::IntersectLineCircle(WORLD->getAgent(i)->GetPosition(),WORLD->getAgent(i)->GetRadius(), r.getOrigin(), r.getOrigin()+dist*r.getDirection(), intersection);
			if (result)
			{
				//find if we need to turn left or right to avoid
				//from agent to obstacle center
				Vector3 v1 = WORLD->getAgent(i)->GetPosition() - r.getOrigin();
				//from collision point to obstacle center
				Vector3 v2 = WORLD->getAgent(i)->GetPosition() - intersection;
				//find angle between v1 & v2
				float angle = v1.getRotationTo(v2).getYaw().valueDegrees();
				//left
				if (angle < 0)
				{
					Quaternion q;
					q.FromAngleAxis(Radian(Math::fDeg2Rad * 90), Vector3::UNIT_Y);
					ret = q * r.getDirection();
				}
				//right
				else if (angle > 0)
				{
					Quaternion q;
					q.FromAngleAxis(Radian(Math::fDeg2Rad * -90), Vector3::UNIT_Y);
					ret = q * r.getDirection();
				}
				else
				{
					int rnd = rand() % 2;
					if (rnd)
					{
						Quaternion q;
						q.FromAngleAxis(Radian(Math::fDeg2Rad * 90), Vector3::UNIT_Y);
						ret = q * r.getDirection();
					}
					else
					{
						Quaternion q;
						q.FromAngleAxis(Radian(Math::fDeg2Rad * -90), Vector3::UNIT_Y);
						ret = q * r.getDirection();
					}
				}
				break;
			}
		}
	}

	if (Position == PLAYER->GetPosition())
	{
		GlobalVars::test1 = ret.length();
	}

	return ret.normalisedCopy() * MaxAcceleration;
}

Vector3 Moving::CalculatePathFollow()
{
	if (mPath != NULL)
	{
		Ogre::Vector3 dest = mPath->GetNode(PathPosition).getPos();
		float pathNextDist = 2*Radius;

		if (Position.distance(dest) < pathNextDist && PathPosition < mPath->GetLength()-1)
		{
			PathPosition++;
			dest = mPath->GetNode(PathPosition).getPos();
		}
		
		SetDest(dest);

		if (PathPosition < mPath->GetLength()-1)
		{
			return CalculateSeek();
		}
		else 
		{
			pathOf();
			arriveOn();
			return CalculateArrive();
		}
	}
	return Vector3::ZERO;
}

Vector3 Moving::CalculateSeperate()
{
	Vector3 ret = Vector3::ZERO;

	for (int i=0;i<WORLD->getAgentTotal();i++)
	{
		Ogre::Vector3 delta = Position - AGENT(i)->GetPosition();
		float dist = delta.length();
		float mindist = Radius + AGENT(i)->GetRadius();
		if (dist < mindist)
		{
			float power = mindist - dist;
			if (power<0)power = 0;
			ret += delta.normalisedCopy() * power;
		}
	}
	return ret.normalisedCopy() * MaxAcceleration;
}
