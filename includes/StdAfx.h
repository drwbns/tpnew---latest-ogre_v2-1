#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "windows.h"

#include "Ogre.h"
#include "OIS.h"

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>


#include <NxPhysics.h>

//Recast includes
//#include <Recast/Recast.h>
//Detour Includes
//#include <Detour/detournavmesh.h>

//OpenAL Includes
#include <al.h>
#include <alc.h>
#include <alut.h>



//xiph includes
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include <theora.h>
#include <codec.h>


/* includes for the "core" */
#include "Application.h"
#include "GlobalVars.h"

/* core vars regulating the world */
#define GST ((GameState*)SSYS->GetCurrentState())
#define WORLD GST->getWorld()
#define LVL GST->getLevel()
#define MOVING(id) GST->getWorld()->getAgent(id)
#define AGENT(id) ((Agent*)GST->getWorld()->getAgent(id))
#define PLAYERID GST->getWorld()->getPlayer()
#define PLAYER ((Agent*)GST->getWorld()->getPlayerAgent())
#define TPCAM ((TpCamController*)GST->GetCurrentCam())
#define CAM GST->GetCurrentCam()->GetCamera()
#define PTF GST->getLevel()