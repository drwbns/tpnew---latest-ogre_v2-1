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
 
#ifndef RECAST_H
#define RECAST_H

// Some math headers don't have PI defined.
static const float RC_PI = 3.14159265f;

enum rcLogCategory
{
	RC_LOG_PROGRESS = 1,
	RC_LOG_WARNING,
	RC_LOG_ERROR,
};

enum rcTimerLabel
{
	RC_TIMER_TOTAL,
	RC_TIMER_TEMP,
	RC_TIMER_RASTERIZE_TRIANGLES,
	RC_TIMER_BUILD_COMPACTHEIGHTFIELD,
	RC_TIMER_BUILD_CONTOURS,
	RC_TIMER_BUILD_CONTOURS_TRACE,
	RC_TIMER_BUILD_CONTOURS_SIMPLIFY,
	RC_TIMER_FILTER_BORDER,
	RC_TIMER_FILTER_WALKABLE,
	RC_TIMER_MEDIAN_AREA,
	RC_TIMER_FILTER_LOW_OBSTACLES,
	RC_TIMER_BUILD_POLYMESH,
	RC_TIMER_MERGE_POLYMESH,
	RC_TIMER_ERODE_AREA,
	RC_TIMER_MARK_BOX_AREA,
	RC_TIMER_MARK_CYLINDER_AREA,
	RC_TIMER_MARK_CONVEXPOLY_AREA,
	RC_TIMER_BUILD_DISTANCEFIELD,
	RC_TIMER_BUILD_DISTANCEFIELD_DIST,
	RC_TIMER_BUILD_DISTANCEFIELD_BLUR,
	RC_TIMER_BUILD_REGIONS,
	RC_TIMER_BUILD_REGIONS_WATERSHED,
	RC_TIMER_BUILD_REGIONS_EXPAND,
	RC_TIMER_BUILD_REGIONS_FLOOD,
	RC_TIMER_BUILD_REGIONS_FILTER,
	RC_TIMER_BUILD_LAYERS,
	RC_TIMER_BUILD_POLYMESHDETAIL,
	RC_TIMER_MERGE_POLYMESHDETAIL,
	RC_MAX_TIMERS
};

/// Build context provides several optional utilities needed for the build process,
/// such as timing, logging, and build time collecting.
class rcContext
{
public:
	 rcContext(bool state = true) : m_logEnabled(state), m_timerEnabled(state) {}
	virtual ~rcContext() {}

	/// Enables or disables logging.
	 void enableLog(bool state) { m_logEnabled = state; }
	/// Resets log.
	 void resetLog() { if (m_logEnabled) doResetLog(); }
	/// Logs a message.
	virtual void log(const rcLogCategory category, const char* format, ...);

	/// Enables or disables timer.
	 void enableTimer(bool state) { m_timerEnabled = state; }
	/// Resets all timers.
	 void resetTimers() { if (m_timerEnabled) doResetTimers(); }
	/// Starts timer, used for performance timing.
	 void startTimer(const rcTimerLabel label) { if (m_timerEnabled) doStartTimer(label); }
	/// Stops timer, used for performance timing.
	 void stopTimer(const rcTimerLabel label) { if (m_timerEnabled) doStopTimer(label); }
	/// Returns time accumulated between timer start/stop.
	 int getAccumulatedTime(const rcTimerLabel label) const { return m_timerEnabled ? doGetAccumulatedTime(label) : -1; }

protected:

	/// @name Virtual functions to override for custom implementations.
	///@{
	virtual void doResetLog() {}
	virtual void doLog(const rcLogCategory /*category*/, const char* /*msg*/, const int /*len*/) {}
	virtual void doResetTimers() {}
	virtual void doStartTimer(const rcTimerLabel /*label*/) {}
	virtual void doStopTimer(const rcTimerLabel /*label*/) {}
	virtual int doGetAccumulatedTime(const rcTimerLabel /*label*/) const { return -1; }
	///@}
	
	bool m_logEnabled;
	bool m_timerEnabled;
};


/// The units of the parameters are specified in parenthesis as follows:
/// (vx) voxels, (wu) world units
struct rcConfig
{
	int width, height;				///< Dimensions of the rasterized heightfield (vx)
	int tileSize;					///< Width and Height of a tile (vx)
	int borderSize;					///< Non-navigable Border around the heightfield (vx)
	float cs, ch;					///< Grid cell size and height (wu)
	float bmin[3], bmax[3];			///< Grid bounds (wu)
	float walkableSlopeAngle;		///< Maximum walkable slope angle in degrees.
	int walkableHeight;				///< Minimum height where the agent can still walk (vx)
	int walkableClimb;				///< Maximum height between grid cells the agent can climb (vx)
	int walkableRadius;				///< Radius of the agent in cells (vx)
	int maxEdgeLen;					///< Maximum contour edge length (vx)
	float maxSimplificationError;	///< Maximum distance error from contour to cells (vx)
	int minRegionArea;				///< Regions whose area is smaller than this threshold will be removed. (vx)
	int mergeRegionArea;			///< Regions whose area is smaller than this threshold will be merged (vx)
	int maxVertsPerPoly;			///< Max number of vertices per polygon
	float detailSampleDist;			///< Detail mesh sample spacing.
	float detailSampleMaxError;		///< Detail mesh simplification max sample error.
};

/// Define number of bits in the above structure for smin/smax.
static const int RC_SPAN_HEIGHT_BITS = 13;
/// The max height is used for clamping rasterized values.
static const int RC_SPAN_MAX_HEIGHT = (1<<RC_SPAN_HEIGHT_BITS)-1;

/// Heightfield span.
struct rcSpan
{
	unsigned int smin : 13;			///< Span min height.
	unsigned int smax : 13;			///< Span max height.
	unsigned int area : 6;			///< Span area type.
	rcSpan* next;					///< Next span in column.
};

/// Number of spans allocated per pool.
static const int RC_SPANS_PER_POOL = 2048;

/// Memory pool used for quick span allocation.
struct rcSpanPool
{
	rcSpanPool* next;					///< Pointer to next pool.
	rcSpan items[RC_SPANS_PER_POOL];	///< Array of spans.
};

/// Dynamic span-heightfield.
struct rcHeightfield
{
	int width, height;			///< Dimension of the heightfield.
	float bmin[3], bmax[3];		///< Bounding box of the heightfield
	float cs, ch;				///< Cell size and height.
	rcSpan** spans;				///< Heightfield of spans (width*height).
	rcSpanPool* pools;			///< Linked list of span pools.
	rcSpan* freelist;			///< Pointer to next free span.
};

rcHeightfield* rcAllocHeightfield();
void rcFreeHeightField(rcHeightfield* hf);


struct rcCompactCell
{
	unsigned int index : 24;	///< Index to first span in column.
	unsigned int count : 8;		///< Number of spans in this column.
};

struct rcCompactSpan
{
	unsigned short y;			///< Bottom coordinate of the span.
	unsigned short reg;
	unsigned int con : 24;		///< Connections to neighbour cells.
	unsigned int h : 8;			///< Height of the span.
};

/// Compact static heightfield.
struct rcCompactHeightfield
{
	int width, height;					///< Width and height of the heightfield.
	int spanCount;						///< Number of spans in the heightfield.
	int walkableHeight, walkableClimb;	///< Agent properties.
	int borderSize;						///< Border size of the heighfield.
	unsigned short maxDistance;			///< Maximum distance value stored in heightfield.
	unsigned short maxRegions;			///< Maximum Region Id stored in heightfield.
	float bmin[3], bmax[3];				///< Bounding box of the heightfield.
	float cs, ch;						///< Cell size and height.
	rcCompactCell* cells;				///< Pointer to width*height cells.
	rcCompactSpan* spans;				///< Pointer to spans.
	unsigned short* dist;				///< Pointer to per span distance to border.
	unsigned char* areas;				///< Pointer to per span area ID.
};

rcCompactHeightfield* rcAllocCompactHeightfield();
void rcFreeCompactHeightfield(rcCompactHeightfield* chf);



struct rcHeightfieldLayer
{
	float bmin[3], bmax[3];				///< Bounding box of the heightfield.
	float cs, ch;						///< Cell size and height.
	int width, height;					///< Width and height of the layer.
	int minx,maxx,miny,maxy;			///< Bounding box of usable data.
	int hmin, hmax;						///< Height min/max
	unsigned char* heights;				///< Heighfield.
	unsigned char* areas;				///< Area types.
	unsigned char* cons;				///< Connections.
};

struct rcHeightfieldLayerSet
{
	rcHeightfieldLayer* layers;			///< Pointer to layers.
	int nlayers;						///< Number of layers.
};

rcHeightfieldLayerSet* rcAllocHeightfieldLayerSet();
void rcFreeHeightfieldLayerSet(rcHeightfieldLayerSet* lset);



struct rcContour
{
	int* verts;			///< Vertex coordinates, each vertex contains 4 components.
	int nverts;			///< Number of vertices.
	int* rverts;		///< Raw vertex coordinates, each vertex contains 4 components.
	int nrverts;		///< Number of raw vertices.
	unsigned short reg;	///< Region ID of the contour.
	unsigned char area;	///< Area ID of the contour.
};

struct rcContourSet
{
	rcContour* conts;		///< Pointer to all contours.
	int nconts;				///< Number of contours.
	float bmin[3], bmax[3];	///< Bounding box of the heightfield.
	float cs, ch;			///< Cell size and height.
	int width, height;		///< Region where the contours were build.
	int borderSize;			///< Border size of the heighfield where the contours were build from.
};

rcContourSet* rcAllocContourSet();
void rcFreeContourSet(rcContourSet* cset);


/// Polymesh store a connected mesh of polygons.
/// The polygons are store in an array where each polygons takes
/// 'nvp*2' elements. The first 'nvp' elements are indices to vertices
/// and the second 'nvp' elements are indices to neighbour polygons.
/// If a polygon has less than 'bvp' vertices, the remaining indices
/// are set to RC_MESH_NULL_IDX. If an polygon edge does not have a neighbour
/// the neighbour index is set to RC_MESH_NULL_IDX.
/// Vertices can be transformed into world space as follows:
///   x = bmin[0] + verts[i*3+0]*cs;
///   y = bmin[1] + verts[i*3+1]*ch;
///   z = bmin[2] + verts[i*3+2]*cs;
struct rcPolyMesh
{	
	unsigned short* verts;	///< Vertices of the mesh, 3 elements per vertex.
	unsigned short* polys;	///< Polygons of the mesh, nvp*2 elements per polygon.
	unsigned short* regs;	///< Region ID of the polygons.
	unsigned short* flags;	///< Per polygon flags.
	unsigned char* areas;	///< Area ID of polygons.
	int nverts;				///< Number of vertices.
	int npolys;				///< Number of polygons.
	int maxpolys;			///< Number of allocated polygons.
	int nvp;				///< Max number of vertices per polygon.
	float bmin[3], bmax[3];	///< Bounding box of the mesh.
	float cs, ch;			///< Cell size and height.
	int borderSize;			///< Border size of the heighfield where the mesh was build from.
};

rcPolyMesh* rcAllocPolyMesh();
void rcFreePolyMesh(rcPolyMesh* pmesh);


/// Detail mesh generated from a rcPolyMesh.
/// Each submesh represents a polygon in the polymesh and they are stored in
/// exactly same order. Each submesh is described as 4 values:
/// base vertex, vertex count, base triangle, triangle count. That is,
///   const unsigned char* t = &dmesh.tris[(tbase+i)*4]; and
///   const float* v = &dmesh.verts[(vbase+t[j])*3];
/// If the input polygon has 'n' vertices, those vertices are first in the
/// submesh vertex list. This allows to compres the mesh by not storing the
/// first vertices and using the polymesh vertices instead.
/// Max number of vertices per submesh is 127 and
/// max number of triangles per submesh is 255.
struct rcPolyMeshDetail
{
	unsigned int* meshes;	///< Pointer to all mesh data.
	float* verts;			///< Pointer to all vertex data.
	unsigned char* tris;	///< Pointer to all triangle data.
	int nmeshes;			///< Number of meshes.
	int nverts;				///< Number of total vertices.
	int ntris;				///< Number of triangles.
};

rcPolyMeshDetail* rcAllocPolyMeshDetail();
void rcFreePolyMeshDetail(rcPolyMeshDetail* dmesh);


/// If heightfield region ID has the following bit set, the region is on border area
/// and excluded from many calculations.
static const unsigned short RC_BORDER_REG = 0x8000;

/// If contour region ID has the following bit set, the vertex will be later
/// removed in order to match the segments and vertices at tile boundaries.
static const int RC_BORDER_VERTEX = 0x10000;

static const int RC_AREA_BORDER = 0x20000;

enum rcBuildContoursFlags
{
	RC_CONTOUR_TESS_WALL_EDGES = 0x01,	///< Tessellate wall edges
	RC_CONTOUR_TESS_AREA_EDGES = 0x02,	///< Tessellate edges between areas.
};

/// Mask used with contours to extract region id.
static const int RC_CONTOUR_REG_MASK = 0xffff;

/// Null index which is used with meshes to mark unset or invalid indices.
static const unsigned short RC_MESH_NULL_IDX = 0xffff;

/// Area ID that is considered empty.
static const unsigned char RC_NULL_AREA = 0;

/// Area ID that is considered generally walkable.
static const unsigned char RC_WALKABLE_AREA = 63;

/// Value returned by rcGetCon() if the direction is not connected.
static const int RC_NOT_CONNECTED = 0x3f;

/// Compact span neighbour helpers.
inline void rcSetCon(rcCompactSpan& s, int dir, int i)
{
	const unsigned int shift = (unsigned int)dir*6;
	unsigned int con = s.con;
	s.con = (con & ~(0x3f << shift)) | (((unsigned int)i & 0x3f) << shift);
}

inline int rcGetCon(const rcCompactSpan& s, int dir)
{
	const unsigned int shift = (unsigned int)dir*6;
	return (s.con >> shift) & 0x3f;
}

inline int rcGetDirOffsetX(int dir)
{
	const int offset[4] = { -1, 0, 1, 0, };
	return offset[dir&0x03];
}

inline int rcGetDirOffsetY(int dir)
{
	const int offset[4] = { 0, 1, 0, -1 };
	return offset[dir&0x03];
}

/// @name Common helper functions
///@{
template<class T> inline void rcSwap(T& a, T& b) { T t = a; a = b; b = t; }
template<class T> inline T rcMin(T a, T b) { return a < b ? a : b; }
template<class T> inline T rcMax(T a, T b) { return a > b ? a : b; }
template<class T> inline T rcAbs(T a) { return a < 0 ? -a : a; }
template<class T> inline T rcSqr(T a) { return a*a; }
template<class T> inline T rcClamp(T v, T mn, T mx) { return v < mn ? mn : (v > mx ? mx : v); }
float rcSqrt(float x);
inline int rcAlign4(int x) { return (x+3) & ~3; }
///@}

/// @name Common vector helper functions.
///@{
inline void rcVcross(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[1]*v2[2] - v1[2]*v2[1];
	dest[1] = v1[2]*v2[0] - v1[0]*v2[2];
	dest[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

inline float rcVdot(const float* v1, const float* v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

inline void rcVmad(float* dest, const float* v1, const float* v2, const float s)
{
	dest[0] = v1[0]+v2[0]*s;
	dest[1] = v1[1]+v2[1]*s;
	dest[2] = v1[2]+v2[2]*s;
}

inline void rcVadd(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[0]+v2[0];
	dest[1] = v1[1]+v2[1];
	dest[2] = v1[2]+v2[2];
}

inline void rcVsub(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[0]-v2[0];
	dest[1] = v1[1]-v2[1];
	dest[2] = v1[2]-v2[2];
}

inline void rcVmin(float* mn, const float* v)
{
	mn[0] = rcMin(mn[0], v[0]);
	mn[1] = rcMin(mn[1], v[1]);
	mn[2] = rcMin(mn[2], v[2]);
}

inline void rcVmax(float* mx, const float* v)
{
	mx[0] = rcMax(mx[0], v[0]);
	mx[1] = rcMax(mx[1], v[1]);
	mx[2] = rcMax(mx[2], v[2]);
}

inline void rcVcopy(float* dest, const float* v)
{
	dest[0] = v[0];
	dest[1] = v[1];
	dest[2] = v[2];
}

inline float rcVdist(const float* v1, const float* v2)
{
	float dx = v2[0] - v1[0];
	float dy = v2[1] - v1[1];
	float dz = v2[2] - v1[2];
	return rcSqrt(dx*dx + dy*dy + dz*dz);
}

inline float rcVdistSqr(const float* v1, const float* v2)
{
	float dx = v2[0] - v1[0];
	float dy = v2[1] - v1[1];
	float dz = v2[2] - v1[2];
	return dx*dx + dy*dy + dz*dz;
}

inline void rcVnormalize(float* v)
{
	float d = 1.0f / rcSqrt(rcSqr(v[0]) + rcSqr(v[1]) + rcSqr(v[2]));
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
}

inline bool rcVequal(const float* p0, const float* p1)
{
	static const float thr = rcSqr(1.0f/16384.0f);
	const float d = rcVdistSqr(p0, p1);
	return d < thr;
}
///@}

/// Calculated bounding box of array of vertices.
///  @param verts [in] array of vertices
///  @param nv [in] vertex count
///  @param bmin,bmax [out] bounding box
void rcCalcBounds(const float* verts, int nv, float* bmin, float* bmax);

/// Calculates grid size based on bounding box and grid cell size.
///  @param bmin,bmax [in] bounding box
///  @param cs [in] grid cell size
///  @param w [out] grid width
///  @param h [out] grid height
void rcCalcGridSize(const float* bmin, const float* bmax, float cs, int* w, int* h);

/// Creates and initializes new heightfield.
///  @param hf [in,out] heightfield to initialize.
///  @param width [in] width of the heightfield.
///  @param height [in] height of the heightfield.
///  @param bmin,bmax [in] bounding box of the heightfield
///  @param cs [in] grid cell size
///  @param ch [in] grid cell height
bool rcCreateHeightfield(rcContext* ctx, rcHeightfield& hf, int width, int height,
						 const float* bmin, const float* bmax,
						 float cs, float ch);

/// Sets the RC_WALKABLE_AREA for every triangle whose slope is below
/// the maximum walkable slope angle.
///  @param walkableSlopeAngle [in] maximum slope angle in degrees.
///  @param verts [in] array of vertices
///  @param nv [in] vertex count
///  @param tris [in] array of triangle vertex indices
///  @param nt [in] triangle count
///  @param areas [out] array of triangle area types
void rcMarkWalkableTriangles(rcContext* ctx, const float walkableSlopeAngle, const float* verts, int nv,
							 const int* tris, int nt, unsigned char* areas); 

/// Sets the RC_NULL_AREA for every triangle whose slope is steeper than
/// the maximum walkable slope angle.
///  @param walkableSlopeAngle [in] maximum slope angle in degrees.
///  @param verts [in] array of vertices
///  @param nv [in] vertex count
///  @param tris [in] array of triangle vertex indices
///  @param nt [in] triangle count
///  @param areas [out] array of triangle are types
void rcClearUnwalkableTriangles(rcContext* ctx, const float walkableSlopeAngle, const float* verts, int nv,
								const int* tris, int nt, unsigned char* areas); 

/// Adds span to heightfield.
/// The span addition can set to favor flags. If the span is merged to
/// another span and the new smax is within 'flagMergeThr' units away
/// from the existing span the span flags are merged and stored.
///  @param x,y [in] location on the heightfield where the span is added
///  @param smin,smax [in] spans min/max height
///  @param area
///  @param flagMergeThr [in] merge threshold.
void rcAddSpan(rcContext* ctx, rcHeightfield& hf, const int x, const int y,
			   const unsigned short smin, const unsigned short smax,
			   const unsigned char area, const int flagMergeThr);

/// Rasterizes a triangle into heightfield spans.
///  @param v0,v1,v2 [in] the vertices of the triangle.
///  @param area [in] area type of the triangle.
///  @param solid [in] heightfield where the triangle is rasterized
///  @param flagMergeThr [in] distance in voxel where walkable flag is favored over non-walkable.
void rcRasterizeTriangle(rcContext* ctx, const float* v0, const float* v1, const float* v2,
						 const unsigned char area, rcHeightfield& solid,
						 const int flagMergeThr = 1);

/// Rasterizes indexed triangle mesh into heightfield spans.
///  @param verts [in] array of vertices
///  @param nv [in] vertex count
///  @param tris [in] array of triangle vertex indices
///  @param areas [in] array of triangle area types.
///  @param nt [in] triangle count
///  @param solid [in] heightfield where the triangles are rasterized
///  @param flagMergeThr [in] distance in voxel where walkable flag is favored over non-walkable.
void rcRasterizeTriangles(rcContext* ctx, const float* verts, const int nv,
						  const int* tris, const unsigned char* areas, const int nt,
						  rcHeightfield& solid, const int flagMergeThr = 1);

/// Rasterizes indexed triangle mesh into heightfield spans.
///  @param verts [in] array of vertices
///  @param nv [in] vertex count
///  @param tris [in] array of triangle vertex indices
///  @param areas [in] array of triangle area types.
///  @param nt [in] triangle count
///  @param solid [in] heightfield where the triangles are rasterized
///  @param flagMergeThr [in] distance in voxel where walkable flag is favored over non-walkable.
void rcRasterizeTriangles(rcContext* ctx, const float* verts, const int nv,
						  const unsigned short* tris, const unsigned char* areas, const int nt,
						  rcHeightfield& solid, const int flagMergeThr = 1);

/// Rasterizes the triangles into heightfield spans.
///  @param verts [in] array of vertices
///  @param areas [in] array of triangle area types.
///  @param nt [in] triangle count
///  @param solid [in] heightfield where the triangles are rasterized
void rcRasterizeTriangles(rcContext* ctx, const float* verts, const unsigned char* areas, const int nt,
						  rcHeightfield& solid, const int flagMergeThr = 1);

/// Marks non-walkable low obstacles as walkable if they are closer than walkableClimb
/// from a walkable surface. Applying this filter allows to step over low hanging
/// low obstacles.
///  @param walkableClimb [in] maximum height between grid cells the agent can climb
///  @param solid [in,out] heightfield describing the solid space
/// @warning TODO: Misses ledge flag, must be called before rcFilterLedgeSpans!
void rcFilterLowHangingWalkableObstacles(rcContext* ctx, const int walkableClimb, rcHeightfield& solid);

/// Removes WALKABLE flag from all spans that are at ledges. This filtering
/// removes possible overestimation of the conservative voxelization so that
/// the resulting mesh will not have regions hanging in air over ledges.
///  @param walkableHeight [in] minimum height where the agent can still walk
///  @param walkableClimb [in] maximum height between grid cells the agent can climb
///  @param solid [in,out] heightfield describing the solid space
void rcFilterLedgeSpans(rcContext* ctx, const int walkableHeight,
						const int walkableClimb, rcHeightfield& solid);

/// Removes WALKABLE flag from all spans which have smaller than
/// 'walkableHeight' clearance above them.
///  @param walkableHeight [in] minimum height where the agent can still walk
///  @param solid [in,out] heightfield describing the solid space
void rcFilterWalkableLowHeightSpans(rcContext* ctx, int walkableHeight, rcHeightfield& solid);

/// Returns number of spans contained in a heightfield.
///  @param hf [in] heightfield to be compacted
///  @returns number of spans.
int rcGetHeightFieldSpanCount(rcContext* ctx, rcHeightfield& hf);

/// Builds compact representation of the heightfield.
///  @param walkableHeight [in] minimum height where the agent can still walk
///  @param walkableClimb [in] maximum height between grid cells the agent can climb
///  @param hf [in] heightfield to be compacted
///  @param chf [out] compact heightfield representing the open space.
///  @returns false if operation ran out of memory.
bool rcBuildCompactHeightfield(rcContext* ctx, const int walkableHeight, const int walkableClimb,
							   rcHeightfield& hf, rcCompactHeightfield& chf);

/// Erodes walkable area.
///  @param radius [in] radius of erosion (max 255).
///  @param chf [in,out] compact heightfield to erode.
///  @returns false if operation ran out of memory.
bool rcErodeWalkableArea(rcContext* ctx, int radius, rcCompactHeightfield& chf);

/// Applies median filter to walkable area types, removing noise.
///  @param chf [in,out] compact heightfield to erode.
///  @returns false if operation ran out of memory.
bool rcMedianFilterWalkableArea(rcContext* ctx, rcCompactHeightfield& chf);

/// Marks the area of the convex polygon into the area type of the compact heightfield.
///  @param bmin,bmax [in] bounds of the axis aligned box.
///  @param areaId [in] area ID to mark.
///  @param chf [in,out] compact heightfield to mark.
void rcMarkBoxArea(rcContext* ctx, const float* bmin, const float* bmax, unsigned char areaId,
				   rcCompactHeightfield& chf);

/// Marks the area of the convex polygon into the area type of the compact heightfield.
///  @param verts [in] vertices of the convex polygon.
///  @param nverts [in] number of vertices in the polygon.
///  @param hmin,hmax [in] min and max height of the polygon.
///  @param areaId [in] area ID to mark.
///  @param chf [in,out] compact heightfield to mark.
void rcMarkConvexPolyArea(rcContext* ctx, const float* verts, const int nverts,
						  const float hmin, const float hmax, unsigned char areaId,
						  rcCompactHeightfield& chf);

/// Marks the area of the cylinder into the area type of the compact heightfield.
///  @param pos [in] center bottom location of hte cylinder.
///  @param r [in] radius of the cylinder.
///  @param h [in] height of the cylinder.
///  @param areaId [in] area ID to mark.
///  @param chf [in,out] compact heightfield to mark.
void rcMarkCylinderArea(rcContext* ctx, const float* pos,
						const float r, const float h, unsigned char areaId,
						rcCompactHeightfield& chf);

/// Builds distance field and stores it into the combat heightfield.
///  @param chf [in,out] compact heightfield representing the open space.
///  @returns false if operation ran out of memory.
bool rcBuildDistanceField(rcContext* ctx, rcCompactHeightfield& chf);

/// Divides the walkable heighfied into simple regions using watershed partitioning.
/// Each region has only one contour and no overlaps.
/// The regions are stored in the compact heightfield 'reg' field.
/// The process sometimes creates small regions. If the area of a regions is
/// smaller than 'mergeRegionArea' then the region will be merged with a neighbour
/// region if possible. If multiple regions form an area which is smaller than
/// 'minRegionArea' all the regions belonging to that area will be removed.
/// Here area means the count of spans in an area.
///  @param chf [in,out] compact heightfield representing the open space.
///  @param borderSize [in] Non-navigable Border around the heightfield.
///  @param minRegionArea [in] the smallest allowed region area.
///  @param maxMergeRegionArea [in] the largest allowed region area which can be merged.
///  @returns false if operation ran out of memory.
bool rcBuildRegions(rcContext* ctx, rcCompactHeightfield& chf,
					const int borderSize, const int minRegionArea, const int mergeRegionArea);

/// Divides the walkable heighfied into simple regions using simple monotone partitioning.
/// Each region has only one contour and no overlaps.
/// The regions are stored in the compact heightfield 'reg' field.
/// The process sometimes creates small regions. If the area of a regions is
/// smaller than 'mergeRegionArea' then the region will be merged with a neighbour
/// region if possible. If multiple regions form an area which is smaller than
/// 'minRegionArea' all the regions belonging to that area will be removed.
/// Here area means the count of spans in an area.
///  @param chf [in,out] compact heightfield representing the open space.
///  @param borderSize [in] Non-navigable Border around the heightfield.
///  @param minRegionArea [in] the smallest allowed regions size.
///  @param maxMergeRegionArea [in] the largest allowed regions size which can be merged.
///  @returns false if operation ran out of memory.
bool rcBuildRegionsMonotone(rcContext* ctx, rcCompactHeightfield& chf,
							const int borderSize, const int minRegionArea, const int mergeRegionArea);

/// Builds 2D layer representation of a heighfield.
///  @param chf [in] compact heightfield representing the open space.
///  @param borderSize [in] Non-navigable Border around the heightfield.
///  @param walkableHeight [in] minimum height where the agent can still walk.
///  @param lset [out] set of 2D heighfield layers.
///  @returns false if operation ran out of memory.
bool rcBuildHeightfieldLayers(rcContext* ctx, rcCompactHeightfield& chf, 
							  const int borderSize, const int walkableHeight,
							  rcHeightfieldLayerSet& lset);

/// Builds simplified contours from the regions outlines.
///  @param chf [in] compact heightfield which has regions set.
///  @param maxError [in] maximum allowed distance between simplified contour and cells.
///  @param maxEdgeLen [in] maximum allowed contour edge length in cells.
///  @param cset [out] Resulting contour set.
///  @param flags [in] build flags, see rcBuildContoursFlags.
///  @returns false if operation ran out of memory.
bool rcBuildContours(rcContext* ctx, rcCompactHeightfield& chf,
					 const float maxError, const int maxEdgeLen,
					 rcContourSet& cset, const int flags = RC_CONTOUR_TESS_WALL_EDGES);

/// Builds connected convex polygon mesh from contour polygons.
///  @param cset [in] contour set.
///  @param nvp [in] maximum number of vertices per polygon.
///  @param mesh [out] poly mesh.
///  @returns false if operation ran out of memory.
bool rcBuildPolyMesh(rcContext* ctx, rcContourSet& cset, const int nvp, rcPolyMesh& mesh);

bool rcMergePolyMeshes(rcContext* ctx, rcPolyMesh** meshes, const int nmeshes, rcPolyMesh& mesh);

/// Builds detail triangle mesh for each polygon in the poly mesh.
///  @param mesh [in] poly mesh to detail.
///  @param chf [in] compact height field, used to query height for new vertices.
///  @param sampleDist [in] spacing between height samples used to generate more detail into mesh.
///  @param sampleMaxError [in] maximum allowed distance between simplified detail mesh and height sample.
///  @param dmesh [out] detail mesh.
///  @returns false if operation ran out of memory.
bool rcBuildPolyMeshDetail(rcContext* ctx, const rcPolyMesh& mesh, const rcCompactHeightfield& chf,
						   const float sampleDist, const float sampleMaxError,
						   rcPolyMeshDetail& dmesh);

bool rcMergePolyMeshDetails(rcContext* ctx, rcPolyMeshDetail** meshes, const int nmeshes, rcPolyMeshDetail& mesh);



#endif // RECAST_H
