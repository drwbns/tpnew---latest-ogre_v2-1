//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef RECASTSAMPLETILEMESH_H
#define RECASTSAMPLETILEMESH_H

#include "Sample.h"

#include "OgreSingleton.h"
#include "DebugDrawOgre.h"
#include "boost\thread\mutex.hpp"

#define SMPL Sample_TileMesh::getSingletonPtr()

class Sample_TileMesh : public Singleton<Sample_TileMesh>, public Sample
{
protected:
	bool m_keepInterResults;
	bool m_buildAll;
	float m_totalBuildTimeMs;
	int m_numTiles;
	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcPolyMeshDetail* m_dmesh;
	rcConfig m_cfg;	
	
	enum DrawMode
	{
		DRAWMODE_NAVMESH,
		DRAWMODE_NAVMESH_TRANS,
		DRAWMODE_NAVMESH_BVTREE,
		DRAWMODE_NAVMESH_NODES,
		DRAWMODE_NAVMESH_PORTALS,
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
		
	DrawMode m_drawMode;
	int mTileWidth, mTileHeight;
	int m_maxTiles;
	int m_maxPolysPerTile;
	float m_tileSize;
	
	unsigned int m_tileCol;
	float m_tileBmin[3];
	float m_tileBmax[3];
	float m_tileBuildTime;
	float m_tileMemUsage;
	int m_tileTriCount;
	
	int tileTriCount;

	//debugging
	ManualObject*	mNavMeshDebugger;
	SceneNode*	mDebuggerNode;
	
	BuildContext * ctx;
	
	void cleanup();
	void CleanupAfterBuild() const;
	void CleanupAfterTileBuild();
	static void saveAll(const char* path, const dtNavMesh* mesh);
	static dtNavMesh* loadAll(const char* path);

	bool mResizedMap;
	std::vector<Vector3> allPositions;
	// TileGrid multi-d array [][]
	std::vector< std::vector<int> > mymap;
	std::string filepath;
	boost::mutex myMutex;
public:
	Sample_TileMesh();
	virtual ~Sample_TileMesh();
	
	static Sample_TileMesh& getSingleton();
	static Sample_TileMesh* getSingletonPtr();
	void AddToNumTiles () {
		boost::mutex::scoped_lock lock(myMutex);
		m_numTiles++;
	}
	bool getResizedBool() const
	{ return mResizedMap; }
	void setResizedBool(bool myBool) { mResizedMap = myBool; }

	BuildContext * getBC() const
	{ return ctx; }
	rcConfig getRcConfig() const
	{ return m_cfg; }
	int getTileWidth() const
	{ return mTileWidth; }
	int getTileHeight() const
	{ return mTileHeight; }
	std::vector< std::vector<int> >& getMap() { return mymap; }
	virtual void handleSettings();
	virtual void handleRender();
	virtual bool handleBuild() override;
	void DrawDebug();
	
	void getTilePos(const float* pos, int& tx, int& ty) const;
	
	void buildTile(const float* pos);
	void removeTile(const float* pos);
	void buildAllTiles();
	void removeAllTiles() const;

	static void QuadrantTilerFunc();

	int GetTotalPos() const
	{ return static_cast<int>(allPositions.size()); }
	Vector3 GetPos(int i) { return allPositions[i]; }
	const char * getFilePath() const
	{ return filepath.c_str(); }
	void GetPath(Vector3 from, Vector3 to, Path* &path) const;
	bool CanBeWalkedTo(Vector3 to, Vector3 extents) const;

	dtNavMesh* buildMesh(int numCores, BuildContext* ctx = SMPL->getBC());
	unsigned char* buildTileMesh2(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize,BuildContext* ctx = SMPL->getBC());
	unsigned char* buildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize, BuildContext* ctx = SMPL->getBC());
};


#endif // RECASTSAMPLETILEMESH_H
