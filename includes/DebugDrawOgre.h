#ifndef _DEBUGDRAWOGRE 
#define _DEBUGDRAWOGRE


#include "InputGeom.h"
#include "OGRE\OgreManualObject.h"
#include "Detour\DetourTileCacheBuilder.h"
#include "Recast\Recast.h"
#include "Detour/DetourNavMesh.h"

class dtNavMeshQuery;
class dtNavMesh;
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
	unsigned char r = (unsigned char)(fr * 255.0f);
	unsigned char g = (unsigned char)(fg * 255.0f);
	unsigned char b = (unsigned char)(fb * 255.0f);
	unsigned char a = (unsigned char)(fa * 255.0f);
	return duRGBA(r, g, b, a);
}

class DebugDrawOgre
{
	DebugDrawOgre(float* m_pos, bool m_depth_mask, int m_size, int m_cap, unsigned* m_color, float m_prim_size, int m_volume_count, int m_off_mesh_con_count)
		: m_pos(m_pos),
		  m_depthMask(m_depth_mask),
		  m_size(m_size),
		  m_cap(m_cap),
		  m_color(m_color),
		  m_primSize(m_prim_size),
		  m_volumeCount(m_volume_count),
		  m_offMeshConCount(m_off_mesh_con_count)
	{
	}

public:
	~DebugDrawOgre();

	void resize(int cap);

	static const int MAX_CONVEXVOL_PTS = 12;

	static unsigned int duIntToCol(int i, int a);
	static void duIntToCol(int i, float* col);

	static inline unsigned int duMultCol(const unsigned int col, const unsigned int d)
	{
		const unsigned int r = col & 0xff;
		const unsigned int g = (col >> 8) & 0xff;
		const unsigned int b = (col >> 16) & 0xff;
		const unsigned int a = (col >> 24) & 0xff;
		return duRGBA((r * d) >> 8, (g * d) >> 8, (b * d) >> 8, a);
	}

	static inline unsigned int duDarkenCol(unsigned int col)
	{
		return ((col >> 1) & 0x007f7f7f) | (col & 0xff000000);
	}

	static inline unsigned int duLerpCol(unsigned int ca, unsigned int cb, unsigned int u)
	{
		const unsigned int ra = ca & 0xff;
		const unsigned int ga = (ca >> 8) & 0xff;
		const unsigned int ba = (ca >> 16) & 0xff;
		const unsigned int aa = (ca >> 24) & 0xff;
		const unsigned int rb = cb & 0xff;
		const unsigned int gb = (cb >> 8) & 0xff;
		const unsigned int bb = (cb >> 16) & 0xff;
		const unsigned int ab = (cb >> 24) & 0xff;

		unsigned int r = (ra * (255 - u) + rb * u) / 255;
		unsigned int g = (ga * (255 - u) + gb * u) / 255;
		unsigned int b = (ba * (255 - u) + bb * u) / 255;
		unsigned int a = (aa * (255 - u) + ab * u) / 255;
		return duRGBA(r, g, b, a);
	}

	static inline unsigned int duTransCol(unsigned int c, unsigned int a)
	{
		return (a << 24) | (c & 0x00ffffff);
	}

	static inline int bit(int a, int b)
	{
		return (a & (1 << b)) >> b;
	}

	static void appendArrowHead(ManualObject* dd, const float* p, const float* q,
	                            const float s);

	static void duAppendBoxWire(ManualObject* dd, float minx, float miny, float minz,
	                            float maxx, float maxy, float maxz, unsigned int col);

	static void duDebugDrawCylinderWire(ManualObject* dd, float minx, float miny, float minz,
	                             float maxx, float maxy, float maxz, unsigned int col, const float lineWidth);

	static void duDebugDrawBoxWire(ManualObject* dd, float minx, float miny, float minz,
	                               float maxx, float maxy, float maxz, unsigned int col);

	static void duDebugDrawArc(ManualObject* dd, const float x0, const float y0, const float z0,
	                           const float x1, const float y1, const float z1, const float h,
	                           const float as0, const float as1, unsigned int col);

	static void duDebugDrawArrow();

	static void duDebugDrawCircle(ManualObject* dd, const float x, const float y, const float z,
	                       const float r, unsigned int col, const float lineWidth);

	static void duDebugDrawCross();

	static void duDebugDrawBox();

	static void duDebugDrawCylinder();

	static void duDebugDrawGridXZ(ManualObject* dd, const float ox, const float oy, const float oz,
	                       const int w, const int h, const float size);


	// Versions without begin/end, can be used to draw multiple primitives.
	static void duAppendCylinderWire(ManualObject* dd, float minx, float miny, float minz,
	                          float maxx, float maxy, float maxz, unsigned int col);

	static void duAppendBoxPoints();

	static void duAppendArc(ManualObject* dd, const float x0, const float y0, const float z0,
	                 const float x1, const float y1, const float z1, const float h,
	                 const float as0, const float as1, unsigned int col);

	static void duAppendArrow(ManualObject* dd, const float x0, const float y0, const float z0,
	                   const float x1, const float y1, const float z1,
	                   const float as0, const float as1, unsigned int col);

	static void duAppendCircle(ManualObject* dd, const float x, const float y, const float z,
	                           const float r);

	static void duAppendCross(ManualObject* dd, const float x, const float y, const float z,
	                   const float size, unsigned int col);

	static void duAppendBox(ManualObject* dd, float minx, float miny, float minz,
	                        float maxx, float maxy, float maxz);

	static void duAppendCylinder();

	void drawOffMeshConnections(ManualObject* dd, bool hilight);
	void drawConvexVolumes(ManualObject* dd, bool /*hilight*/) const;

	void duDebugDrawNavMesh(ManualObject* dd, const dtNavMesh& mesh, unsigned char flags);
	void duDebugDrawNavMeshWithClosedList(ManualObject* dd, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags);
	static void duDebugDrawNavMeshNodes(ManualObject* dd, const dtNavMeshQuery& query);
	static void duDebugDrawNavMeshBVTree(ManualObject* dd, const dtNavMesh& mesh);
	static void duDebugDrawNavMeshPortals(ManualObject* dd, const dtNavMesh& mesh);
	static void duDebugDrawNavMeshPoly(ManualObject* dd, const dtNavMesh& mesh, dtPolyRef ref, const unsigned int col);

	void duDebugDrawTileCacheLayer(ManualObject* dd, const dtTileCacheLayer& layer, const float cs, const float ch) const;

	static void duDebugDrawTriMesh();
	static void duDebugDrawTriMeshSlope(ManualObject* dd, const float* verts, int nverts, const size_t* tris, const float* normals, int ntris, const float walkableSlopeAngle, const float texScale);

	static void duDebugDrawHeightfieldSolid(ManualObject* dd, const struct rcHeightfield& hf);
	void duDebugDrawHeightfieldWalkable(ManualObject* dd, const struct rcHeightfield& hf) const;

	static void duDebugDrawCompactHeightfieldSolid(ManualObject* dd, const struct rcCompactHeightfield& chf);
	static void duDebugDrawCompactHeightfieldRegions(ManualObject* dd, const struct rcCompactHeightfield& chf);
	static void duDebugDrawCompactHeightfieldDistance(ManualObject* dd, const struct rcCompactHeightfield& chf);

	void duDebugDrawHeightfieldLayer(ManualObject* dd, const struct rcHeightfieldLayer& layer, const int idx) const;
	void duDebugDrawHeightfieldLayers(ManualObject* dd, const struct rcHeightfieldLayerSet& lset) const;
	static void duDebugDrawHeightfieldLayersRegions(ManualObject* dd);

	//void duDebugDrawLayerContours(Ogre::ManualObject * dd, const struct rcLayerContourSet& lcset);
	//void duDebugDrawLayerPolyMesh(Ogre::ManualObject * dd, const struct rcLayerPolyMesh& lmesh);


	void duDebugDrawRegionConnections(ManualObject* dd, const struct rcContourSet& cset, const float alpha = 1.0f) const;
	void duDebugDrawRawContours(ManualObject* dd, const struct rcContourSet& cset, const float alpha = 1.0f) const;
	void duDebugDrawContours(ManualObject* dd, const struct rcContourSet& cset, const float alpha = 1.0f) const;
	static void duDebugDrawPolyMesh(ManualObject* dd, const struct rcPolyMesh& mesh);
	void duDebugDrawPolyMeshDetail(ManualObject* dd, const struct rcPolyMeshDetail& dmesh) const;

	static void drawPolyBoundaries(ManualObject* dd, const dtMeshTile* tile, const float linew,
	                               bool inner);

	void drawMeshTile(ManualObject* dd, const dtNavMesh& mesh, const dtNavMeshQuery* query,
	                  const dtMeshTile* tile, unsigned char flags);

	static void drawMeshTileBVTree(ManualObject* dd, const dtMeshTile* tile);

	static void drawMeshTilePortal(ManualObject* dd, const dtMeshTile* tile);
	static void duCalcBoxColors(unsigned int* colors, unsigned int colTop, unsigned int colSide);
	static void drawLayerPortals(ManualObject* dd, const rcHeightfieldLayer* layer);
	static const rcContour* findContourFromSet(const rcContourSet& cset, unsigned short reg);
	static void getContourCenter(const rcContour* cont, const float* orig, float cs, float ch, float* center);
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
	float m_offMeshConVerts[MAX_OFFMESH_CONNECTIONS * 3 * 2];
	float m_offMeshConRads[MAX_OFFMESH_CONNECTIONS];
	unsigned char m_offMeshConDirs[MAX_OFFMESH_CONNECTIONS];
	unsigned char m_offMeshConAreas[MAX_OFFMESH_CONNECTIONS];
	unsigned short m_offMeshConFlags[MAX_OFFMESH_CONNECTIONS];
	unsigned int m_offMeshConId[MAX_OFFMESH_CONNECTIONS];
	int m_offMeshConCount;
};

#endif

