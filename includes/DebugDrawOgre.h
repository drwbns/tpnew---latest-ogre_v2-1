#ifndef _DEBUGDRAWOGRE 
#define _DEBUGDRAWOGRE


#include "InputGeom.h"
#include "Ogre.h"
#include "DetourTileCacheBuilder.h"
#include "Recast.h"

// Some math headers don't have PI defined.
static const float DU_PI = 3.14159265f;

enum DrawNavMeshFlags
{
	DU_DRAWNAVMESH_OFFMESHCONS = 0x01,
	DU_DRAWNAVMESH_CLOSEDLIST = 0x02,
	DU_DRAWNAVMESH_COLOR_TILES = 0x04,
};

inline unsigned int duRGBA(int r, int g, int b, int a)
{
	return ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) | ((unsigned int)a << 24);
}

inline unsigned int duRGBAf(float fr, float fg, float fb, float fa)
{
	unsigned char r = (unsigned char)(fr*255.0f);
	unsigned char g = (unsigned char)(fg*255.0f);
	unsigned char b = (unsigned char)(fb*255.0f);
	unsigned char a = (unsigned char)(fa*255.0f);
	return duRGBA(r,g,b,a);
}

class DebugDrawOgre {
public:
	~DebugDrawOgre();

	void resize(int cap);
	
	static const int MAX_CONVEXVOL_PTS = 12;

	unsigned int duIntToCol(int i, int a);
	void duIntToCol(int i, float* col);

	inline unsigned int duMultCol(const unsigned int col, const unsigned int d)
	{
		const unsigned int r = col & 0xff;
		const unsigned int g = (col >> 8) & 0xff;
		const unsigned int b = (col >> 16) & 0xff;
		const unsigned int a = (col >> 24) & 0xff;
		return duRGBA((r*d) >> 8, (g*d) >> 8, (b*d) >> 8, a);
	}

	inline unsigned int duDarkenCol(unsigned int col)
	{
		return ((col >> 1) & 0x007f7f7f) | (col & 0xff000000);
	}

	inline unsigned int duLerpCol(unsigned int ca, unsigned int cb, unsigned int u)
	{
		const unsigned int ra = ca & 0xff;
		const unsigned int ga = (ca >> 8) & 0xff;
		const unsigned int ba = (ca >> 16) & 0xff;
		const unsigned int aa = (ca >> 24) & 0xff;
		const unsigned int rb = cb & 0xff;
		const unsigned int gb = (cb >> 8) & 0xff;
		const unsigned int bb = (cb >> 16) & 0xff;
		const unsigned int ab = (cb >> 24) & 0xff;

		unsigned int r = (ra*(255-u) + rb*u)/255;
		unsigned int g = (ga*(255-u) + gb*u)/255;
		unsigned int b = (ba*(255-u) + bb*u)/255;
		unsigned int a = (aa*(255-u) + ab*u)/255;
		return duRGBA(r,g,b,a);
	}

	inline unsigned int duTransCol(unsigned int c, unsigned int a)
	{
		return (a<<24) | (c & 0x00ffffff);
	}

	inline int bit(int a, int b)
	{
		return (a & (1 << b)) >> b;
	}

	void appendArrowHead(Ogre::ManualObject * dd, const float* p, const float* q,
		const float s, unsigned int col);

	void duAppendBoxWire(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, unsigned int col);

	void duDebugDrawCylinderWire(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, unsigned int col, const float lineWidth);

	void duDebugDrawBoxWire(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, unsigned int col, const float lineWidth);

	void duDebugDrawArc(Ogre::ManualObject* dd, const float x0, const float y0, const float z0,
		const float x1, const float y1, const float z1, const float h,
		const float as0, const float as1, unsigned int col, const float lineWidth);

	void duDebugDrawArrow(Ogre::ManualObject* dd, const float x0, const float y0, const float z0,
		const float x1, const float y1, const float z1,
		const float as0, const float as1, unsigned int col, const float lineWidth);

	void duDebugDrawCircle(Ogre::ManualObject* dd, const float x, const float y, const float z,
		const float r, unsigned int col, const float lineWidth);

	void duDebugDrawCross(Ogre::ManualObject* dd, const float x, const float y, const float z,
		const float size, unsigned int col, const float lineWidth);

	void duDebugDrawBox(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, const unsigned int* fcol);

	void duDebugDrawCylinder(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, unsigned int col);

	void duDebugDrawGridXZ(Ogre::ManualObject* dd, const float ox, const float oy, const float oz,
		const int w, const int h, const float size,
		const unsigned int col, const float lineWidth);


	// Versions without begin/end, can be used to draw multiple primitives.
	void duAppendCylinderWire(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, unsigned int col);

	void duAppendBoxPoints(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, unsigned int col);

	void duAppendArc(Ogre::ManualObject* dd, const float x0, const float y0, const float z0,
		const float x1, const float y1, const float z1, const float h,
		const float as0, const float as1, unsigned int col);

	void duAppendArrow(Ogre::ManualObject* dd, const float x0, const float y0, const float z0,
		const float x1, const float y1, const float z1,
		const float as0, const float as1, unsigned int col);

	void duAppendCircle(Ogre::ManualObject* dd, const float x, const float y, const float z,
		const float r, unsigned int col);

	void duAppendCross(Ogre::ManualObject* dd, const float x, const float y, const float z,
		const float size, unsigned int col);

	void duAppendBox(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, const unsigned int* fcol);

	void duAppendCylinder(Ogre::ManualObject* dd, float minx, float miny, float minz,
		float maxx, float maxy, float maxz, unsigned int col);

	void drawOffMeshConnections(Ogre::ManualObject* dd, bool hilight);
	void drawConvexVolumes(Ogre::ManualObject* dd, bool /*hilight*/);

	void duDebugDrawNavMesh(Ogre::ManualObject * dd, const dtNavMesh& mesh, unsigned char flags);
	void duDebugDrawNavMeshWithClosedList(Ogre::ManualObject * dd, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags);
	void duDebugDrawNavMeshNodes(Ogre::ManualObject * dd, const dtNavMeshQuery& query);
	void duDebugDrawNavMeshBVTree(Ogre::ManualObject * dd, const dtNavMesh& mesh);
	void duDebugDrawNavMeshPortals(Ogre::ManualObject * dd, const dtNavMesh& mesh);
	void duDebugDrawNavMeshPoly(Ogre::ManualObject * dd, const dtNavMesh& mesh, dtPolyRef ref, const unsigned int col);

	void duDebugDrawTileCacheLayer(Ogre::ManualObject * dd, const dtTileCacheLayer& layer, const float cs, const float ch);

	void duDebugDrawTriMesh(Ogre::ManualObject * dd, const float* verts, int nverts, const int* tris, const float* normals, int ntris, const unsigned char* flags, const float texScale);
	void duDebugDrawTriMeshSlope(Ogre::ManualObject * dd, const float* verts, int nverts, const size_t* tris, const float* normals, int ntris, const float walkableSlopeAngle, const float texScale);

	void duDebugDrawHeightfieldSolid(Ogre::ManualObject * dd, const struct rcHeightfield& hf);
	void duDebugDrawHeightfieldWalkable(Ogre::ManualObject * dd, const struct rcHeightfield& hf);

	void duDebugDrawCompactHeightfieldSolid(Ogre::ManualObject * dd, const struct rcCompactHeightfield& chf);
	void duDebugDrawCompactHeightfieldRegions(Ogre::ManualObject * dd, const struct rcCompactHeightfield& chf);
	void duDebugDrawCompactHeightfieldDistance(Ogre::ManualObject * dd, const struct rcCompactHeightfield& chf);

	void duDebugDrawHeightfieldLayer(Ogre::ManualObject * dd, const struct rcHeightfieldLayer& layer, const int idx);
	void duDebugDrawHeightfieldLayers(Ogre::ManualObject * dd, const struct rcHeightfieldLayerSet& lset);
	void duDebugDrawHeightfieldLayersRegions(Ogre::ManualObject * dd, const struct rcHeightfieldLayerSet& lset);

	//void duDebugDrawLayerContours(Ogre::ManualObject * dd, const struct rcLayerContourSet& lcset);
	//void duDebugDrawLayerPolyMesh(Ogre::ManualObject * dd, const struct rcLayerPolyMesh& lmesh);


	void duDebugDrawRegionConnections(Ogre::ManualObject * dd, const struct rcContourSet& cset, const float alpha = 1.0f);
	void duDebugDrawRawContours(Ogre::ManualObject * dd, const struct rcContourSet& cset, const float alpha = 1.0f);
	void duDebugDrawContours(Ogre::ManualObject * dd, const struct rcContourSet& cset, const float alpha = 1.0f);
	void duDebugDrawPolyMesh(Ogre::ManualObject * dd, const struct rcPolyMesh& mesh);
	void duDebugDrawPolyMeshDetail(Ogre::ManualObject * dd, const struct rcPolyMeshDetail& dmesh);

	void drawPolyBoundaries(Ogre::ManualObject * dd, const dtMeshTile* tile,
		const unsigned int col, const float linew,
		bool inner);

	void drawMeshTile(Ogre::ManualObject * dd, const dtNavMesh& mesh, const dtNavMeshQuery* query,
		const dtMeshTile* tile, unsigned char flags);

	void drawMeshTileBVTree(Ogre::ManualObject * dd, const dtMeshTile* tile);

	void drawMeshTilePortal(Ogre::ManualObject * dd, const dtMeshTile* tile);
	void duCalcBoxColors(unsigned int* colors, unsigned int colTop, unsigned int colSide);
	void drawLayerPortals(Ogre::ManualObject * dd, const rcHeightfieldLayer* layer);
	const rcContour* findContourFromSet(const rcContourSet& cset, unsigned short reg);
	void getContourCenter(const rcContour* cont, const float* orig, float cs, float ch, float* center);
	//void duDebugDrawGridXZ(struct duDebugDraw* dd, const float ox, const float oy, const float oz,
	//	const int w, const int h, const float size,
	//	const unsigned int col, const float lineWidth);
protected:
	float* m_pos;
	bool m_depthMask;
	int m_size;
	int m_cap;
	unsigned int* m_color;
	//duDebugDrawPrimitives m_prim;
	float m_primSize;

	// Convex Volumes.
	static const int MAX_VOLUMES = 256;
	ConvexVolume m_volumes[MAX_VOLUMES];
	int m_volumeCount;

	// Off-Mesh connections.
	static const int MAX_OFFMESH_CONNECTIONS = 256;
	float m_offMeshConVerts[MAX_OFFMESH_CONNECTIONS*3*2];
	float m_offMeshConRads[MAX_OFFMESH_CONNECTIONS];
	unsigned char m_offMeshConDirs[MAX_OFFMESH_CONNECTIONS];
	unsigned char m_offMeshConAreas[MAX_OFFMESH_CONNECTIONS];
	unsigned short m_offMeshConFlags[MAX_OFFMESH_CONNECTIONS];
	unsigned int m_offMeshConId[MAX_OFFMESH_CONNECTIONS];
	int m_offMeshConCount;
};

#endif