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

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#include "Sample.h"
#include "InputGeom.h"

//#include "RecastDebugDraw.h"
//#include "DetourDebugDraw.h"

#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshQuery.h"

#include "DebugDrawOgre.h"

//#include "DetourCrowd.h"
//#include "imgui.h"
//#include "SDL.h"
//#include "SDL_opengl.h"

#ifdef WIN32
#	define snprintf _snprintf
#endif

Sample::Sample() :
	m_navMesh(nullptr),
	m_navQuery(nullptr),
	//m_crowd(0),
	m_navMeshDrawFlags(DU_DRAWNAVMESH_OFFMESHCONS | DU_DRAWNAVMESH_CLOSEDLIST),
	m_tool(nullptr),
	m_ctx(nullptr)
{
	resetCommonSettings();
	m_navQuery = dtAllocNavMeshQuery();
	//	m_crowd = dtAllocCrowd();
}

Sample::~Sample()
{
	dtFreeNavMeshQuery(m_navQuery);
	dtFreeNavMesh(m_navMesh);
	//	dtFreeCrowd(m_crowd);
	delete m_tool;
}

void Sample::setTool(SampleTool* tool)
{
	delete m_tool;
	m_tool = tool;
	if (tool)
		m_tool->init(this);
}

void Sample::resetCommonSettings()
{
	m_cellSize = 0.3f;
	m_cellHeight = 0.2f;
	m_agentHeight = 2.0f;
	m_agentRadius = 0.6f;
	m_agentMaxClimb = 0.9f;
	m_agentMaxSlope = 45.0f;
	m_regionMinSize = 8;
	m_regionMergeSize = 20;
	m_monotonePartitioning = false;
	m_edgeMaxLen = 12.0f;
	m_edgeMaxError = 1.3f;
	m_vertsPerPoly = 6.0f;
	m_detailSampleDist = 6.0f;
	m_detailSampleMaxError = 1.0f;
}

void Sample::handleCommonSettings()
{
}

/*
void Sample::handleClick(const float* s, const float* p, bool shift)
{
	if (m_tool)
		m_tool->handleClick(s, p, shift);
}

void Sample::handleToggle()
{
	if (m_tool)
		m_tool->handleToggle();
}

void Sample::handleStep()
{
	if (m_tool)
		m_tool->handleStep();
}
*/
bool Sample::handleBuild()
{
	return true;
}

const float* Sample::getBoundsMin()
{
	if (!INPT) return nullptr;
	return INPT->getMeshBoundsMin();
}

const float* Sample::getBoundsMax()
{
	if (!INPT) return nullptr;
	return INPT->getMeshBoundsMax();
}

void Sample::handleUpdate()
{
	/*
	if (m_tool)
		m_tool->handleUpdate(dt);
		*/
}