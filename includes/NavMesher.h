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

#ifndef NavMesher_H_
#define NavMesher_H_

#include "Path.h"

#include "Recast/Recast.h"
//#include "Recast/RecastLog.h"
//#include "Recast/RecastTimer.h"
//#include "Detour/DetourStatNavMesh.h"
//#include "Detour/DetourStatNavMeshBuilder.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Detour/DetourNavMeshBuilder.h"

#include "InputGeom.h"

#include "OgreSingleton.h"

#define NAV NavMesher::getSingletonPtr()

class NavMesher : public Ogre::Singleton<NavMesher>
{
public:
	NavMesher();
	~NavMesher();

	void resetCommonSettings();
	bool Build();
	void Clear();
	void cleanup();
	void DrawDebug();
	void GetPath(Ogre::Vector3 from, Ogre::Vector3 to, Path* &path);
	bool CanBeWalkedTo(Ogre::Vector3 to, Ogre::Vector3 extents) const;
	void ShowDebug(bool b) const
	{ mDebuggerNode->setVisible(b); }
	int GetTotalPos() const
	{ return (int)allPositions.size(); }
	Ogre::Vector3 GetPos(int i) { return allPositions[i]; }
	const char * getFilePath() const
	{ return filepath; }

	enum DrawMode
	{
		DRAWMODE_NAVMESH,
		DRAWMODE_NAVMESH_TRANS,
		DRAWMODE_NAVMESH_BVTREE,
		DRAWMODE_NAVMESH_NODES,
		DRAWMODE_NAVMESH_INVIS,
		DRAWMODE_MESH,
		DRAWMODE_VOXELS,
		DRAWMODE_VOXELS_WALKABLE,
		DRAWMODE_COMPACT,
		DRAWMODE_COMPACT_DISTANCE,
		DRAWMODE_COMPACT_REGIONS,
		DRAWMODE_REGION_CONNECTIONS,
		DRAWMODE_RAW_CONTOURS,
		DRAWMODE_BOTH_CONTOURS,
		DRAWMODE_CONTOURS,
		DRAWMODE_POLYMESH,
		DRAWMODE_POLYMESH_DETAIL,
		MAX_DRAWMODE
	};

	/// These are just sample areas to use consistent values across the samples.
	/// The use should specify these base on his needs.
	enum SamplePolyAreas
	{
		SAMPLE_POLYAREA_GROUND,
		SAMPLE_POLYAREA_WATER,
		SAMPLE_POLYAREA_ROAD,
		SAMPLE_POLYAREA_DOOR,
		SAMPLE_POLYAREA_GRASS,
		SAMPLE_POLYAREA_JUMP,
	};
	
	enum SamplePolyFlags
	{
		SAMPLE_POLYFLAGS_WALK		= 0x01,		// Ability to walk (ground, grass, road)
		SAMPLE_POLYFLAGS_SWIM		= 0x02,		// Ability to swim (water).
		SAMPLE_POLYFLAGS_DOOR		= 0x04,		// Ability to move through doors.
		SAMPLE_POLYFLAGS_JUMP		= 0x08,		// Ability to jump.
		SAMPLE_POLYFLAGS_DISABLED	= 0x10,		// Disabled polygon
		SAMPLE_POLYFLAGS_ALL		= 0xffff	// All abilities.
	};

	DrawMode m_drawMode;

protected:
	InputGeom* m_geom;
	rcContext *m_ctx;

	bool m_keepInterResults;
	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcPolyMesh* m_pmesh;
	rcPolyMeshDetail* m_dmesh;
	dtNavMesh* m_navMesh;
	dtNavMeshQuery* m_navQuery;	
	// Poly search filter
	dtQueryFilter filter;
	dtNavMeshCreateParams m_params;
	rcConfig m_cfg;
	rcContourSet* m_cset;

	int numPaths;

	

	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	bool m_monotonePartitioning;
	float m_edgeMaxLen;
	float m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailSampleDist;
	float m_detailSampleMaxError;
private:
	std::vector<Ogre::Vector3> allPositions;

	const char * filepath;

	//debugging
	Ogre::ManualObject*	mNavMeshDebugger;
	Ogre::SceneNode*	mDebuggerNode;
};

#endif