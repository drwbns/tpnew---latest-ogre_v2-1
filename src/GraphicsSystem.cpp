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


#include "GraphicsSystem.h"

#include "StateSystem.h"
#include "GameState.h"
#include "SharedData.h"
#include "Application.h"

#include "OgreConfigFile.h"
#include "OgreTextureManager.h"
#include "OgreViewport.h"
#include "OgreLogManager.h"



using namespace Ogre;

template<> GraphicsSystem* Ogre::Singleton<GraphicsSystem>::msSingleton = 0;

GraphicsSystem* GraphicsSystem::getSingletonPtr(void)
{
	return msSingleton;
}

GraphicsSystem& GraphicsSystem::getSingleton(void)
{  
	assert( msSingleton );  return ( *msSingleton );
}

GraphicsSystem::GraphicsSystem() : mRoot(NULL), mWindow(NULL), mSceneMgr(NULL), mCamera(NULL)
{
}

GraphicsSystem::~GraphicsSystem()
{
	Finalize();
}

void GraphicsSystem::Initialize()
{
	//init root
	mRoot = new Ogre::Root();

	//add resource locations
	Ogre::ConfigFile cf;
	cf.load("resources.cfg");
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName, typeName, archName;


	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "TutorialApplication Render Window");
    }
    else
	{
		//init core
		Ogre::RenderSystem* mRenderSys = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
		mRenderSys->setConfigOption("Video Mode", "1024 x 768 @ 32-bit colour");
		//mRenderSys->setConfigOption("Anti aliasing", "No");
		mRenderSys->setConfigOption("VSync", "No");
		mRenderSys->setConfigOption("Full Screen", "No");
		mRenderSys->setConfigOption("Floating-point mode", "Fastest");
		mRenderSys->setConfigOption("Allow NVPerfHUD", "No");
		mRoot->setRenderSystem(mRenderSys);
		mWindow = mRoot->initialise(true,"Whatever");
	}
	mOverlaySystem = OGRE_NEW(OverlaySystem);
	mOverlayMgr = OverlayManager::getSingletonPtr();
	//init resources
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	//create scene manager
	mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);

	//create camera
	mCamera = mSceneMgr->createCamera("MainCamera");
	mCamera->setNearClipDistance(0.1f);
	mCamera->setFarClipDistance(2048);
	mCamera->setAutoAspectRatio(true);
	mCamera->setPosition(Ogre::Vector3(0,1,0));
	mCamera->lookAt(Ogre::Vector3(0,1,1));

	//create view ports
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	//set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	//set ambient light & skybox
	mSceneMgr->setAmbientLight(ColourValue(1.0, 1.0, 1.0));

	Ogre::StringVectorPtr missions = 
	Ogre::ResourceGroupManager::getSingleton().findResourceNames("General", "*.material");

	for (StringVector::iterator i = missions->begin(); i != missions->end(); ++i)
	{
		LogManager::getSingleton().logMessage("TEST: " + *i);
	}

	// Shared data singleton initialization - used as as a global accessor to config variables
	new SharedData();
	SharedData::getSingleton().iGlobalActivate = true;
	SharedData::getSingleton().iCamera = mCamera;
	SharedData::getSingleton().iSceneMgr = mSceneMgr;
	SharedData::getSingleton().iRoot = mRoot;
	SharedData::getSingleton().iWindow = mWindow;
}

void GraphicsSystem::Start()
{
	//register as a window Listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	//register a rendersystem listener
	//Ogre::Root::getSingleton().getRenderSystem()->addListener(this);

	mRoot->addFrameListener(APP);

	//start render
	mRoot->startRendering();
}

void GraphicsSystem::Finalize()
{
	//remove listeners
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	//Ogre::Root::getSingleton().getRenderSystem()->removeListener(this);
	mRoot->removeFrameListener(APP);

	mWindow->removeViewport(0);
	mSceneMgr->destroyCamera(mCamera);
	mRoot->destroySceneManager(mSceneMgr);

	mRoot->shutdown();
	//delete mRoot;
	//delete mWindow;
}

void GraphicsSystem::windowResized(Ogre::RenderWindow* rw)
{
}

void GraphicsSystem::windowClosed(Ogre::RenderWindow* rw)
{
}

void GraphicsSystem::windowFocusChange(Ogre::RenderWindow* rw)
{
}

void GraphicsSystem::eventOccurred(const Ogre::String& eventName, const Ogre::NameValuePairList* parameters)
{
}