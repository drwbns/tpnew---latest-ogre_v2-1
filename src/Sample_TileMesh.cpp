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
#include "Sample_TileMesh.h"

#include "SampleInterfaces.h"
#include "Path.h"
#include "DebugDrawOgre.h"
#include "GraphicsSystem.h"
#include "InputGeom.h"
#include "Sample.h"

#include "OgreMaterialManager.h"
#include "OgreTechnique.h"

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <string.h>
//#include "SDL.h"
//#include "SDL_opengl.h"
//#include "imgui.h"

#include "Recast/Recast.h"

#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Detour/DetourNavMeshBuilder.h"

//#include "DetourDebugDraw.h"
//#include "NavMeshTesterTool.h"
//#include "OffMeshConnectionTool.h"
//#include "ConvexVolumeTool.h"
//#include "CrowdTool.h"

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>
#include <SharedData.h>
#include <Recast/RecastDebugDraw.h>

#ifdef WIN32
#	define snprintf _snprintf
#endif

void Sample_TileMesh::CleanupAfterBuild() const
{
	if (m_navMesh != nullptr) { dtFreeNavMesh(m_navMesh); }
}

void Sample_TileMesh::CleanupAfterTileBuild() {
	delete[] m_triareas;
	m_triareas = nullptr;
	rcFreeHeightField(m_solid);
	m_solid = nullptr;
	rcFreeCompactHeightfield(m_chf);
	m_chf = nullptr;
	rcFreeContourSet(m_cset);
	m_cset = nullptr;
	rcFreePolyMesh(m_pmesh);
	m_pmesh = nullptr;
	rcFreePolyMeshDetail(m_dmesh);
	m_dmesh = nullptr;
}

void Sample_TileMesh::cleanup()
{
	delete[] m_triareas;
	m_triareas = nullptr;
	rcFreeHeightField(m_solid);
	m_solid = nullptr;
	rcFreeCompactHeightfield(m_chf);
	m_chf = nullptr;
	rcFreeContourSet(m_cset);
	m_cset = nullptr;
	rcFreePolyMesh(m_pmesh);
	m_pmesh = nullptr;
	rcFreePolyMeshDetail(m_dmesh);
	m_dmesh = nullptr;
}

template <typename TData>
class MonitorQueue : boost::noncopyable
{
public:
	void Push(const TData& data)
	{
		boost::mutex::scoped_lock lock(monitorMutex);
		//std::cout << "Pushing data to stack: " << &data << "\n";
		queue.push(data);
		itemAvailable.notify_one();
	}
	bool IsEmpty() { return queue.empty(); }
	TData PopWait()
	{
		boost::mutex::scoped_lock lock(monitorMutex);
		if (queue.empty())
		{
			itemAvailable.wait(lock);
		}

		TData temp = queue.front();
		queue.pop();
		return temp;
	}

private:
	std::queue<TData> queue;

	boost::mutex monitorMutex;
	boost::condition_variable itemAvailable;
};

class TileDispatcher : boost::noncopyable
{
public:
	int maxWidth, maxHeight;

	void Reset()
	{
		nextWidth = 0;
		maxWidth = 0;
		nextHeight = 0;
		maxHeight = 0;
	}

	void GetNextNeededTile(int &x, int &y)
	{
		boost::mutex::scoped_lock lock(dispatchMutex);
		if (nextWidth >= maxWidth)
		{
			nextWidth = 0;
			nextHeight++;
		}

		if (nextHeight >= maxHeight)
		{
			x = -1; y = -1;
			return;
		}

		//std::cout << "NextTile: " << nextWidth << ", " << nextHeight << "\n";

		x = nextWidth++;
		y = nextHeight;
	}

	TileDispatcher()
	{
		Reset();
	}

private:
	boost::mutex dispatchMutex;
	int nextWidth, nextHeight;
};

#define INVALID_TILE -47
struct TileData
{
	int X, Y, DataSize;
	unsigned char* Data;
};

MonitorQueue<TileData> tileQueue;
TileDispatcher dispatcher;

std::vector< std::vector<TileData> > mymap2;
void addTile(int x, int y, TileData data) { mymap2[x][y] = data; }

class MyThread
{
protected:
	std::vector<TileData> mTiles;
public:
	MyThread() {}

	static void resize() {
		if (SMPL->getResizedBool() == false) {
			SMPL->setResizedBool(true);
			// Set up sizes. (HEIGHT x WIDTH)
			mymap2.resize(SMPL->getTileWidth());
			for (int i = 0; i < SMPL->getTileWidth(); i++)
				mymap2[i].resize(SMPL->getTileHeight());
		}
	}
	void operator()(void) const
	{
		const float* bmin = INPT->getMeshBoundsMin();
		const float* bmax = INPT->getMeshBoundsMax();
		const float tcs = SMPL->getRcConfig().tileSize*SMPL->getRcConfig().cs;

		int x = 0;
		int y = 0;
		while (x != -1)
		{
			dispatcher.GetNextNeededTile(x, y);
			float tileBmin[3], tileBmax[3];
			tileBmin[0] = bmin[0] + x*tcs;
			tileBmin[1] = bmin[1];
			tileBmin[2] = bmin[2] + y*tcs;

			tileBmax[0] = bmin[0] + (x + 1)*tcs;
			tileBmax[1] = bmax[1];
			tileBmax[2] = bmin[2] + (y + 1)*tcs;

			int dataSize = 0;
			if (x != -1) {
				SMPL->AddToNumTiles();
				//if(SMPL->getMap()[x][y]==0) {
				//SMPL->getMap()[x][y] = 1;
				unsigned char* data = SMPL->buildTileMesh2(x, y, tileBmin, tileBmax, dataSize, new BuildContext);
				if (data)
				{
					TileData newData;
					newData.X = x;
					newData.Y = y;
					newData.DataSize = dataSize;
					newData.Data = data;

					SMPL->getNavMesh()->removeTile(SMPL->getNavMesh()->getTileRefAt(newData.X, newData.Y, 0), nullptr, nullptr);
					addTile(x, y, newData);
					// Let the navmesh own the data
					/*
					dtStatus status = SMPL->getNavMesh()->addTile(newData.Data, newData.DataSize, DT_TILE_FREE_DATA, 0, 0);
					if (dtStatusFailed(status))
						dtFree(newData.Data);
					*/
					//std::cout << "\tNew TileData: {X = " << newData.X << ", Y = " << newData.Y << ", DataSize = " << newData.DataSize << "}\n";

					SMPL->getBC()->log(RC_LOG_PROGRESS, "New TileData: {X = %d, Y = %d, DataSize = %d}", newData.X, newData.Y, newData.DataSize);
					//tileQueue.Push(newData);
				}
				//}
				SMPL->getBC()->dumpLogToFile("TileData.txt");
			}
		}
		SMPL->getBC()->dumpLogToFile("navMesh.txt");
		/*
		TileData doneData;
		doneData.X = INVALID_TILE;
		doneData.Y = INVALID_TILE;
		doneData.DataSize = 0;
		doneData.Data = (unsigned char*)0;

		tileQueue.Push(doneData);
		*/
	}
};

struct QuadrantTiler
{
	BuildContext *ctx;
	int *numTiles;

	void operator()() const
	{
		const float* bmin = INPT->getMeshBoundsMin();
		const float* bmax = INPT->getMeshBoundsMax();
		const float tcs = SMPL->getRcConfig().tileSize*SMPL->getRcConfig().cs;

		int x = 0;
		int y = 0;
		while (x != -1)
		{
			dispatcher.GetNextNeededTile(x, y);
			float tileBmin[3], tileBmax[3];
			tileBmin[0] = bmin[0] + x*tcs;
			tileBmin[1] = bmin[1];
			tileBmin[2] = bmin[2] + y*tcs;

			tileBmax[0] = bmin[0] + (x + 1)*tcs;
			tileBmax[1] = bmax[1];
			tileBmax[2] = bmin[2] + (y + 1)*tcs;

			int dataSize = 0;
			if (x != -1) {
				SMPL->AddToNumTiles();
				//if(SMPL->getMap()[x][y]==0) {
					//SMPL->getMap()[x][y] = 1;
				unsigned char* data = SMPL->buildTileMesh2(x, y, tileBmin, tileBmax, dataSize, ctx);
				if (data)
				{
					TileData newData;
					newData.X = x;
					newData.Y = y;
					newData.DataSize = dataSize;
					newData.Data = data;

					SMPL->getNavMesh()->removeTile(SMPL->getNavMesh()->getTileRefAt(newData.X, newData.Y, 0), nullptr, nullptr);

					// Let the navmesh own the data
					dtStatus status = SMPL->getNavMesh()->addTile(newData.Data, newData.DataSize, DT_TILE_FREE_DATA, 0, nullptr);
					if (dtStatusFailed(status))
						dtFree(newData.Data);

					//std::cout << "\tNew TileData: {X = " << newData.X << ", Y = " << newData.Y << ", DataSize = " << newData.DataSize << "}\n";

					ctx->log(RC_LOG_PROGRESS, "New TileData: {X = %d, Y = %d, DataSize = %d}", newData.X, newData.Y, newData.DataSize);
					//tileQueue.Push(newData);
				}
				//}
				ctx->dumpLogToFile("TileData.txt");
			}
		}
		ctx->dumpLogToFile("navMesh.txt");
		/*
			TileData doneData;
			doneData.X = INVALID_TILE;
			doneData.Y = INVALID_TILE;
			doneData.DataSize = 0;
			doneData.Data = (unsigned char*)0;

			tileQueue.Push(doneData);
			*/
	}
};

struct TileAdder
{
	dtNavMesh *mesh;
	int numThreads;
	BuildContext *ctx;

	void operator()() const
	{
		int numThreadsCompleted = 0;
		while (!tileQueue.IsEmpty()) {
			//std::cout << "**Waiting for tile data...**\n";
			//std::cout << "**TileData found: ";
			TileData data = tileQueue.PopWait();
			if (data.X == INVALID_TILE && data.Y == INVALID_TILE)
			{
				//std::cout << "Invalid Data, one of the threads has completed**\n";
				ctx->log(RC_LOG_PROGRESS, "Invalid Data, one of the threads has completed** data.x = %d, data.y = %d", data.X, data.Y);
				numThreadsCompleted++;
				continue;
			}

			//std::cout << "Valid data, adding to mesh**\n";
			ctx->log(RC_LOG_PROGRESS, "Valid data, adding to mesh** data.x = %d, data.y = %d", data.X, data.Y);
			// Remove any previous data (navmesh owns and deletes the data).
			mesh->removeTile(mesh->getTileRefAt(data.X, data.Y, 0), nullptr, nullptr);

			// Let the navmesh own the data
			dtStatus status = mesh->addTile(data.Data, data.DataSize, DT_TILE_FREE_DATA, 0, nullptr);
			if (dtStatusFailed(status))
				dtFree(data.Data);
		}
		std::cout << "All quadrants finished.\n";
	}
};

inline unsigned int nextPow2(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

inline unsigned int ilog2(unsigned int v)
{
	unsigned int r;
	unsigned int shift;
	r = (v > 0xffff) << 4; v >>= r;
	shift = (v > 0xff) << 3; v >>= shift; r |= shift;
	shift = (v > 0xf) << 2; v >>= shift; r |= shift;
	shift = (v > 0x3) << 1; v >>= shift; r |= shift;
	r |= (v >> 1);
	return r;
}

class NavMeshTileTool : public SampleTool
{
	Sample_TileMesh* m_sample;
	float m_hitPos[3];
	bool m_hitPosSet;
	float m_agentRadius;

public:

	NavMeshTileTool() :
		m_sample(nullptr),
		m_hitPosSet(false),
		m_agentRadius(0)
	{
		m_hitPos[0] = m_hitPos[1] = m_hitPos[2] = 0;
	}

	virtual ~NavMeshTileTool()
	{
	}

	virtual int type() override
	{
		return TOOL_TILE_EDIT;
	}

	virtual void init(Sample* sample) override
	{
		m_sample = static_cast<Sample_TileMesh*>(sample);
	}

	virtual void reset() override
	{}
};

template<> Sample_TileMesh* Ogre::Singleton<Sample_TileMesh>::msSingleton = nullptr;

Sample_TileMesh* Sample_TileMesh::getSingletonPtr(void)
{
	return msSingleton;
}

Sample_TileMesh& Sample_TileMesh::getSingleton(void)
{
	assert(msSingleton);  return (*msSingleton);
}

Sample_TileMesh::Sample_TileMesh() :
	m_keepInterResults(false),
	m_buildAll(true),
	m_totalBuildTimeMs(0),
	m_numTiles(0),
	m_triareas(nullptr),
	m_solid(nullptr),
	m_chf(nullptr),
	m_cset(nullptr),
	m_pmesh(nullptr),
	m_dmesh(nullptr),
	m_drawMode(DRAWMODE_NAVMESH),
	m_maxTiles(0),
	m_maxPolysPerTile(0),
	m_tileSize(128),
	m_tileCol(duRGBA(0, 0, 0, 32)),
	m_tileBuildTime(0),
	m_tileMemUsage(0),
	m_tileTriCount(0),
	mResizedMap(false),
	filepath("city.mesh")
{
	resetCommonSettings();
	memset(m_tileBmin, 0, sizeof(m_tileBmin));
	memset(m_tileBmax, 0, sizeof(m_tileBmax));

	//debuggers
	mDebuggerNode = SharedData::getSingleton().iSceneMgr->getRootSceneNode()->createChildSceneNode();
	AxisAlignedBox aabInf;
	aabInf.setInfinite();

	mNavMeshDebugger = SharedData::getSingleton().iSceneMgr->createManualObject("mNavMesherDebugger");
	mNavMeshDebugger->setVisible(true);
	mNavMeshDebugger->setCastShadows(false);
	mDebuggerNode->attachObject(mNavMeshDebugger);
	mDebuggerNode->setVisible(true);
	MaterialPtr mNavMeshDebuggerMaterial = MaterialManager::getSingleton().create("mDebuggerMaterial", "General");
	mNavMeshDebuggerMaterial->setReceiveShadows(false);
	mNavMeshDebuggerMaterial->getTechnique(0)->setLightingEnabled(false);
	mNavMeshDebugger->setBoundingBox(aabInf);

	setTool(new NavMeshTileTool);

	ctx = new BuildContext();
	ctx->enableLog(true);
}

Sample_TileMesh::~Sample_TileMesh()
{
	cleanup();
	mDebuggerNode->detachAllObjects();
	mDebuggerNode->getParentSceneNode()->removeAndDestroyChild(mDebuggerNode->getName());
	mDebuggerNode = nullptr;

	mNavMeshDebugger->clear();
	GSYS->GetSceneMgr()->destroyManualObject(mNavMeshDebugger);
	mNavMeshDebugger = nullptr;

	dtFreeNavMesh(m_navMesh);
	m_navMesh = nullptr;
	delete ctx;
}

bool Sample_TileMesh::CanBeWalkedTo(Ogre::Vector3 to, Ogre::Vector3 extents) const
{
	int MAX_POLYS = 256;
	dtPolyRef m_polys[256];
	int numPolys;

	float center[3];
	center[0] = to.x;
	center[1] = to.y;
	center[2] = to.z;

	float extent[3];
	extent[0] = extents.x;
	extent[1] = extents.y;
	extent[2] = extents.z;

	dtQueryFilter filter;

	m_navQuery->queryPolygons(center, extent, &filter, m_polys, &numPolys, MAX_POLYS);
	return numPolys != 0;
}

void Sample_TileMesh::GetPath(Ogre::Vector3 from, Ogre::Vector3 to, Path* &path) const
{
	//find a path
	int MAX_POLYS = 256;
	dtPolyRef m_polys[256];
	float m_straightPath[256 * 3];

	//Extents of Poly search
	float m_polyPickExt[3];
	m_polyPickExt[0] = 2;
	m_polyPickExt[1] = 4;
	m_polyPickExt[2] = 2;

	// Start mPosition
	float m_spos[3];
	m_spos[0] = from.x;
	m_spos[1] = from.y;
	m_spos[2] = from.z;

	// End position
	float m_epos[3];
	m_epos[0] = to.x;
	m_epos[1] = to.y;
	m_epos[2] = to.z;
	dtPolyRef m_startRef, m_endRef;

	int numPaths;

	dtQueryFilter filter;
	//filter.setIncludeFlags(1);
	m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &filter, &m_startRef, nullptr);
	m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &filter, &m_endRef, nullptr);

	if (m_startRef != 0 && m_endRef != 0) {
		m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &filter, m_polys, &numPaths, MAX_POLYS);
		if (numPaths > 0)
		{
			m_navQuery->findStraightPath(m_spos, m_epos, m_polys, numPaths, m_straightPath, nullptr, nullptr, &path->numStraightPaths, MAX_POLYS);
			path->Clear();
			for (int i = 0; i < path->numStraightPaths; ++i)
			{
				Vector3 cpos(m_straightPath[i * 3], m_straightPath[i * 3 + 1] + 0.25, m_straightPath[i * 3 + 2]);
				PtfNode node(&cpos);
				path->AddNode(node);
			}
		}
	}
}

static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

void Sample_TileMesh::saveAll(const char* path, const dtNavMesh* mesh)
{
	if (!mesh) return;

	FILE* fp = fopen(path, "wb");
	if (!fp)
		return;

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}
	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(NavMeshSetHeader), 1, fp);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

		fwrite(tile->data, tile->dataSize, 1, fp);
	}

	fclose(fp);
}

dtNavMesh* Sample_TileMesh::loadAll(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (!fp) return nullptr;

	// Read header.
	NavMeshSetHeader header;
	fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return nullptr;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return nullptr;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		return nullptr;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return nullptr;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = static_cast<unsigned char*>(dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM));
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		fread(data, tileHeader.dataSize, 1, fp);

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, nullptr);
	}

	fclose(fp);

	return mesh;
}

void Sample_TileMesh::handleSettings()
{
	if (INPT)
	{
		const float* bmin = INPT->getMeshBoundsMin();
		const float* bmax = INPT->getMeshBoundsMax();
		char text[64];
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
		const int ts = static_cast<int>(m_tileSize);
		const int tw = (gw + ts - 1) / ts;
		const int th = (gh + ts - 1) / ts;
		snprintf(text, 64, "Tiles  %d x %d", tw, th);

		mTileWidth = tw;
		mTileHeight = th;

		// Set up sizes. (HEIGHT x WIDTH)
		SMPL->getMap().resize(SMPL->getTileHeight());
		for (int i = 0; i < SMPL->getTileHeight(); ++i)
			mymap[i].resize(SMPL->getTileWidth());
		//imguiValue(text);

		// Max tiles and max polys affect how the tile IDs are caculated.
		// There are 22 bits available for identifying a tile and a polygon.
		int tileBits = rcMin(static_cast<int>(ilog2(nextPow2(tw*th))), 14);
		if (tileBits > 14) tileBits = 14;
		int polyBits = 22 - tileBits;
		m_maxTiles = 1 << tileBits;
		m_maxPolysPerTile = 1 << polyBits;
		snprintf(text, 64, "Max Tiles  %d", m_maxTiles);
		//imguiValue(text);
		snprintf(text, 64, "Max Polys  %d", m_maxPolysPerTile);
		//imguiValue(text);
	}
	else
	{
		m_maxTiles = 0;
		m_maxPolysPerTile = 0;
	}
}

void Sample_TileMesh::handleRender()
{
	if (!INPT || !INPT->getMesh())
		return;

	const float texScale = 1.0f / (m_cellSize * 10.0f);

	// Draw mesh
	if (m_drawMode != DRAWMODE_NAVMESH_TRANS)
	{
		// Draw mesh
		duDebugDrawTriMeshSlope(mNavMeshDebugger, &INPT->getMesh()->getVerts()[0], INPT->getMesh()->getVertCount(),
			&INPT->getMesh()->getTris()[0], INPT->getMesh()->getNormals(), INPT->getMesh()->getTriCount(),
			m_agentMaxSlope, texScale);
		drawOffMeshConnections(mNavMeshDebugger, true);
	}

	//	glDepthMask(GL_FALSE);

		// Draw bounds
	const float* bmin = INPT->getMeshBoundsMin();
	const float* bmax = INPT->getMeshBoundsMax();
	duDebugDrawBoxWire(mNavMeshDebugger, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], duRGBA(255, 255, 255, 128), 1.0f);

	// Tiling grid.
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int tw = (gw + static_cast<int>(m_tileSize) - 1) / static_cast<int>(m_tileSize);
	const int th = (gh + static_cast<int>(m_tileSize) - 1) / static_cast<int>(m_tileSize);
	const float s = m_tileSize*m_cellSize;
	duDebugDrawGridXZ(mNavMeshDebugger, bmin[0], bmin[1], bmin[2], tw, th, s, duRGBA(0, 0, 0, 64), 1.0f);

	// Draw active tile
	duDebugDrawBoxWire(mNavMeshDebugger, m_tileBmin[0], m_tileBmin[1], m_tileBmin[2],
		m_tileBmax[0], m_tileBmax[1], m_tileBmax[2], m_tileCol, 1.0f);

	if (m_navMesh && m_navQuery &&
		(m_drawMode == DRAWMODE_NAVMESH ||
			m_drawMode == DRAWMODE_NAVMESH_TRANS ||
			m_drawMode == DRAWMODE_NAVMESH_BVTREE ||
			m_drawMode == DRAWMODE_NAVMESH_NODES ||
			m_drawMode == DRAWMODE_NAVMESH_PORTALS ||
			m_drawMode == DRAWMODE_NAVMESH_INVIS))
	{
		if (m_drawMode != DRAWMODE_NAVMESH_INVIS)
			duDebugDrawNavMeshWithClosedList(mNavMeshDebugger, *m_navMesh, *m_navQuery, m_navMeshDrawFlags);
		if (m_drawMode == DRAWMODE_NAVMESH_BVTREE)
			duDebugDrawNavMeshBVTree(mNavMeshDebugger, *m_navMesh);
		if (m_drawMode == DRAWMODE_NAVMESH_PORTALS)
			duDebugDrawNavMeshPortals(mNavMeshDebugger, *m_navMesh);
		if (m_drawMode == DRAWMODE_NAVMESH_NODES)
			duDebugDrawNavMeshNodes(mNavMeshDebugger, *m_navQuery);
	}

	//glDepthMask(GL_TRUE);

	if (m_chf && m_drawMode == DRAWMODE_COMPACT)
		duDebugDrawCompactHeightfieldSolid(mNavMeshDebugger, *m_chf);

	if (m_chf && m_drawMode == DRAWMODE_COMPACT_DISTANCE)
		duDebugDrawCompactHeightfieldDistance(mNavMeshDebugger, *m_chf);
	if (m_chf && m_drawMode == DRAWMODE_COMPACT_REGIONS)
		duDebugDrawCompactHeightfieldRegions(mNavMeshDebugger, *m_chf);
	if (m_solid && m_drawMode == DRAWMODE_VOXELS)
	{
		//	glEnable(GL_FOG);
		duDebugDrawHeightfieldSolid(mNavMeshDebugger, *m_solid);
		//	glDisable(GL_FOG);
	}
	if (m_solid && m_drawMode == DRAWMODE_VOXELS_WALKABLE)
	{
		//	glEnable(GL_FOG);
		duDebugDrawHeightfieldWalkable(mNavMeshDebugger, *m_solid);
		//	glDisable(GL_FOG);
	}

	if (m_cset && m_drawMode == DRAWMODE_RAW_CONTOURS)
	{
		//	glDepthMask(GL_FALSE);
		duDebugDrawRawContours(mNavMeshDebugger, *m_cset);
		//	glDepthMask(GL_TRUE);
	}

	if (m_cset && m_drawMode == DRAWMODE_BOTH_CONTOURS)
	{
		//	glDepthMask(GL_FALSE);
		duDebugDrawRawContours(mNavMeshDebugger, *m_cset, 0.5f);
		duDebugDrawContours(mNavMeshDebugger, *m_cset);
		//	glDepthMask(GL_TRUE);
	}
	if (m_cset && m_drawMode == DRAWMODE_CONTOURS)
	{
		//	glDepthMask(GL_FALSE);
		duDebugDrawContours(mNavMeshDebugger, *m_cset);
		//	glDepthMask(GL_TRUE);
	}
	if (m_chf && m_cset && m_drawMode == DRAWMODE_REGION_CONNECTIONS)
	{
		duDebugDrawCompactHeightfieldRegions(mNavMeshDebugger, *m_chf);

		//	glDepthMask(GL_FALSE);
		duDebugDrawRegionConnections(mNavMeshDebugger, *m_cset);
		//	glDepthMask(GL_TRUE);
	}
	if (m_pmesh && m_drawMode == DRAWMODE_POLYMESH)
	{
		//	glDepthMask(GL_FALSE);
		duDebugDrawPolyMesh(mNavMeshDebugger, *m_pmesh);
		//	glDepthMask(GL_TRUE);
	}
	if (m_dmesh && m_drawMode == DRAWMODE_POLYMESH_DETAIL)
	{
		//	glDepthMask(GL_FALSE);
		duDebugDrawPolyMeshDetail(mNavMeshDebugger, *m_dmesh);
		//	glDepthMask(GL_TRUE);
	}

	drawConvexVolumes(mNavMeshDebugger, true);

	//	if (m_tool)
	//		m_tool->handleRender();

		//glDepthMask(GL_TRUE);
}

bool Sample_TileMesh::handleBuild()
{
	if (!INPT || !INPT->loadMesh(filepath.c_str()))
	{
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: No vertices and triangles.");
		return false;
	}
	// Free any old namesh that's still in memory
	dtFreeNavMesh(m_navMesh);

	// begin recast prep settings
	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		//CleanupAfterBuild();
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
		return false;
	}
	m_navQuery = dtAllocNavMeshQuery();
	if (!m_navQuery)
	{
		//CleanupAfterBuild();
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate Navmesh Query.");
		return false;
	}
	// Init build configuration from GUI
	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = m_cellSize;
	m_cfg.ch = m_cellHeight;
	m_cfg.walkableSlopeAngle = m_agentMaxSlope;
	m_cfg.walkableHeight = static_cast<int>(ceilf(m_agentHeight / m_cfg.ch));
	m_cfg.walkableClimb = static_cast<int>(floorf(m_agentMaxClimb / m_cfg.ch));
	m_cfg.walkableRadius = static_cast<int>(ceilf(m_agentRadius / m_cfg.cs));
	m_cfg.maxEdgeLen = static_cast<int>(m_edgeMaxLen / m_cellSize);
	m_cfg.maxSimplificationError = m_edgeMaxError;
	m_cfg.minRegionArea = static_cast<int>(rcSqr(m_regionMinSize));		// Note: area = size*size
	m_cfg.mergeRegionArea = static_cast<int>(rcSqr(m_regionMergeSize));	// Note: area = size*size
	m_cfg.maxVertsPerPoly = static_cast<int>(m_vertsPerPoly);
	m_cfg.tileSize = static_cast<int>(m_tileSize);
	m_cfg.borderSize = m_cfg.walkableRadius + 3; // Reserve enough padding.
	m_cfg.width = m_cfg.tileSize + m_cfg.borderSize * 2;
	m_cfg.height = m_cfg.tileSize + m_cfg.borderSize * 2;
	m_cfg.detailSampleDist = 0; // m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist; // Set to 0 for fast builds
	m_cfg.detailSampleMaxError = 0; // m_cellHeight * m_detailSampleMaxError; // Set to 0 for fast builds

	// default calculations - for some reason not included in basic recast
	const float* bmin = INPT->getMeshBoundsMin();
	const float* bmax = INPT->getMeshBoundsMax();

	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int ts = m_tileSize;
	const int tw = (gw + ts - 1) / ts;
	const int th = (gh + ts - 1) / ts;

	mTileWidth = tw;
	mTileHeight = th;

	// Max tiles and max polys affect how the tile IDs are caculated.
	// There are 22 bits available for identifying a tile and a polygon.
	int tileBits = rcMin(static_cast<int>(ilog2(nextPow2(tw*th))), 14);
	if (tileBits > 14) tileBits = 14;
	int polyBits = 22 - tileBits;
	int maxTiles = 1 << tileBits;
	int maxPolysPerTile = 1 << polyBits;

	dtNavMeshParams params;
	rcVcopy(params.orig, INPT->getMeshBoundsMin());
	params.tileWidth = m_tileSize * m_cellSize;
	params.tileHeight = m_tileSize * m_cellSize;
	params.maxTiles = maxTiles;
	params.maxPolys = maxPolysPerTile;

	dtStatus status;

	status = m_navMesh->init(&params);
	if (dtStatusFailed(status))
	{
		//CleanupAfterBuild();
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
		return false;
	}

	// Try to load existing navMesh from file
	m_navMesh = loadAll((filepath + ".bin").c_str());
	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
		return false;
	}
	if (!m_navMesh) {
		if (INPT)
		{
			const float* bmin1 = INPT->getMeshBoundsMin();
			const float* bmax1 = INPT->getMeshBoundsMax();
			char text[64];
			int gw1 = 0, gh1 = 0;
			rcCalcGridSize(bmin1, bmax1, m_cellSize, &gw1, &gh1);
			const int ts1 = static_cast<int>(m_tileSize);
			const int tw1 = (gw1 + ts1 - 1) / ts1;
			const int th1 = (gh1 + ts1 - 1) / ts1;
			snprintf(text, 64, "Tiles  %d x %d", tw1, th1);
			//imguiValue(text);

			//imguiValue(text);

			// Max tiles and max polys affect how the tile IDs are caculated.
			// There are 22 bits available for identifying a tile and a polygon.
			int tile_bits = rcMin(static_cast<int>(ilog2(nextPow2(tw1*th1))), 14);
			if (tile_bits > 14) tile_bits = 14;
			int poly_bits = 22 - tile_bits;
			m_maxTiles = 1 << tile_bits;
			m_maxPolysPerTile = 1 << poly_bits;
			snprintf(text, 64, "Max Tiles  %d", m_maxTiles);
			//imguiValue(text);
			snprintf(text, 64, "Max Polys  %d", m_maxPolysPerTile);
			//imguiValue(text);
		}
		else
		{
			m_maxTiles = 0;
			m_maxPolysPerTile = 0;
		}

		if (m_buildAll) {
			//buildAllTiles();
			m_navMesh = buildMesh(boost::thread::hardware_concurrency());
			saveAll((filepath + ".bin").c_str(), m_navMesh);
		}
	}
	if (m_tool)
		m_tool->init(this);

	ctx->dumpLogToFile("navMesh.txt");
	return true;
}

void Sample_TileMesh::buildTile(const float* pos)
{
	if (!INPT) return;
	if (!m_navMesh) return;

	const float* bmin = INPT->getMeshBoundsMin();
	const float* bmax = INPT->getMeshBoundsMax();

	const float ts = m_tileSize*m_cellSize;
	const int tx = static_cast<int>((pos[0] - bmin[0]) / ts);
	const int ty = static_cast<int>((pos[2] - bmin[2]) / ts);

	m_tileBmin[0] = bmin[0] + tx*ts;
	m_tileBmin[1] = bmin[1];
	m_tileBmin[2] = bmin[2] + ty*ts;

	m_tileBmax[0] = bmin[0] + (tx + 1)*ts;
	m_tileBmax[1] = bmax[1];
	m_tileBmax[2] = bmin[2] + (ty + 1)*ts;

	m_tileCol = duRGBA(255, 255, 255, 64);

	ctx->resetLog();

	int dataSize = 0;
	unsigned char* data = buildTileMesh(tx, ty, m_tileBmin, m_tileBmax, dataSize);

	if (data)
	{
		// Remove any previous data (navmesh owns and deletes the data).
		m_navMesh->removeTile(m_navMesh->getTileRefAt(tx, ty, 0), nullptr, nullptr);

		// Let the navmesh own the data.
		dtStatus status = m_navMesh->addTile(data, dataSize, DT_TILE_FREE_DATA, 0, nullptr);
		if (dtStatusFailed(status))
			dtFree(data);
	}

	ctx->dumpLog("Build Tile (%d,%d):", tx, ty);
}

void Sample_TileMesh::getTilePos(const float* pos, int& tx, int& ty) const
{
	if (!INPT) return;

	const float* bmin = INPT->getMeshBoundsMin();

	const float ts = m_tileSize*m_cellSize;
	tx = static_cast<int>((pos[0] - bmin[0]) / ts);
	ty = static_cast<int>((pos[2] - bmin[2]) / ts);
}

void Sample_TileMesh::removeTile(const float* pos)
{
	if (!INPT) return;
	if (!m_navMesh) return;

	const float* bmin = INPT->getMeshBoundsMin();
	const float* bmax = INPT->getMeshBoundsMax();

	const float ts = m_tileSize*m_cellSize;
	const int tx = static_cast<int>((pos[0] - bmin[0]) / ts);
	const int ty = static_cast<int>((pos[2] - bmin[2]) / ts);

	m_tileBmin[0] = bmin[0] + tx*ts;
	m_tileBmin[1] = bmin[1];
	m_tileBmin[2] = bmin[2] + ty*ts;

	m_tileBmax[0] = bmin[0] + (tx + 1)*ts;
	m_tileBmax[1] = bmax[1];
	m_tileBmax[2] = bmin[2] + (ty + 1)*ts;

	m_tileCol = duRGBA(128, 32, 16, 64);

	m_navMesh->removeTile(m_navMesh->getTileRefAt(tx, ty, 0), nullptr, nullptr);
}

dtNavMesh* Sample_TileMesh::buildMesh(int numCores, BuildContext* ctx)
{
	InputGeom * geom = INPT;

	if (!geom || !geom->getMesh())
	{
		//CleanupAfterBuild();
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: No vertices and triangles.");
		return nullptr;
	}

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		//CleanupAfterBuild();
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
		return nullptr;
	}
	m_navQuery = dtAllocNavMeshQuery();
	if (!m_navQuery)
	{
		//CleanupAfterBuild();
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate Navmesh Query.");
		return nullptr;
	}
	// Init build configuration from GUI
	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = m_cellSize;
	m_cfg.ch = m_cellHeight;
	m_cfg.walkableSlopeAngle = m_agentMaxSlope;
	m_cfg.walkableHeight = static_cast<int>(ceilf(m_agentHeight / m_cfg.ch));
	m_cfg.walkableClimb = static_cast<int>(floorf(m_agentMaxClimb / m_cfg.ch));
	m_cfg.walkableRadius = static_cast<int>(ceilf(m_agentRadius / m_cfg.cs));
	m_cfg.maxEdgeLen = static_cast<int>(m_edgeMaxLen / m_cellSize);
	m_cfg.maxSimplificationError = m_edgeMaxError;
	m_cfg.minRegionArea = static_cast<int>(rcSqr(m_regionMinSize));		// Note: area = size*size
	m_cfg.mergeRegionArea = static_cast<int>(rcSqr(m_regionMergeSize));	// Note: area = size*size
	m_cfg.maxVertsPerPoly = static_cast<int>(m_vertsPerPoly);
	m_cfg.tileSize = static_cast<int>(m_tileSize);
	m_cfg.borderSize = m_cfg.walkableRadius + 3; // Reserve enough padding.
	m_cfg.width = m_cfg.tileSize + m_cfg.borderSize * 2;
	m_cfg.height = m_cfg.tileSize + m_cfg.borderSize * 2;
	m_cfg.detailSampleDist = 0; // m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist; // Set to 0 for fast builds
	m_cfg.detailSampleMaxError = 0; // m_cellHeight * m_detailSampleMaxError; // Set to 0 for fast builds

	// default calculations - for some reason not included in basic recast
	const float* bmin = geom->getMeshBoundsMin();
	const float* bmax = geom->getMeshBoundsMax();

	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int ts = m_tileSize;
	const int tw = (gw + ts - 1) / ts;
	const int th = (gh + ts - 1) / ts;

	mTileWidth = tw;
	mTileHeight = th;

	// Set up sizes. (HEIGHT x WIDTH)
	mymap.resize(SMPL->getTileWidth());
	for (int i = 0; i < mTileWidth; i++)
		mymap[i].resize(mTileHeight);

	// Max tiles and max polys affect how the tile IDs are caculated.
	// There are 22 bits available for identifying a tile and a polygon.
	int tileBits = rcMin(static_cast<int>(ilog2(nextPow2(tw*th))), 14);
	if (tileBits > 14) tileBits = 14;
	int polyBits = 22 - tileBits;
	int maxTiles = 1 << tileBits;
	int maxPolysPerTile = 1 << polyBits;

	dtNavMeshParams params;
	rcVcopy(params.orig, geom->getMeshBoundsMin());
	params.tileWidth = m_tileSize * m_cellSize;
	params.tileHeight = m_tileSize * m_cellSize;
	params.maxTiles = maxTiles;
	params.maxPolys = maxPolysPerTile;

	dtStatus status;

	status = m_navMesh->init(&params);
	if (dtStatusFailed(status))
	{
		//CleanupAfterBuild();
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
		return nullptr;
	}

	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
		return nullptr;
	}

	// start building
	//const float tcs = m_cfg.tileSize*m_cfg.cs;
	ctx->startTimer(RC_TIMER_TEMP);

	//QuadrantTiler Tiler1, Tiler2, Tiler3, Tiler4, Tiler5, Tiler6;
	//TileAdder Adder;
	rcContext dummyCtx;

	dispatcher.Reset();
	dispatcher.maxHeight = th;
	dispatcher.maxWidth = tw;

	int numThreads = std::min(numCores, 8);
	boost::thread_group *threadGroup = new boost::thread_group[numThreads];
	for (int i = 0; i < numThreads; i++)
	{
		/*
		QuadrantTiler newTiler;
		newTiler.numTiles = &m_numTiles;
		newTiler.geom = geom;
		newTiler.m_cfg = m_cfg;
		newTiler.ctx = ctx;
		newTiler.ctx->enableLog(true);
		*/
		MyThread TilerThread;
		TilerThread.resize();
		threadGroup->create_thread(TilerThread);
	}
	threadGroup->join_all();
	for (int x = 0; x < SMPL->getTileWidth(); x++) {
		for (int y = 0; y < SMPL->getTileHeight(); y++) {
			TileData myData = mymap2[x][y];
			if (myData.DataSize > 0) {
				dtStatus status1 = m_navMesh->addTile(myData.Data, myData.DataSize, DT_TILE_FREE_DATA, 0, nullptr);
				if (dtStatusFailed(status1))
					continue;
			}
		}
	}
	/*
	Adder.ctx = ctx;
	Adder.ctx->enableLog(true);
	Adder.mesh = m_navMesh;
	Adder.numThreads = numThreads;
	boost::thread AdderThread(boost::ref(Adder));
	AdderThread.join();
	*/
	//delete threadGroup;
	// Start the build process.
	ctx->stopTimer(RC_TIMER_TEMP);
	return m_navMesh;
}

void Sample_TileMesh::buildAllTiles()
{
	if (!INPT) return;
	if (!m_navMesh) return;

	const float* bmin = INPT->getMeshBoundsMin();
	const float* bmax = INPT->getMeshBoundsMax();
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int ts = static_cast<int>(m_tileSize);
	const int tw = (gw + ts - 1) / ts;
	const int th = (gh + ts - 1) / ts;
	const float tcs = m_tileSize*m_cellSize;

	// Start the build process.
	ctx->startTimer(RC_TIMER_TEMP);

	for (int y = 0; y < th; ++y)
	{
		tileTriCount = 0;
		for (int x = 0; x < tw; ++x)
		{
			m_tileBmin[0] = bmin[0] + x*tcs;
			m_tileBmin[1] = bmin[1];
			m_tileBmin[2] = bmin[2] + y*tcs;

			m_tileBmax[0] = bmin[0] + (x + 1)*tcs;
			m_tileBmax[1] = bmax[1];
			m_tileBmax[2] = bmin[2] + (y + 1)*tcs;

			int dataSize = 0;
			unsigned char* data = buildTileMesh(x, y, m_tileBmin, m_tileBmax, dataSize);
			if (data)
			{
				// Remove any previous data (navmesh owns and deletes the data).
				m_navMesh->removeTile(m_navMesh->getTileRefAt(x, y, 0), nullptr, nullptr);
				// Let the navmesh own the data.
				dtStatus status = m_navMesh->addTile(data, dataSize, DT_TILE_FREE_DATA, 0, nullptr);
				if (dtStatusFailed(status))
					dtFree(data);
			}
		}
	}

	// Start the build process.
	ctx->stopTimer(RC_TIMER_TEMP);

	m_totalBuildTimeMs = ctx->getAccumulatedTime(RC_TIMER_TEMP) / 1000.0f;
}

void Sample_TileMesh::removeAllTiles() const
{
	const float* bmin = INPT->getMeshBoundsMin();
	const float* bmax = INPT->getMeshBoundsMax();
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int ts = static_cast<int>(m_tileSize);
	const int tw = (gw + ts - 1) / ts;
	const int th = (gh + ts - 1) / ts;

	for (int y = 0; y < th; ++y)
		for (int x = 0; x < tw; ++x)
			m_navMesh->removeTile(m_navMesh->getTileRefAt(x, y, 0), nullptr, nullptr);
}

void Sample_TileMesh::QuadrantTilerFunc()
{
}

unsigned char* Sample_TileMesh::buildTileMesh(const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize, BuildContext * ctx)
{
	if (!INPT || !INPT->getMesh() || !INPT->getChunkyMesh())
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Input mesh is not specified.");
		return nullptr;
	}

	m_tileMemUsage = 0;
	m_tileBuildTime = 0;

	cleanup();
	const rcChunkyTriMesh* chunkyMesh = INPT->getChunkyMesh();

	rcVcopy(m_cfg.bmin, bmin);
	rcVcopy(m_cfg.bmax, bmax);
	m_cfg.bmin[0] -= m_cfg.borderSize*m_cfg.cs;
	m_cfg.bmin[2] -= m_cfg.borderSize*m_cfg.cs;
	m_cfg.bmax[0] += m_cfg.borderSize*m_cfg.cs;
	m_cfg.bmax[2] += m_cfg.borderSize*m_cfg.cs;

	// Reset build times gathering.
	ctx->resetTimers();

	// Start the build process.
	ctx->startTimer(RC_TIMER_TOTAL);

	ctx->log(RC_LOG_PROGRESS, "Building navigation:");
	ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
	ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", INPT->getMesh()->getVertCount() / 1000.0f, INPT->getMesh()->getTriCount() / 1000.0f);

	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return nullptr;
	}
	if (!rcCreateHeightfield(ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return nullptr;
	}

	// Allocate array that can hold triangle flags.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
	if (!m_triareas)
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
		return nullptr;
	}

	float tbmin[2], tbmax[2];
	tbmin[0] = m_cfg.bmin[0];
	tbmin[1] = m_cfg.bmin[2];
	tbmax[0] = m_cfg.bmax[0];
	tbmax[1] = m_cfg.bmax[2];
	int cid[512];// TODO: Make grow when returning too many items.
	const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
	if (!ncid)
		return nullptr;

	m_tileTriCount = 0;

	for (int i = 0; i < ncid; ++i)
	{
		const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
		const int* tris = &chunkyMesh->tris[node.i * 3];
		const int ntris = node.n;

		m_tileTriCount += ntris;
		tileTriCount += ntris;
		memset(m_triareas, 0, ntris*sizeof(unsigned char));
		rcMarkWalkableTriangles(ctx, m_cfg.walkableSlopeAngle,
			&INPT->getMesh()->getVerts()[0], INPT->getMesh()->getVertCount(), tris, ntris, m_triareas);
		rcRasterizeTriangles(ctx, &INPT->getMesh()->getVerts()[0], INPT->getMesh()->getVertCount(), tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb);
	}

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
		m_triareas = nullptr;
	}

	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(ctx, m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(ctx, m_cfg.walkableHeight, *m_solid);

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return nullptr;
	}
	if (!rcBuildCompactHeightfield(ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return nullptr;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = nullptr;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(ctx, m_cfg.walkableRadius, *m_chf))
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return nullptr;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = INPT->getConvexVolumes();
	for (int i = 0; i < INPT->getConvexVolumeCount(); ++i)
		rcMarkConvexPolyArea(ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, static_cast<unsigned char>(vols[i].area), *m_chf);

	if (m_monotonePartitioning)
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegionsMonotone(ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
			return nullptr;
		}
	}
	else
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(ctx, *m_chf))
		{
			ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
			return nullptr;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
			return nullptr;
		}
	}

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return nullptr;
	}
	if (!rcBuildContours(ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return nullptr;
	}

	if (m_cset->nconts == 0)
	{
		return nullptr;
	}

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return nullptr;
	}
	if (!rcBuildPolyMesh(ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		return nullptr;
	}

	// Build detail mesh.
	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'dmesh'.");
		return nullptr;
	}

	if (!rcBuildPolyMeshDetail(ctx, *m_pmesh, *m_chf,
		m_cfg.detailSampleDist, m_cfg.detailSampleMaxError,
		*m_dmesh))
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could build polymesh detail.");
		return nullptr;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = nullptr;
		rcFreeContourSet(m_cset);
		m_cset = nullptr;
	}

	unsigned char* navData = nullptr;
	int navDataSize = 0;
	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		if (m_pmesh->nverts >= 0xffff)
		{
			// The vertex indices are ushorts, and cannot point to more than 0xffff vertices.
			ctx->log(RC_LOG_ERROR, "Too many vertices per tile %d (max: %d).", m_pmesh->nverts, 0xffff);
			return nullptr;
		}

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
				m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

			if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}

		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		params.offMeshConVerts = INPT->getOffMeshConnectionVerts();
		params.offMeshConRad = INPT->getOffMeshConnectionRads();
		params.offMeshConDir = INPT->getOffMeshConnectionDirs();
		params.offMeshConAreas = INPT->getOffMeshConnectionAreas();
		params.offMeshConFlags = INPT->getOffMeshConnectionFlags();
		params.offMeshConUserID = INPT->getOffMeshConnectionId();
		params.offMeshConCount = INPT->getOffMeshConnectionCount();
		params.walkableHeight = m_agentHeight;
		params.walkableRadius = m_agentRadius;
		params.walkableClimb = m_agentMaxClimb;
		params.tileX = tx;
		params.tileY = ty;
		params.tileLayer = 0;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		params.buildBvTree = true;

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			return nullptr;
		}
	}
	m_tileMemUsage = navDataSize / 1024.0f;

	ctx->stopTimer(RC_TIMER_TOTAL);

	// Show performance stats.
	duLogBuildTimes(*ctx, ctx->getAccumulatedTime(RC_TIMER_TOTAL));
	ctx->log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", m_pmesh->nverts, m_pmesh->npolys);

	m_tileBuildTime = ctx->getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;

	ctx->dumpLogToFile("NavMeshLog.txt");

	dataSize = navDataSize;
	return navData;
}

unsigned char* Sample_TileMesh::buildTileMesh2(const int tx, const int ty,
	const float* bmin, const float* bmax, int& dataSize,
	BuildContext* ctx)
{
	InputGeom *geom = INPT;
	if (!geom || !geom->getMesh() || !geom->getChunkyMesh())
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Input mesh is not specified.");
		return nullptr;
	}
	cleanup();
	const rcChunkyTriMesh* chunkyMesh = geom->getChunkyMesh();

	rcVcopy(m_cfg.bmin, bmin);
	rcVcopy(m_cfg.bmax, bmax);
	m_cfg.bmin[0] -= m_cfg.borderSize*m_cfg.cs;
	m_cfg.bmin[2] -= m_cfg.borderSize*m_cfg.cs;
	m_cfg.bmax[0] += m_cfg.borderSize*m_cfg.cs;
	m_cfg.bmax[2] += m_cfg.borderSize*m_cfg.cs;

	m_tileMemUsage = 0;
	m_tileBuildTime = 0;

	// Reset build times gathering.
	ctx->resetTimers();

	// Start the build process.
	ctx->startTimer(RC_TIMER_TOTAL);

	ctx->log(RC_LOG_PROGRESS, "Building navigation:");
	ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
	ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", INPT->getMesh()->getVertCount() / 1000.0f, INPT->getMesh()->getTriCount() / 1000.0f);

	// all involved objects
	rcHeightfield* m_solid;
	unsigned char* m_triareas;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcPolyMeshDetail* m_dmesh;

	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return nullptr;
	}
	if (!rcCreateHeightfield(ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return nullptr;
	}

	// Allocate array that can hold triangle flags.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
	if (!m_triareas)
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
		return nullptr;
	}

	float tbmin[2], tbmax[2];
	tbmin[0] = m_cfg.bmin[0];
	tbmin[1] = m_cfg.bmin[2];
	tbmax[0] = m_cfg.bmax[0];
	tbmax[1] = m_cfg.bmax[2];
	int cid[512];// TODO: Make grow when returning too many items.
	const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
	if (!ncid) {
		//CleanupAfterTileBuild();
		return nullptr;
	}

	int m_tileTriCount = 0;

	for (int i = 0; i < ncid; ++i)
	{
		const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
		const int* tris = &chunkyMesh->tris[node.i * 3];
		const int ntris = node.n;

		m_tileTriCount += ntris;

		memset(m_triareas, 0, ntris*sizeof(unsigned char));
		rcMarkWalkableTriangles(ctx, m_cfg.walkableSlopeAngle,
			&geom->getMesh()->getVerts()[0], INPT->getMesh()->getVertCount(), tris, ntris, m_triareas);
		rcRasterizeTriangles(ctx, &geom->getMesh()->getVerts()[0], INPT->getMesh()->getVertCount(), tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb);
	}

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
	}
	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.

	rcFilterLowHangingWalkableObstacles(ctx, m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(ctx, m_cfg.walkableHeight, *m_solid);

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return nullptr;
	}
	if (!rcBuildCompactHeightfield(ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
		return nullptr;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(ctx, m_cfg.walkableRadius, *m_chf))
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return nullptr;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = geom->getConvexVolumes();
	for (int i = 0; i < geom->getConvexVolumeCount(); ++i)
		rcMarkConvexPolyArea(ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, static_cast<unsigned char>(vols[i].area), *m_chf);

	if (m_monotonePartitioning)		// m_monotonePartitioning
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegionsMonotone(ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			//CleanupAfterTileBuild();
			ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
			return nullptr;
		}
	}
	else
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(ctx, *m_chf))
		{
			//CleanupAfterTileBuild();
			ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
			return nullptr;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			//CleanupAfterTileBuild();
			ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build regions.");
			return nullptr;
		}
	}

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return nullptr;
	}
	if (!rcBuildContours(ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return nullptr;
	}

	if (m_cset->nconts == 0)
	{
		CleanupAfterTileBuild();
		return nullptr;
	}

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return nullptr;
	}
	if (!rcBuildPolyMesh(ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		return nullptr;
	}

	// Build detail mesh.
	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'dmesh'.");
		return nullptr;
	}

	if (!rcBuildPolyMeshDetail(ctx, *m_pmesh, *m_chf,
		m_cfg.detailSampleDist, m_cfg.detailSampleMaxError,
		*m_dmesh))
	{
		//CleanupAfterTileBuild();
		ctx->log(RC_LOG_ERROR, "buildNavigation: Could build polymesh detail.");
		return nullptr;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		rcFreeContourSet(m_cset);
	}

	unsigned char* navData = nullptr;
	int navDataSize = 0;
	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		if (m_pmesh->nverts >= 0xffff)
		{
			//CleanupAfterTileBuild();

			// The vertex indices are ushorts, and cannot point to more than 0xffff vertices.
			ctx->log(RC_LOG_ERROR, "Too many vertices per tile %d (max: %d).", m_pmesh->nverts, 0xffff);
			return nullptr;
		}

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
				m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

			if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}

		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		params.offMeshConVerts = geom->getOffMeshConnectionVerts();
		params.offMeshConRad = geom->getOffMeshConnectionRads();
		params.offMeshConDir = geom->getOffMeshConnectionDirs();
		params.offMeshConAreas = geom->getOffMeshConnectionAreas();
		params.offMeshConFlags = geom->getOffMeshConnectionFlags();
		params.offMeshConUserID = geom->getOffMeshConnectionId();
		params.offMeshConCount = geom->getOffMeshConnectionCount();
		params.walkableHeight = m_cfg.walkableHeight;
		params.walkableRadius = m_cfg.walkableRadius;
		params.walkableClimb = m_cfg.walkableClimb;
		params.tileX = tx;
		params.tileY = ty;
		params.tileLayer = 0;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		params.buildBvTree = true;

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			//CleanupAfterTileBuild();
			ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			return nullptr;
		}
	}
	m_tileMemUsage = navDataSize / 1024.0f;
	ctx->stopTimer(RC_TIMER_TOTAL);

	// Show performance stats.
	duLogBuildTimes(*ctx, ctx->getAccumulatedTime(RC_TIMER_TOTAL));
	ctx->log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons", m_pmesh->nverts, m_pmesh->npolys);
	m_tileBuildTime = ctx->getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;

	dataSize = navDataSize;
	CleanupAfterTileBuild();

	return navData;
}

void Sample_TileMesh::DrawDebug() {
	//fill debugger
	mNavMeshDebugger->clear();
	mNavMeshDebugger->begin("mNavMesherDebuggerMaterial", Ogre::v1::RenderOperation::OT_LINE_LIST);

	const int nvp = m_pmesh->nvp;
	const float cs = m_pmesh->cs;
	const float ch = m_pmesh->ch;
	const float* orig = m_pmesh->bmin;

	for (int i = 0; i < m_pmesh->npolys; ++i)
	{
		const unsigned short* p = &m_pmesh->polys[i*nvp * 2];
		unsigned short vi[3];
		for (int j = 2; j < nvp; ++j)
		{
			if (p[j] == 0xffff) break;
			vi[0] = p[0];
			vi[1] = p[j - 1];
			vi[2] = p[j];
			for (int k = 0; k < 3 + 1; ++k)
			{
				const unsigned short* v = &m_pmesh->verts[vi[k % 3] * 3];
				const float x = orig[0] + v[0] * cs;
				const float y = orig[1] + (v[1] + 1)*ch;
				const float z = orig[2] + v[2] * cs;
				Vector3 cpos(x, y, z);
				mNavMeshDebugger->position(cpos.x, cpos.y, cpos.z);
				mNavMeshDebugger->colour(1, 1, 1);
			}
			Vector3 tpos = Vector3::ZERO;
			for (int k = 0; k < 3; ++k)
			{
				const unsigned short* v = &m_pmesh->verts[vi[k] * 3];
				const float x = orig[0] + v[0] * cs;
				const float y = orig[1] + (v[1] + 1)*ch;
				const float z = orig[2] + v[2] * cs;
				Vector3 cpos(x, y, z);
				tpos += cpos;
			}
			tpos /= 3;
			allPositions.push_back(tpos);

			mNavMeshDebugger->position(tpos.x, tpos.y, tpos.z);
			mNavMeshDebugger->colour(1, 1, 1);
			mNavMeshDebugger->position(tpos.x, tpos.y + 0.25, tpos.z);
			mNavMeshDebugger->colour(1, 1, 1);
		}
	}
	mNavMeshDebugger->end();
}