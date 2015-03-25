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

#ifndef AIUtility_H_
#define AIUtility_H_

class AIUtility
{
public:
	static Ogre::Vector3 LineIntersect(Ogre::Vector3 start1,Ogre::Vector3 end1,Ogre::Vector3 start2,Ogre::Vector3 end2)
	{
		Ogre::Vector3 intersection;
		Ogre::Vector3 begin = start1;
		Ogre::Vector3 end = end1;
		Ogre::Vector3 other_begin = start2;
		Ogre::Vector3 other_end = end2;

		float denom = ((other_end.z - other_begin.z)*(end.x - begin.x)) -
					  ((other_end.x - other_begin.x)*(end.z - begin.z));

		float nume_a = ((other_end.x - other_begin.x)*(begin.z - other_begin.z)) -
					   ((other_end.z - other_begin.z)*(begin.x - other_begin.x));

		float nume_b = ((end.x - begin.x)*(begin.z - other_begin.z)) -
					   ((end.z - begin.z)*(begin.x - other_begin.x));

		if(denom == 0.0f)
		{
			if(nume_a == 0.0f && nume_b == 0.0f)
			{
				return Ogre::Vector3(-1,-1,-1);
			}
			return Ogre::Vector3(-1,-1,-1);
		}

		float ua = nume_a / denom;
		float ub = nume_b / denom;

		if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
		{
			//get the intersection point.
			intersection.x = begin.x + ua*(end.x - begin.x);
			intersection.z = begin.z + ua*(end.z - begin.z);
			intersection.y = 0;
			return intersection;
		}

		return Ogre::Vector3(-1,-1,-1);
	}

	
	static bool IntersectLineCircle(Ogre::Vector3 vOrginCircle, float r, Ogre::Vector3 vtLineStart, Ogre::Vector3 vtLineEnd, Ogre::Vector3& outIp)
	{
		Ogre::Vector3 vOrginNew = vOrginCircle - vtLineStart;
		Ogre::Vector3 vtRay = (vtLineEnd - vtLineStart).normalisedCopy();
		float b = vOrginNew.x* vtRay.x + vtRay.y * vOrginNew.y;
		float c = r*r - ( vOrginNew.x*vOrginNew.x + vOrginNew.y*vOrginNew.y );
		float a = vtRay.x*vtRay.x + vtRay.y*vtRay.y; // Norm2() is x*x + y*y

		float diva = 1.0F/a;
		// Negative doesn't have square root.
		if( (b*b + a * c ) <= 0 ) return false;
		float b4ac = sqrt( (b*b + a * c ) );
		float l1 = (b - b4ac) * diva;
		float l2 = (b + b4ac) * diva;

		// we need the closest intersection point.. so find smaller l.
		// To get the other end , just change the comparison operator.
		float l = l2;
		if (l2 < l1)
		{
			l = l1;
		}

		// find the intersecting point
		outIp = (vtRay * l) + vtLineStart;

		return true;
	}

	static bool PointInTriangle3D(Ogre::Vector3 P, Ogre::Vector3 A, Ogre::Vector3 B, Ogre::Vector3 C)
	{
		// Compute vectors        
		Ogre::Vector3 v0 = C - A;
		Ogre::Vector3 v1 = B - A;
		Ogre::Vector3 v2 = P - A;

		// Compute dot products
		float dot00 = v0.dotProduct(v0);
		float dot01 = v0.dotProduct(v1);
		float dot02 = v0.dotProduct(v2);
		float dot11 = v1.dotProduct(v1);
		float dot12 = v1.dotProduct(v2);

		// Compute barycentric coordinates
		float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is in triangle
		return (u > 0) && (v > 0) && (u + v < 1);
	}

	static bool PointInTriangle2D(Ogre::Vector3 P, Ogre::Vector3 A, Ogre::Vector3 B, Ogre::Vector3 C)
	{
		//no Y-coordinate
		P.y = 0;
		A.y = 0;
		B.y = 0;
		C.y = 0;

		// Compute vectors        
		Ogre::Vector3 v0 = C - A;
		Ogre::Vector3 v1 = B - A;
		Ogre::Vector3 v2 = P - A;

		// Compute dot products
		float dot00 = v0.dotProduct(v0);
		float dot01 = v0.dotProduct(v1);
		float dot02 = v0.dotProduct(v2);
		float dot11 = v1.dotProduct(v1);
		float dot12 = v1.dotProduct(v2);

		// Compute barycentric coordinates
		float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is in triangle
		return (u > 0) && (v > 0) && (u + v < 1);
	}
};

#endif