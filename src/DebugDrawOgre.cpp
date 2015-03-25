#include "DebugDrawOgre.h"
#include "Recast.h"
#include "string.h"
#include "DetourNode.h"

inline void evalArc(const float x0, const float y0, const float z0,
					const float dx, const float dy, const float dz,
					const float h, const float u, float* res)
{
	res[0] = x0 + dx * u;
	res[1] = y0 + dy * u + h * (1-(u*2-1)*(u*2-1));
	res[2] = z0 + dz * u;
}


inline void vcross(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[1]*v2[2] - v1[2]*v2[1];
	dest[1] = v1[2]*v2[0] - v1[0]*v2[2];
	dest[2] = v1[0]*v2[1] - v1[1]*v2[0]; 
}

inline void vnormalize(float* v)
{
	float d = 1.0f / sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
}

inline void vsub(float* dest, const float* v1, const float* v2)
{
	dest[0] = v1[0]-v2[0];
	dest[1] = v1[1]-v2[1];
	dest[2] = v1[2]-v2[2];
}

inline float vdistSqr(const float* v1, const float* v2)
{
	const float x = v1[0]-v2[0];
	const float y = v1[1]-v2[1];
	const float z = v1[2]-v2[2];
	return x*x + y*y + z*z;
}


DebugDrawOgre::~DebugDrawOgre() {}

void DebugDrawOgre::resize(int cap)
{
	float* newPos = new float[cap*3];
	if (m_size)
		memcpy(newPos, m_pos, sizeof(float)*3*m_size);
	delete [] m_pos;
	m_pos = newPos;

	unsigned int* newColor = new unsigned int[cap];
	if (m_size)
		memcpy(newColor, m_color, sizeof(unsigned int)*m_size);
	delete [] m_color;
	m_color = newColor;

	m_cap = cap;
}

void DebugDrawOgre::duDebugDrawTriMeshSlope(Ogre::ManualObject* dd, const float *verts, int /*nverts*/,
							 const size_t* tris, const float* normals, int ntris,
							 const float walkableSlopeAngle, const float texScale)
{
	if (!dd) return;
	if (!verts) return;
	if (!tris) return;
	if (!normals) return;

	const float walkableThr = cosf(walkableSlopeAngle/180.0f*DU_PI);

	float uva[2];
	float uvb[2];
	float uvc[2];

	//dd->texture(true);

	const unsigned int unwalkable = duRGBA(192,128,0,255);

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i < ntris*3; i += 3)
	{
		const float* norm = &normals[i];
		unsigned int color;
		unsigned char a = (unsigned char)(220*(2+norm[0]+norm[1])/4);
		if (norm[1] < walkableThr)
			color = duLerpCol(duRGBA(a,a,a,255), unwalkable, 64);
		else
			color = duRGBA(a,a,a,255);

		const float* va = &verts[tris[i+0]*3];
		const float* vb = &verts[tris[i+1]*3];
		const float* vc = &verts[tris[i+2]*3];

		int ax = 0, ay = 0;
		if (rcAbs(norm[1]) > rcAbs(norm[ax]))
			ax = 1;
		if (rcAbs(norm[2]) > rcAbs(norm[ax]))
			ax = 2;
		ax = (1<<ax)&3; // +1 mod 3
		ay = (1<<ax)&3; // +1 mod 3

		uva[0] = va[ax]*texScale;
		uva[1] = va[ay]*texScale;
		uvb[0] = vb[ax]*texScale;
		uvb[1] = vb[ay]*texScale;
		uvc[0] = vc[ax]*texScale;
		uvc[1] = vc[ay]*texScale;

		dd->position((Ogre::Real)*va,(Ogre::Real)*vb,(Ogre::Real)*vc);
	}
	dd->end();

	//dd->texture(false);
}

void DebugDrawOgre::duDebugDrawBoxWire(Ogre::ManualObject* dd, float minx, float miny, float minz,
						float maxx, float maxy, float maxz, unsigned int col, const float lineWidth)
{
	if (!dd) return;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	duAppendBoxWire(dd, minx,miny,minz, maxx,maxy,maxz,col);
	dd->end();
}

void DebugDrawOgre::duDebugDrawArc(Ogre::ManualObject* dd, const float x0, const float y0, const float z0,
					const float x1, const float y1, const float z1, const float h,
					const float as0, const float as1, unsigned int col, const float lineWidth)
{
	if (!dd) return;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	duAppendArc(dd, x0,y0,z0, x1,y1,z1, h, as0, as1, col);
	dd->end();
}

void DebugDrawOgre::duAppendBoxWire(Ogre::ManualObject* dd, float minx, float miny, float minz,
					 float maxx, float maxy, float maxz, unsigned int col)
{
	if (!dd) return;
	// Top
	dd->position(minx, miny, minz);
	dd->position(maxx, miny, minz);
	dd->position(maxx, miny, minz);
	dd->position(maxx, miny, maxz);
	dd->position(maxx, miny, maxz);
	dd->position(minx, miny, maxz);
	dd->position(minx, miny, maxz);
	dd->position(minx, miny, minz);

	// bottom
	dd->position(minx, maxy, minz);
	dd->position(maxx, maxy, minz);
	dd->position(maxx, maxy, minz);
	dd->position(maxx, maxy, maxz);
	dd->position(maxx, maxy, maxz);
	dd->position(minx, maxy, maxz);
	dd->position(minx, maxy, maxz);
	dd->position(minx, maxy, minz);

	// Sides
	dd->position(minx, miny, minz);
	dd->position(minx, maxy, minz);
	dd->position(maxx, miny, minz);
	dd->position(maxx, maxy, minz);
	dd->position(maxx, miny, maxz);
	dd->position(maxx, maxy, maxz);
	dd->position(minx, miny, maxz);
	dd->position(minx, maxy, maxz);
}

void DebugDrawOgre::drawConvexVolumes(Ogre::ManualObject* dd, bool /*hilight*/)
{
	//dd->depthMask(false);

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duIntToCol(vol->area, 32);
		for (int j = 0, k = vol->nverts-1; j < vol->nverts; k = j++)
		{
			const float* va = &vol->verts[k*3];
			const float* vb = &vol->verts[j*3];

			dd->position(vol->verts[0],vol->hmax,vol->verts[2]);
			dd->position(vb[0],vol->hmax,vb[2]);
			dd->position(va[0],vol->hmax,va[2]);

			dd->position(va[0],vol->hmin,va[2]);
			dd->position(va[0],vol->hmax,va[2]);
			dd->position(vb[0],vol->hmax,vb[2]);

			dd->position(va[0],vol->hmin,va[2]);
			dd->position(vb[0],vol->hmax,vb[2]);
			dd->position(vb[0],vol->hmin,vb[2]);
		}
	}

	dd->end();

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duIntToCol(vol->area, 220);
		for (int j = 0, k = vol->nverts-1; j < vol->nverts; k = j++)
		{
			const float* va = &vol->verts[k*3];
			const float* vb = &vol->verts[j*3];
			dd->position(va[0],vol->hmin,va[2]);
			dd->position(vb[0],vol->hmin,vb[2]);
			dd->position(va[0],vol->hmax,va[2]);
			dd->position(vb[0],vol->hmax,vb[2]);
			dd->position(va[0],vol->hmin,va[2]);
			dd->position(va[0],vol->hmax,va[2]);
		}
	}
	dd->end();

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_POINT_LIST);
	for (int i = 0; i < m_volumeCount; ++i)
	{
		const ConvexVolume* vol = &m_volumes[i];
		unsigned int col = duDarkenCol(duIntToCol(vol->area, 255));
		for (int j = 0; j < vol->nverts; ++j)
		{
			dd->position(vol->verts[j*3+0],vol->verts[j*3+1]+0.1f,vol->verts[j*3+2]);
			dd->position(vol->verts[j*3+0],vol->hmin,vol->verts[j*3+2]);
			dd->position(vol->verts[j*3+0],vol->hmax,vol->verts[j*3+2]);
		}
	}
	dd->end();


	//dd->depthMask(true);
}

void DebugDrawOgre::drawOffMeshConnections(Ogre::ManualObject* dd, bool hilight)
{
	unsigned int conColor = duRGBA(192,0,128,192);
	unsigned int baseColor = duRGBA(0,0,0,64);
	//dd->depthMask(false);

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i < m_offMeshConCount; ++i)
	{
		float* v = &m_offMeshConVerts[i*3*2];

		dd->position(v[0],v[1],v[2]);
		dd->position(v[0],v[1]+0.2f,v[2]);

		dd->position(v[3],v[4],v[5]);
		dd->position(v[3],v[4]+0.2f,v[5]);

		duAppendCircle(dd, v[0],v[1]+0.1f,v[2], m_offMeshConRads[i],conColor);
		duAppendCircle(dd, v[3],v[4]+0.1f,v[5], m_offMeshConRads[i],conColor);

		if (hilight)
		{
			duAppendArc(dd, v[0],v[1],v[2], v[3],v[4],v[5], 0.25f,
				(m_offMeshConDirs[i]&1) ? 0.6f : 0.0f, 0.6f, conColor);
		}
	}	
	dd->end();

	//dd->depthMask(true);
}

void DebugDrawOgre::appendArrowHead(Ogre::ManualObject * dd, const float* p, const float* q,
					 const float s, unsigned int col)
{
	const float eps = 0.001f;
	if (!dd) return;
	if (vdistSqr(p,q) < eps*eps) return;
	float ax[3], ay[3] = {0,1,0}, az[3];
	vsub(az, q, p);
	vnormalize(az);
	vcross(ax, ay, az);
	vcross(ay, az, ax);
	vnormalize(ay);

	dd->position(p[0],p[1],p[2]);
	dd->position(p[0]+az[0]*s+ay[0]*s/2, p[1]+az[1]*s+ay[1]*s/2, p[2]+az[2]*s+ay[2]*s/2);
	dd->position(p[0]+az[0]*s+ax[0]*s/3, p[1]+az[1]*s+ax[1]*s/3, p[2]+az[2]*s+ax[2]*s/3);

	dd->position(p[0],p[1],p[2]);
	dd->position(p[0]+az[0]*s-ay[0]*s/2, p[1]+az[1]*s-ay[1]*s/2, p[2]+az[2]*s-ay[2]*s/2);
	dd->position(p[0]+az[0]*s-ax[0]*s/3, p[1]+az[1]*s-ax[1]*s/3, p[2]+az[2]*s-ax[2]*s/3);

}

void DebugDrawOgre::duAppendBox(Ogre::ManualObject * dd, float minx, float miny, float minz,
				 float maxx, float maxy, float maxz, const unsigned int* fcol)
{
	if (!dd) return;
	const float verts[8*3] =
	{
		minx, miny, minz,
		maxx, miny, minz,
		maxx, miny, maxz,
		minx, miny, maxz,
		minx, maxy, minz,
		maxx, maxy, minz,
		maxx, maxy, maxz,
		minx, maxy, maxz,
	};
	static const unsigned char inds[6*4] =
	{
		7, 6, 5, 4,
		0, 1, 2, 3,
		1, 5, 6, 2,
		3, 7, 4, 0,
		2, 6, 7, 3,
		0, 4, 5, 1,
	};

	const unsigned char* in = inds;
	for (int i = 0; i < 6; ++i)
	{
		//dd->vertex(&verts[*in*3], fcol[i]); in++;
		//dd->vertex(&verts[*in*3], fcol[i]); in++;
		//dd->vertex(&verts[*in*3], fcol[i]); in++;
		//dd->vertex(&verts[*in*3], fcol[i]); in++;
	}
}

void DebugDrawOgre::duAppendArc(Ogre::ManualObject * dd, const float x0, const float y0, const float z0,
				 const float x1, const float y1, const float z1, const float h,
				 const float as0, const float as1, unsigned int col)
{
	if (!dd) return;
	static const int NUM_ARC_PTS = 8;
	static const float PAD = 0.05f;
	static const float ARC_PTS_SCALE = (1.0f-PAD*2) / (float)NUM_ARC_PTS;
	const float dx = x1 - x0;
	const float dy = y1 - y0;
	const float dz = z1 - z0;
	const float len = sqrtf(dx*dx + dy*dy + dz*dz);
	float prev[3];
	evalArc(x0,y0,z0, dx,dy,dz, len*h, PAD, prev);
	for (int i = 1; i <= NUM_ARC_PTS; ++i)
	{
		const float u = PAD + i * ARC_PTS_SCALE;
		float pt[3];
		evalArc(x0,y0,z0, dx,dy,dz, len*h, u, pt);
		dd->position(prev[0],prev[1],prev[2]);
		dd->position(pt[0],pt[1],pt[2]);
		prev[0] = pt[0]; prev[1] = pt[1]; prev[2] = pt[2];
	}

	// End arrows
	if (as0 > 0.001f)
	{
		float p[3], q[3];
		evalArc(x0,y0,z0, dx,dy,dz, len*h, PAD, p);
		evalArc(x0,y0,z0, dx,dy,dz, len*h, PAD+0.05f, q);
		appendArrowHead(dd, p, q, as0, col);
	}

	if (as1 > 0.001f)
	{
		float p[3], q[3];
		evalArc(x0,y0,z0, dx,dy,dz, len*h, 1-PAD, p);
		evalArc(x0,y0,z0, dx,dy,dz, len*h, 1-(PAD+0.05f), q);
		appendArrowHead(dd, p, q, as1, col);
	}
}

void DebugDrawOgre::duDebugDrawCircle(Ogre::ManualObject * dd, const float x, const float y, const float z,
					   const float r, unsigned int col, const float lineWidth)
{
	if (!dd) return;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	duAppendCircle(dd, x,y,z, r, col);
	dd->end();
}

void DebugDrawOgre::duDebugDrawCylinderWire(Ogre::ManualObject * dd, float minx, float miny, float minz,
							 float maxx, float maxy, float maxz, unsigned int col, const float lineWidth)
{
	if (!dd) return;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	duAppendCylinderWire(dd, minx,miny,minz, maxx,maxy,maxz, col);
	dd->end();
}

void DebugDrawOgre::duAppendArrow(Ogre::ManualObject * dd, const float x0, const float y0, const float z0,
				   const float x1, const float y1, const float z1,
				   const float as0, const float as1, unsigned int col)
{
	if (!dd) return;

	dd->position(x0,y0,z0);
	dd->position(x1,y1,z1);

	// End arrows
	const float p[3] = {x0,y0,z0}, q[3] = {x1,y1,z1};
	if (as0 > 0.001f)
		appendArrowHead(dd, p, q, as0, col);
	if (as1 > 0.001f)
		appendArrowHead(dd, q, p, as1, col);
}

void DebugDrawOgre::duAppendCircle(Ogre::ManualObject * dd, const float x, const float y, const float z,
					const float r, unsigned int col)
{
	if (!dd) return;
	static const int NUM_SEG = 40;
	static float dir[40*2];
	static bool init = false;
	if (!init)
	{
		init = true;
		for (int i = 0; i < NUM_SEG; ++i)
		{
			const float a = (float)i/(float)NUM_SEG*DU_PI*2;
			dir[i*2] = cosf(a);
			dir[i*2+1] = sinf(a);
		}
	}

	for (int i = 0, j = NUM_SEG-1; i < NUM_SEG; j = i++)
	{
		dd->position(x+dir[j*2+0]*r, y, z+dir[j*2+1]*r);
		dd->position(x+dir[i*2+0]*r, y, z+dir[i*2+1]*r);
	}
}

void DebugDrawOgre::duAppendCross(Ogre::ManualObject * dd, const float x, const float y, const float z,
				   const float s, unsigned int col)
{
	if (!dd) return;
	dd->position(x-s,y,z);
	dd->position(x+s,y,z);
	dd->position(x,y-s,z);
	dd->position(x,y+s,z);
	dd->position(x,y,z-s);
	dd->position(x,y,z+s);
}

void DebugDrawOgre::duAppendCylinderWire(Ogre::ManualObject * dd, float minx, float miny, float minz,
						  float maxx, float maxy, float maxz, unsigned int col)
{
	if (!dd) return;

	static const int NUM_SEG = 16;
	static float dir[NUM_SEG*2];
	static bool init = false;
	if (!init)
	{
		init = true;
		for (int i = 0; i < NUM_SEG; ++i)
		{
			const float a = (float)i/(float)NUM_SEG*DU_PI*2;
			dir[i*2] = cosf(a);
			dir[i*2+1] = sinf(a);
		}
	}

	const float cx = (maxx + minx)/2;
	const float cz = (maxz + minz)/2;
	const float rx = (maxx - minx)/2;
	const float rz = (maxz - minz)/2;

	for (int i = 0, j = NUM_SEG-1; i < NUM_SEG; j = i++)
	{
		dd->position(cx+dir[j*2+0]*rx, miny, cz+dir[j*2+1]*rz);
		dd->position(cx+dir[i*2+0]*rx, miny, cz+dir[i*2+1]*rz);
		dd->position(cx+dir[j*2+0]*rx, maxy, cz+dir[j*2+1]*rz);
		dd->position(cx+dir[i*2+0]*rx, maxy, cz+dir[i*2+1]*rz);
	}
	for (int i = 0; i < NUM_SEG; i += NUM_SEG/4)
	{
		dd->position(cx+dir[i*2+0]*rx, miny, cz+dir[i*2+1]*rz);
		dd->position(cx+dir[i*2+0]*rx, maxy, cz+dir[i*2+1]*rz);
	}
}

static float distancePtLine2d(const float* pt, const float* p, const float* q)
{
	float pqx = q[0] - p[0];
	float pqz = q[2] - p[2];
	float dx = pt[0] - p[0];
	float dz = pt[2] - p[2];
	float d = pqx*pqx + pqz*pqz;
	float t = pqx*dx + pqz*dz;
	if (d != 0) t /= d;
	dx = p[0] + t*pqx - pt[0];
	dz = p[2] + t*pqz - pt[2];
	return dx*dx + dz*dz;
}

void DebugDrawOgre::drawPolyBoundaries(Ogre::ManualObject * dd, const dtMeshTile* tile,
							   const unsigned int col, const float linew,
							   bool inner)
{
	static const float thr = 0.01f*0.01f;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];

		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) continue;

		const dtPolyDetail* pd = &tile->detailMeshes[i];

		for (int j = 0, nj = (int)p->vertCount; j < nj; ++j)
		{
			unsigned int c = col;
			if (inner)
			{
				if (p->neis[j] == 0) continue;
				if (p->neis[j] & DT_EXT_LINK)
				{
					bool con = false;
					for (unsigned int k = p->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
					{
						if (tile->links[k].edge == j)
						{
							con = true;
							break;
						}
					}
					if (con)
						c = duRGBA(255,255,255,48);
					else
						c = duRGBA(0,0,0,48);
				}
				else
					c = duRGBA(0,48,64,32);
			}
			else
			{
				if (p->neis[j] != 0) continue;
			}

			const float* v0 = &tile->verts[p->verts[j]*3];
			const float* v1 = &tile->verts[p->verts[(j+1) % nj]*3];

			// Draw detail mesh edges which align with the actual poly edge.
			// This is really slow.
			for (int k = 0; k < pd->triCount; ++k)
			{
				const unsigned char* t = &tile->detailTris[(pd->triBase+k)*4];
				const float* tv[3];
				for (int m = 0; m < 3; ++m)
				{
					if (t[m] < p->vertCount)
						tv[m] = &tile->verts[p->verts[t[m]]*3];
					else
						tv[m] = &tile->detailVerts[(pd->vertBase+(t[m]-p->vertCount))*3];
				}
				for (int m = 0, n = 2; m < 3; n=m++)
				{
					if (((t[3] >> (n*2)) & 0x3) == 0) continue;	// Skip inner detail edges.
					if (distancePtLine2d(tv[n],v0,v1) < thr &&
						distancePtLine2d(tv[m],v0,v1) < thr)
					{
						//dd->vertex(tv[n], c);
						//dd->vertex(tv[m], c);
					}
				}
			}
		}
	}
	dd->end();
}

void DebugDrawOgre::drawMeshTile(Ogre::ManualObject * dd, const dtNavMesh& mesh, const dtNavMeshQuery* query,
						 const dtMeshTile* tile, unsigned char flags)
{
	dtPolyRef base = mesh.getPolyRefBase(tile);

	int tileNum = mesh.decodePolyIdTile(base);

	//dd->depthMask(false);

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];
		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
			continue;

		const dtPolyDetail* pd = &tile->detailMeshes[i];

		unsigned int col;
		if (query && query->isInClosedList(base | (dtPolyRef)i))
			col = duRGBA(255,196,0,64);
		else
		{
			if (flags & DU_DRAWNAVMESH_COLOR_TILES)
			{
				col = duIntToCol(tileNum, 128);
			}
			else
			{
				if (p->getArea() == 0) // Treat zero area type as default.
					col = duRGBA(0,192,255,64);
				else
					col = duIntToCol(p->getArea(), 64);
			}
		}

		for (int j = 0; j < pd->triCount; ++j)
		{
			const unsigned char* t = &tile->detailTris[(pd->triBase+j)*4];
			size_t k = 0;
			/*
			for (int k = 0; k < 3; ++k)
			{
				
				if (t[k] < p->vertCount)
					dd->position((Real)tile->verts[p->verts[t[k]]*3],(Real)tile->verts[p->verts[t[k+1]]*3],(Real)tile->verts[p->verts[t[k+2]]*3]);
				else
					dd->position((Real)tile->detailVerts[(pd->vertBase+t[k]-p->vertCount)*3],(Real)tile->detailVerts[(pd->vertBase+t[k+1]-p->vertCount)*3],(Real)tile->detailVerts[(pd->vertBase+t[k+2]-p->vertCount)*3]);
				
			}
			*/
		}
	}
	dd->end();

	// Draw inter poly boundaries
	drawPolyBoundaries(dd, tile, duRGBA(0,48,64,32), 1.5f, true);

	// Draw outer poly boundaries
	drawPolyBoundaries(dd, tile, duRGBA(0,48,64,220), 2.5f, false);

	if (flags & DU_DRAWNAVMESH_OFFMESHCONS)
	{
		dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
		for (int i = 0; i < tile->header->polyCount; ++i)
		{
			const dtPoly* p = &tile->polys[i];
			if (p->getType() != DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip regular polys.
				continue;

			unsigned int col;
			if (query && query->isInClosedList(base | (dtPolyRef)i))
				col = duRGBA(255,196,0,220);
			else
				col = duDarkenCol(duIntToCol(p->getArea(), 220));

			const dtOffMeshConnection* con = &tile->offMeshCons[i - tile->header->offMeshBase];
			const float* va = &tile->verts[p->verts[0]*3];
			const float* vb = &tile->verts[p->verts[1]*3];

			// Check to see if start and end end-points have links.
			bool startSet = false;
			bool endSet = false;
			for (unsigned int k = p->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
			{
				if (tile->links[k].edge == 0)
					startSet = true;
				if (tile->links[k].edge == 1)
					endSet = true;
			}

			// End points and their on-mesh locations. 
			if (startSet)
			{
				dd->position((Real)va[0],(Real)va[1],(Real)va[2]);
				dd->position((Real)con->pos[0],(Real)con->pos[1],(Real)con->pos[2]);
				duAppendCircle(dd, con->pos[0],con->pos[1]+0.1f,con->pos[2], con->rad, duRGBA(0,48,64,196));
			}
			if (endSet)
			{
				dd->position((Real)vb[0],(Real)vb[1],(Real)vb[2]);
				dd->position((Real)con->pos[3],(Real)con->pos[4],(Real)con->pos[5]);
				duAppendCircle(dd, con->pos[3],con->pos[4]+0.1f,con->pos[5], con->rad, duRGBA(0,48,64,196));
			}	

			// End point vertices.
			dd->position(con->pos[0],con->pos[1],con->pos[2]);
			dd->position(con->pos[0],con->pos[1]+0.2f,con->pos[2]);

			dd->position(con->pos[3],con->pos[4],con->pos[5]);
			dd->position(con->pos[3],con->pos[4]+0.2f,con->pos[5]);

			// Connection arc.
			duAppendArc(dd, con->pos[0],con->pos[1],con->pos[2], con->pos[3],con->pos[4],con->pos[5], 0.25f,
				(con->flags & 1) ? 0.6f : 0, 0.6f, col);
		}
		dd->end();
	}

	const unsigned int vcol = duRGBA(0,0,0,196);
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_POINT_LIST);
	for (int i = 0; i < tile->header->vertCount; ++i)
	{
		const float* v = &tile->verts[i*3];
		dd->position(v[0], v[1], v[2]);
	}
	dd->end();

	//dd->depthMask(true);
}

void DebugDrawOgre::duDebugDrawNavMesh(Ogre::ManualObject * dd, const dtNavMesh& mesh, unsigned char flags)
{
	if (!dd) return;

	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTile(dd, mesh, 0, tile, flags);
	}
}

void DebugDrawOgre::duDebugDrawNavMeshWithClosedList(Ogre::ManualObject * dd, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags)
{
	if (!dd) return;

	const dtNavMeshQuery* q = (flags & DU_DRAWNAVMESH_CLOSEDLIST) ? &query : 0;

	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTile(dd, mesh, q, tile, flags);
	}
}

void DebugDrawOgre::duDebugDrawNavMeshNodes(Ogre::ManualObject * dd, const dtNavMeshQuery& query)
{
	if (!dd) return;

	const dtNodePool* pool = query.getNodePool();
	if (pool)
	{
		const float off = 0.5f;
		dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_POINT_LIST);
		for (int i = 0; i < pool->getHashSize(); ++i)
		{
			for (dtNodeIndex j = pool->getFirst(i); j != DT_NULL_IDX; j = pool->getNext(j))
			{
				const dtNode* node = pool->getNodeAtIdx(j+1);
				if (!node) continue;
				dd->position(node->pos[0],node->pos[1]+off,node->pos[2]);
			}
		}
		dd->end();

		dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
		for (int i = 0; i < pool->getHashSize(); ++i)
		{
			for (dtNodeIndex j = pool->getFirst(i); j != DT_NULL_IDX; j = pool->getNext(j))
			{
				const dtNode* node = pool->getNodeAtIdx(j+1);
				if (!node) continue;
				if (!node->pidx) continue;
				const dtNode* parent = pool->getNodeAtIdx(node->pidx);
				if (!parent) continue;
				dd->position(node->pos[0],node->pos[1]+off,node->pos[2]);
				dd->position(parent->pos[0],parent->pos[1]+off,parent->pos[2]);
			}
		}
		dd->end();
	}
}


void DebugDrawOgre::drawMeshTileBVTree(Ogre::ManualObject * dd, const dtMeshTile* tile)
{
	// Draw BV nodes.
	const float cs = 1.0f / tile->header->bvQuantFactor;
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i < tile->header->bvNodeCount; ++i)
	{
		const dtBVNode* n = &tile->bvTree[i];
		if (n->i < 0) // Leaf indices are positive.
			continue;
		duAppendBoxWire(dd, tile->header->bmin[0] + n->bmin[0]*cs,
			tile->header->bmin[1] + n->bmin[1]*cs,
			tile->header->bmin[2] + n->bmin[2]*cs,
			tile->header->bmin[0] + n->bmax[0]*cs,
			tile->header->bmin[1] + n->bmax[1]*cs,
			tile->header->bmin[2] + n->bmax[2]*cs,
			duRGBA(255,255,255,128));
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawNavMeshBVTree(Ogre::ManualObject * dd, const dtNavMesh& mesh)
{
	if (!dd) return;

	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTileBVTree(dd, tile);
	}
}

void DebugDrawOgre::drawMeshTilePortal(Ogre::ManualObject * dd, const dtMeshTile* tile)
{
	// Draw portals
	const float padx = 0.04f;
	const float pady = tile->header->walkableClimb;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	for (int side = 0; side < 8; ++side)
	{
		unsigned short m = DT_EXT_LINK | (unsigned short)side;

		for (int i = 0; i < tile->header->polyCount; ++i)
		{
			dtPoly* poly = &tile->polys[i];

			// Create new links.
			const int nv = poly->vertCount;
			for (int j = 0; j < nv; ++j)
			{
				// Skip edges which do not point to the right side.
				if (poly->neis[j] != m)
					continue;

				// Create new links
				const float* va = &tile->verts[poly->verts[j]*3];
				const float* vb = &tile->verts[poly->verts[(j+1) % nv]*3];

				if (side == 0 || side == 4)
				{
					unsigned int col = side == 0 ? duRGBA(128,0,0,128) : duRGBA(128,0,128,128);

					const float x = va[0] + ((side == 0) ? -padx : padx);

					dd->position(x,va[1]-pady,va[2]);
					dd->position(x,va[1]+pady,va[2]);

					dd->position(x,va[1]+pady,va[2]);
					dd->position(x,vb[1]+pady,vb[2]);

					dd->position(x,vb[1]+pady,vb[2]);
					dd->position(x,vb[1]-pady,vb[2]);

					dd->position(x,vb[1]-pady,vb[2]);
					dd->position(x,va[1]-pady,va[2]);
				}
				else if (side == 2 || side == 6)
				{
					unsigned int col = side == 2 ? duRGBA(0,128,0,128) : duRGBA(0,128,128,128);

					const float z = va[2] + ((side == 2) ? -padx : padx);

					dd->position(va[0],va[1]-pady,z);
					dd->position(va[0],va[1]+pady,z);

					dd->position(va[0],va[1]+pady,z);
					dd->position(vb[0],vb[1]+pady,z);

					dd->position(vb[0],vb[1]+pady,z);
					dd->position(vb[0],vb[1]-pady,z);

					dd->position(vb[0],vb[1]-pady,z);
					dd->position(va[0],va[1]-pady,z);
				}

			}
		}
	}

	dd->end();
}

void DebugDrawOgre::duDebugDrawNavMeshPortals(Ogre::ManualObject * dd, const dtNavMesh& mesh)
{
	if (!dd) return;

	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTilePortal(dd, tile);
	}
}

void DebugDrawOgre::duDebugDrawNavMeshPoly(Ogre::ManualObject * dd, const dtNavMesh& mesh, dtPolyRef ref, const unsigned int col)
{
	if (!dd) return;

	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(mesh.getTileAndPolyByRef(ref, &tile, &poly)))
		return;

	//dd->depthMask(false);

	const unsigned int c = (col & 0x00ffffff) | (64 << 24);
	const unsigned int ip = (unsigned int)(poly - tile->polys);

	if (poly->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)
	{
		dtOffMeshConnection* con = &tile->offMeshCons[ip - tile->header->offMeshBase];

		dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);

		// Connection arc.
		duAppendArc(dd, con->pos[0],con->pos[1],con->pos[2], con->pos[3],con->pos[4],con->pos[5], 0.25f,
			(con->flags & 1) ? 0.6f : 0, 0.6f, c);

		dd->end();
	}
	else
	{
		const dtPolyDetail* pd = &tile->detailMeshes[ip];

		dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		for (int i = 0; i < pd->triCount; ++i)
		{
			const unsigned char* t = &tile->detailTris[(pd->triBase+i)*4];
			for (int j = 0; j < 3; ++j)
			{
				//if (t[j] < poly->vertCount)
					//dd->position(&tile->verts[poly->verts[t[j]]*3], c);
				//else
					//dd->position(&tile->detailVerts[(pd->vertBase+t[j]-poly->vertCount)*3], c);
			}
		}
		dd->end();
	}

	//dd->depthMask(true);

}

void DebugDrawOgre::duDebugDrawTileCacheLayer(Ogre::ManualObject * dd, const dtTileCacheLayer& layer, const float cs, const float ch)
{
	const int w = (int)layer.header->width;
	const int h = (int)layer.header->height;
	const float* bmin = layer.header->bmin;
	const float* bmax = layer.header->bmax;
	const int idx = layer.header->tlayer;

	unsigned int color = duIntToCol(idx+1, 255);

	// Layer bounds
	float lbmin[3], lbmax[3];
	lbmin[0] = bmin[0] + layer.header->minx*cs;
	lbmin[1] = bmin[1];
	lbmin[2] = bmin[2] + layer.header->miny*cs;
	lbmax[0] = bmin[0] + (layer.header->maxx+1)*cs;
	lbmax[1] = bmax[1];
	lbmax[2] = bmin[2] + (layer.header->maxy+1)*cs;
	duDebugDrawBoxWire(dd, lbmin[0],lbmin[1],lbmin[2], lbmax[0],lbmax[1],lbmax[2], duTransCol(color,128), 2.0f);

	// Layer height
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			const int idx = x+y*w;
			const int h = (int)layer.heights[idx];
			if (h == 0xff) continue;
			const unsigned char area = layer.areas[idx];

			unsigned int col;
			if (area == 63)
				col = duLerpCol(color, duRGBA(0,192,255,64), 32);
			else if (area == 0)
				col = duLerpCol(color, duRGBA(0,0,0,64), 32);
			else
				col = duLerpCol(color, duIntToCol(area, 255), 32);

			const float fx = bmin[0] + x*cs;
			const float fy = bmin[1] + (h+1)*ch;
			const float fz = bmin[2] + y*cs;

			dd->position(fx, fy, fz);
			dd->position(fx, fy, fz+cs);
			dd->position(fx+cs, fy, fz+cs);
			dd->position(fx+cs, fy, fz);
		}
	}
	dd->end();

	// Portals
	unsigned int pcol = duRGBA(255,255,255,255);

	const int segs[4*4] = {0,0,0,1, 0,1,1,1, 1,1,1,0, 1,0,0,0};

	// Layer portals
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			const int idx = x+y*w;
			const int h = (int)layer.heights[idx];
			if (h == 0xff) continue;

			for (int dir = 0; dir < 4; ++dir)
			{
				if (layer.cons[idx] & (1<<(dir+4)))
				{
					const int* seg = &segs[dir*4];
					const float ax = bmin[0] + (x+seg[0])*cs;
					const float ay = bmin[1] + (h+2)*ch;
					const float az = bmin[2] + (y+seg[1])*cs;
					const float bx = bmin[0] + (x+seg[2])*cs;
					const float by = bmin[1] + (h+2)*ch;
					const float bz = bmin[2] + (y+seg[3])*cs;
					dd->position(ax, ay, az);
					dd->position(bx, by, bz);
				}
			}
		}
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawHeightfieldSolid(Ogre::ManualObject * dd, const rcHeightfield& hf)
{
	if (!dd) return;

	const float* orig = hf.bmin;
	const float cs = hf.cs;
	const float ch = hf.ch;

	const int w = hf.width;
	const int h = hf.height;

	unsigned int fcol[6];
	duCalcBoxColors(fcol, duRGBA(255,255,255,255), duRGBA(255,255,255,255));

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			float fx = orig[0] + x*cs;
			float fz = orig[2] + y*cs;
			const rcSpan* s = hf.spans[x + y*w];
			while (s)
			{
				duAppendBox(dd, fx, orig[1]+s->smin*ch, fz, fx+cs, orig[1] + s->smax*ch, fz+cs, fcol);
				s = s->next;
			}
		}
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawHeightfieldWalkable(Ogre::ManualObject * dd, const rcHeightfield& hf)
{
	if (!dd) return;

	const float* orig = hf.bmin;
	const float cs = hf.cs;
	const float ch = hf.ch;

	const int w = hf.width;
	const int h = hf.height;

	unsigned int fcol[6];
	duCalcBoxColors(fcol, duRGBA(255,255,255,255), duRGBA(217,217,217,255));

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			float fx = orig[0] + x*cs;
			float fz = orig[2] + y*cs;
			const rcSpan* s = hf.spans[x + y*w];
			while (s)
			{
				if (s->area == RC_WALKABLE_AREA)
					fcol[0] = duRGBA(64,128,160,255);
				else if (s->area == RC_NULL_AREA)
					fcol[0] = duRGBA(64,64,64,255);
				else
					fcol[0] = duMultCol(duIntToCol(s->area, 255), 200);

				duAppendBox(dd, fx, orig[1]+s->smin*ch, fz, fx+cs, orig[1] + s->smax*ch, fz+cs, fcol);
				s = s->next;
			}
		}
	}

	dd->end();
}

void DebugDrawOgre::duDebugDrawCompactHeightfieldSolid(Ogre::ManualObject * dd, const rcCompactHeightfield& chf)
{
	if (!dd) return;

	const float cs = chf.cs;
	const float ch = chf.ch;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int y = 0; y < chf.height; ++y)
	{
		for (int x = 0; x < chf.width; ++x)
		{
			const float fx = chf.bmin[0] + x*cs;
			const float fz = chf.bmin[2] + y*cs;
			const rcCompactCell& c = chf.cells[x+y*chf.width];

			for (unsigned i = c.index, ni = c.index+c.count; i < ni; ++i)
			{
				const rcCompactSpan& s = chf.spans[i];

				unsigned int color;
				if (chf.areas[i] == RC_WALKABLE_AREA)
					color = duRGBA(0,192,255,64);
				else if (chf.areas[i] == RC_NULL_AREA)
					color = duRGBA(0,0,0,64);
				else
					color = duIntToCol(chf.areas[i], 255);

				const float fy = chf.bmin[1] + (s.y+1)*ch;
				dd->position(fx, fy, fz);
				dd->position(fx, fy, fz+cs);
				dd->position(fx+cs, fy, fz+cs);
				dd->position(fx+cs, fy, fz);
			}
		}
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawCompactHeightfieldRegions(Ogre::ManualObject * dd, const rcCompactHeightfield& chf)
{
	if (!dd) return;

	const float cs = chf.cs;
	const float ch = chf.ch;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int y = 0; y < chf.height; ++y)
	{
		for (int x = 0; x < chf.width; ++x)
		{
			const float fx = chf.bmin[0] + x*cs;
			const float fz = chf.bmin[2] + y*cs;
			const rcCompactCell& c = chf.cells[x+y*chf.width];

			for (unsigned i = c.index, ni = c.index+c.count; i < ni; ++i)
			{
				const rcCompactSpan& s = chf.spans[i];
				const float fy = chf.bmin[1] + (s.y)*ch;
				unsigned int color;
				if (s.reg)
					color = duIntToCol(s.reg, 192);
				else
					color = duRGBA(0,0,0,64);

				dd->position(fx, fy, fz);
				dd->position(fx, fy, fz+cs);
				dd->position(fx+cs, fy, fz+cs);
				dd->position(fx+cs, fy, fz);
			}
		}
	}

	dd->end();
}


void DebugDrawOgre::duDebugDrawCompactHeightfieldDistance(Ogre::ManualObject * dd, const rcCompactHeightfield& chf)
{
	if (!dd) return;
	if (!chf.dist) return;

	const float cs = chf.cs;
	const float ch = chf.ch;

	float maxd = chf.maxDistance;
	if (maxd < 1.0f) maxd = 1;
	const float dscale = 255.0f / maxd;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int y = 0; y < chf.height; ++y)
	{
		for (int x = 0; x < chf.width; ++x)
		{
			const float fx = chf.bmin[0] + x*cs;
			const float fz = chf.bmin[2] + y*cs;
			const rcCompactCell& c = chf.cells[x+y*chf.width];

			for (unsigned i = c.index, ni = c.index+c.count; i < ni; ++i)
			{
				const rcCompactSpan& s = chf.spans[i];
				const float fy = chf.bmin[1] + (s.y+1)*ch;
				const unsigned char cd = (unsigned char)(chf.dist[i] * dscale);
				const unsigned int color = duRGBA(cd,cd,cd,255);
				dd->position(fx, fy, fz);
				dd->position(fx, fy, fz+cs);
				dd->position(fx+cs, fy, fz+cs);
				dd->position(fx+cs, fy, fz);
			}
		}
	}
	dd->end();
}

void DebugDrawOgre::drawLayerPortals(Ogre::ManualObject * dd, const rcHeightfieldLayer* layer)
{
	const float cs = layer->cs;
	const float ch = layer->ch;
	const int w = layer->width;
	const int h = layer->height;

	unsigned int pcol = duRGBA(255,255,255,255);

	const int segs[4*4] = {0,0,0,1, 0,1,1,1, 1,1,1,0, 1,0,0,0};

	// Layer portals
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			const int idx = x+y*w;
			const int h = (int)layer->heights[idx];
			if (h == 255) continue;

			for (int dir = 0; dir < 4; ++dir)
			{
				if (layer->cons[idx] & (1<<(dir+4)))
				{
					const int* seg = &segs[dir*4];
					const float ax = layer->bmin[0] + (x+seg[0])*cs;
					const float ay = layer->bmin[1] + (h+2)*ch;
					const float az = layer->bmin[2] + (y+seg[1])*cs;
					const float bx = layer->bmin[0] + (x+seg[2])*cs;
					const float by = layer->bmin[1] + (h+2)*ch;
					const float bz = layer->bmin[2] + (y+seg[3])*cs;
					dd->position(ax, ay, az);
					dd->position(bx, by, bz);
				}
			}
		}
	}
	dd->end();
}

void DebugDrawOgre::duCalcBoxColors(unsigned int* colors, unsigned int colTop, unsigned int colSide)
{
	if (!colors) return;

	colors[0] = duMultCol(colTop, 250);
	colors[1] = duMultCol(colSide, 140);
	colors[2] = duMultCol(colSide, 165);
	colors[3] = duMultCol(colSide, 217);
	colors[4] = duMultCol(colSide, 165);
	colors[5] = duMultCol(colSide, 217);
}

void DebugDrawOgre::duDebugDrawHeightfieldLayer(Ogre::ManualObject * dd, const struct rcHeightfieldLayer& layer, const int idx)
{
	const float cs = layer.cs;
	const float ch = layer.ch;
	const int w = layer.width;
	const int h = layer.height;

	unsigned int color = duIntToCol(idx+1, 255);

	// Layer bounds
	float bmin[3], bmax[3];
	bmin[0] = layer.bmin[0] + layer.minx*cs;
	bmin[1] = layer.bmin[1];
	bmin[2] = layer.bmin[2] + layer.miny*cs;
	bmax[0] = layer.bmin[0] + (layer.maxx+1)*cs;
	bmax[1] = layer.bmax[1];
	bmax[2] = layer.bmin[2] + (layer.maxy+1)*cs;
	duDebugDrawBoxWire(dd, bmin[0],bmin[1],bmin[2], bmax[0],bmax[1],bmax[2], duTransCol(color,128), 2.0f);

	// Layer height
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			const int idx = x+y*w;
			const int h = (int)layer.heights[idx];
			if (h == 0xff) continue;
			const unsigned char area = layer.areas[idx];

			unsigned int col;
			if (area == RC_WALKABLE_AREA)
				col = duLerpCol(color, duRGBA(0,192,255,64), 32);
			else if (area == RC_NULL_AREA)
				col = duLerpCol(color, duRGBA(0,0,0,64), 32);
			else
				col = duLerpCol(color, duIntToCol(area, 255), 32);

			const float fx = layer.bmin[0] + x*cs;
			const float fy = layer.bmin[1] + (h+1)*ch;
			const float fz = layer.bmin[2] + y*cs;

			dd->position(fx, fy, fz);
			dd->position(fx, fy, fz+cs);
			dd->position(fx+cs, fy, fz+cs);
			dd->position(fx+cs, fy, fz);
		}
	}
	dd->end();

	// Portals
	drawLayerPortals(dd, &layer);
}

void DebugDrawOgre::duDebugDrawHeightfieldLayers(Ogre::ManualObject * dd, const struct rcHeightfieldLayerSet& lset)
{
	if (!dd) return;
	for (int i = 0; i < lset.nlayers; ++i)
		duDebugDrawHeightfieldLayer(dd, lset.layers[i], i);
}

unsigned int DebugDrawOgre::duIntToCol(int i, int a)
{
	int	r = bit(i, 1) + bit(i, 3) * 2 + 1;
	int	g = bit(i, 2) + bit(i, 4) * 2 + 1;
	int	b = bit(i, 0) + bit(i, 5) * 2 + 1;
	return duRGBA(r*63,g*63,b*63,a);
}

void DebugDrawOgre::duIntToCol(int i, float* col)
{
	int	r = bit(i, 0) + bit(i, 3) * 2 + 1;
	int	g = bit(i, 1) + bit(i, 4) * 2 + 1;
	int	b = bit(i, 2) + bit(i, 5) * 2 + 1;
	col[0] = 1 - r*63.0f/255.0f;
	col[1] = 1 - g*63.0f/255.0f;
	col[2] = 1 - b*63.0f/255.0f;
}

void DebugDrawOgre::duDebugDrawRegionConnections(Ogre::ManualObject * dd, const rcContourSet& cset, const float alpha)
{
	if (!dd) return;

	const float* orig = cset.bmin;
	const float cs = cset.cs;
	const float ch = cset.ch;

	// Draw centers
	float pos[3], pos2[3];

	unsigned int color = duRGBA(0,0,0,196);

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	for (int i = 0; i < cset.nconts; ++i)
	{
		const rcContour* cont = &cset.conts[i];
		getContourCenter(cont, orig, cs, ch, pos);
		for (int j = 0; j < cont->nverts; ++j)
		{
			const int* v = &cont->verts[j*4];
			if (v[3] == 0 || (unsigned short)v[3] < cont->reg) continue;
			const rcContour* cont2 = findContourFromSet(cset, (unsigned short)v[3]);
			if (cont2)
			{
				getContourCenter(cont2, orig, cs, ch, pos2);
				duAppendArc(dd, pos[0],pos[1],pos[2], pos2[0],pos2[1],pos2[2], 0.25f, 0.6f, 0.6f, color);
			}
		}
	}

	dd->end();

	unsigned char a = (unsigned char)(alpha * 255.0f);

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_POINT_LIST);

	for (int i = 0; i < cset.nconts; ++i)
	{
		const rcContour* cont = &cset.conts[i];
		unsigned int color = duDarkenCol(duIntToCol(cont->reg,a));
		getContourCenter(cont, orig, cs, ch, pos);
		dd->position(pos[0],pos[1],pos[2]);
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawRawContours(Ogre::ManualObject * dd, const rcContourSet& cset, const float alpha)
{
	if (!dd) return;

	const float* orig = cset.bmin;
	const float cs = cset.cs;
	const float ch = cset.ch;

	const unsigned char a = (unsigned char)(alpha*255.0f);

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	for (int i = 0; i < cset.nconts; ++i)
	{
		const rcContour& c = cset.conts[i];
		unsigned int color = duIntToCol(c.reg, a);

		for (int j = 0; j < c.nrverts; ++j)
		{
			const int* v = &c.rverts[j*4];
			float fx = orig[0] + v[0]*cs;
			float fy = orig[1] + (v[1]+1+(i&1))*ch;
			float fz = orig[2] + v[2]*cs;
			dd->position(fx,fy,fz);
			if (j > 0)
				dd->position(fx,fy,fz);
		}
		// Loop last segment.
		const int* v = &c.rverts[0];
		float fx = orig[0] + v[0]*cs;
		float fy = orig[1] + (v[1]+1+(i&1))*ch;
		float fz = orig[2] + v[2]*cs;
		dd->position(fx,fy,fz);
	}
	dd->end();

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_POINT_LIST);

	for (int i = 0; i < cset.nconts; ++i)
	{
		const rcContour& c = cset.conts[i];
		unsigned int color = duDarkenCol(duIntToCol(c.reg, a));

		for (int j = 0; j < c.nrverts; ++j)
		{
			const int* v = &c.rverts[j*4];
			float off = 0;
			unsigned int colv = color;
			if (v[3] & RC_BORDER_VERTEX)
			{
				colv = duRGBA(255,255,255,a);
				off = ch*2;
			}

			float fx = orig[0] + v[0]*cs;
			float fy = orig[1] + (v[1]+1+(i&1))*ch + off;
			float fz = orig[2] + v[2]*cs;
			dd->position(fx,fy,fz);
		}
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawContours(Ogre::ManualObject * dd, const rcContourSet& cset, const float alpha)
{
	if (!dd) return;

	const float* orig = cset.bmin;
	const float cs = cset.cs;
	const float ch = cset.ch;

	const unsigned char a = (unsigned char)(alpha*255.0f);

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	for (int i = 0; i < cset.nconts; ++i)
	{
		const rcContour& c = cset.conts[i];
		if (!c.nverts)
			continue;
		unsigned int color = duIntToCol(c.reg, a);

		for (int j = 0; j < c.nverts; ++j)
		{
			const int* v = &c.verts[j*4];
			float fx = orig[0] + v[0]*cs;
			float fy = orig[1] + (v[1]+1+(i&1))*ch;
			float fz = orig[2] + v[2]*cs;
			dd->position(fx,fy,fz);
			if (j > 0)
				dd->position(fx,fy,fz);
		}
		// Loop last segment
		const int* v = &c.verts[0];
		float fx = orig[0] + v[0]*cs;
		float fy = orig[1] + (v[1]+1+(i&1))*ch;
		float fz = orig[2] + v[2]*cs;
		dd->position(fx,fy,fz);
	}
	dd->end();

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_POINT_LIST);

	for (int i = 0; i < cset.nconts; ++i)
	{
		const rcContour& c = cset.conts[i];
		unsigned int color = duDarkenCol(duIntToCol(c.reg, a));
		for (int j = 0; j < c.nverts; ++j)
		{
			const int* v = &c.verts[j*4];
			float off = 0;
			unsigned int colv = color;
			if (v[3] & RC_BORDER_VERTEX)
			{
				colv = duRGBA(255,255,255,a);
				off = ch*2;
			}

			float fx = orig[0] + v[0]*cs;
			float fy = orig[1] + (v[1]+1+(i&1))*ch + off;
			float fz = orig[2] + v[2]*cs;
			dd->position(fx,fy,fz);
		}
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawPolyMesh(Ogre::ManualObject * dd, const struct rcPolyMesh& mesh)
{
	if (!dd) return;

	const int nvp = mesh.nvp;
	const float cs = mesh.cs;
	const float ch = mesh.ch;
	const float* orig = mesh.bmin;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int i = 0; i < mesh.npolys; ++i)
	{
		const unsigned short* p = &mesh.polys[i*nvp*2];

		unsigned int color;
		if (mesh.areas[i] == RC_WALKABLE_AREA)
			color = duRGBA(0,192,255,64);
		else if (mesh.areas[i] == RC_NULL_AREA)
			color = duRGBA(0,0,0,64);
		else
			color = duIntToCol(mesh.areas[i], 255);

		unsigned short vi[3];
		for (int j = 2; j < nvp; ++j)
		{
			if (p[j] == RC_MESH_NULL_IDX) break;
			vi[0] = p[0];
			vi[1] = p[j-1];
			vi[2] = p[j];
			for (int k = 0; k < 3; ++k)
			{
				const unsigned short* v = &mesh.verts[vi[k]*3];
				const float x = orig[0] + v[0]*cs;
				const float y = orig[1] + (v[1]+1)*ch;
				const float z = orig[2] + v[2]*cs;
				dd->position(x,y,z);
			}
		}
	}
	dd->end();

	// Draw neighbours edges
	const unsigned int coln = duRGBA(0,48,64,32);
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i < mesh.npolys; ++i)
	{
		const unsigned short* p = &mesh.polys[i*nvp*2];
		for (int j = 0; j < nvp; ++j)
		{
			if (p[j] == RC_MESH_NULL_IDX) break;
			if (p[nvp+j] & 0x8000) continue;
			const int nj = (j+1 >= nvp || p[j+1] == RC_MESH_NULL_IDX) ? 0 : j+1; 
			const int vi[2] = {p[j], p[nj]};

			for (int k = 0; k < 2; ++k)
			{
				const unsigned short* v = &mesh.verts[vi[k]*3];
				const float x = orig[0] + v[0]*cs;
				const float y = orig[1] + (v[1]+1)*ch + 0.1f;
				const float z = orig[2] + v[2]*cs;
				dd->position(x, y, z);
			}
		}
	}
	dd->end();

	// Draw boundary edges
	const unsigned int colb = duRGBA(0,48,64,220);
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i < mesh.npolys; ++i)
	{
		const unsigned short* p = &mesh.polys[i*nvp*2];
		for (int j = 0; j < nvp; ++j)
		{
			if (p[j] == RC_MESH_NULL_IDX) break;
			if ((p[nvp+j] & 0x8000) == 0) continue;
			const int nj = (j+1 >= nvp || p[j+1] == RC_MESH_NULL_IDX) ? 0 : j+1; 
			const int vi[2] = {p[j], p[nj]};

			unsigned int col = colb;
			if ((p[nvp+j] & 0xf) != 0xf)
				col = duRGBA(255,255,255,128);
			for (int k = 0; k < 2; ++k)
			{
				const unsigned short* v = &mesh.verts[vi[k]*3];
				const float x = orig[0] + v[0]*cs;
				const float y = orig[1] + (v[1]+1)*ch + 0.1f;
				const float z = orig[2] + v[2]*cs;
				dd->position(x, y, z);
			}
		}
	}
	dd->end();

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_POINT_LIST);
	const unsigned int colv = duRGBA(0,0,0,220);
	for (int i = 0; i < mesh.nverts; ++i)
	{
		const unsigned short* v = &mesh.verts[i*3];
		const float x = orig[0] + v[0]*cs;
		const float y = orig[1] + (v[1]+1)*ch + 0.1f;
		const float z = orig[2] + v[2]*cs;
		dd->position(x,y,z);
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawPolyMeshDetail(Ogre::ManualObject * dd, const struct rcPolyMeshDetail& dmesh)
{
	if (!dd) return;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for (int i = 0; i < dmesh.nmeshes; ++i)
	{
		const unsigned int* m = &dmesh.meshes[i*4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &dmesh.verts[bverts*3];
		const unsigned char* tris = &dmesh.tris[btris*4];

		unsigned int color = duIntToCol(i, 192);

		for (int j = 0; j < ntris; ++j)
		{
			//dd->vertex(&verts[tris[j*4+0]*3], color);
			//dd->vertex(&verts[tris[j*4+1]*3], color);
			//dd->vertex(&verts[tris[j*4+2]*3], color);
		}
	}
	dd->end();

	// Internal edges.
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	const unsigned int coli = duRGBA(0,0,0,64);
	for (int i = 0; i < dmesh.nmeshes; ++i)
	{
		const unsigned int* m = &dmesh.meshes[i*4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &dmesh.verts[bverts*3];
		const unsigned char* tris = &dmesh.tris[btris*4];

		for (int j = 0; j < ntris; ++j)
		{
			const unsigned char* t = &tris[j*4];
			for (int k = 0, kp = 2; k < 3; kp=k++)
			{
				unsigned char ef = (t[3] >> (kp*2)) & 0x3;
				if (ef == 0)
				{
					// Internal edge
					if (t[kp] < t[k])
					{
						//dd->vertex(&verts[t[kp]*3], coli);
						//dd->vertex(&verts[t[k]*3], coli);
					}
				}
			}
		}
	}
	dd->end();

	// External edges.
	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	const unsigned int cole = duRGBA(0,0,0,64);
	for (int i = 0; i < dmesh.nmeshes; ++i)
	{
		const unsigned int* m = &dmesh.meshes[i*4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &dmesh.verts[bverts*3];
		const unsigned char* tris = &dmesh.tris[btris*4];

		for (int j = 0; j < ntris; ++j)
		{
			const unsigned char* t = &tris[j*4];
			for (int k = 0, kp = 2; k < 3; kp=k++)
			{
				unsigned char ef = (t[3] >> (kp*2)) & 0x3;
				if (ef != 0)
				{
					// Ext edge
					//dd->vertex(&verts[t[kp]*3], cole);
					//dd->vertex(&verts[t[k]*3], cole);
				}
			}
		}
	}
	dd->end();

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_POINT_LIST);
	const unsigned int colv = duRGBA(0,0,0,64);
	for (int i = 0; i < dmesh.nmeshes; ++i)
	{
		const unsigned int* m = &dmesh.meshes[i*4];
		const unsigned int bverts = m[0];
		const int nverts = (int)m[1];
		const float* verts = &dmesh.verts[bverts*3];
		//for (int j = 0; j < nverts; ++j)
			//dd->vertex(&verts[j*3], colv);
	}
	dd->end();
}

void DebugDrawOgre::duDebugDrawGridXZ(Ogre::ManualObject * dd, const float ox, const float oy, const float oz,
					   const int w, const int h, const float size,
					   const unsigned int col, const float lineWidth)
{
	if (!dd) return;

	dd->begin("mNavMesherDebuggerMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i <= h; ++i)
	{
		dd->position(ox,oy,oz+i*size);
		dd->position(ox+w*size,oy,oz+i*size);
	}
	for (int i = 0; i <= w; ++i)
	{
		dd->position(ox+i*size,oy,oz);
		dd->position(ox+i*size,oy,oz+h*size);
	}
	dd->end();
}

void DebugDrawOgre::getContourCenter(const rcContour* cont, const float* orig, float cs, float ch, float* center)
{
	center[0] = 0;
	center[1] = 0;
	center[2] = 0;
	if (!cont->nverts)
		return;
	for (int i = 0; i < cont->nverts; ++i)
	{
		const int* v = &cont->verts[i*4];
		center[0] += (float)v[0];
		center[1] += (float)v[1];
		center[2] += (float)v[2];
	}
	const float s = 1.0f / cont->nverts;
	center[0] *= s * cs;
	center[1] *= s * ch;
	center[2] *= s * cs;
	center[0] += orig[0];
	center[1] += orig[1] + 4*ch;
	center[2] += orig[2];
}

const rcContour* DebugDrawOgre::findContourFromSet(const rcContourSet& cset, unsigned short reg)
{
	for (int i = 0; i < cset.nconts; ++i)
	{
		if (cset.conts[i].reg == reg)
			return &cset.conts[i];
	}
	return 0;
}