/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include "OgreSingleton.h"
#include "OgreFrameListener.h"
#include "OgreWindowEventUtilities.h"


#include "OIS\OISMouse.h"
#include "OIS\OISKeyboard.h"

class OgreCharacter;

#include "Overlay\OgreOverlayElement.h"

#define BASE BaseApplication::getSingletonPtr()

using namespace Ogre;

class BaseApplication : public Ogre::Singleton<BaseApplication>, public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
    BaseApplication(void);
    virtual ~BaseApplication(void);

	static BaseApplication& getSingleton();
	static BaseApplication* getSingletonPtr();

	RenderWindow* getWindow() { return mWindow; };
	OIS::Mouse * getMouse() { return mMouse; };
	OIS::Keyboard* getKeyboard() { return mKeyboard; }
	Camera* getCamera() { return mCamera; }
	SceneManager* getSceneMgr() { return mSceneMgr; }
	Root* getRoot() { return mRoot; } 
	OIS::InputManager* getInputMgr() { return mInputManager; } 

	OgreCharacter * getCharacter() { return mChar; }

    virtual void go(void);

		

protected:
     bool setup();
     bool configure(void);
     void chooseSceneManager(void);
     void createCamera(void);
     void createFrameListener(void);
    virtual void createScene(void) = 0; // Override me!
    virtual void destroyScene(void);
    virtual void createViewports(void);
    virtual void setupResources(void);
    virtual void createResourceListener(void);
    virtual void loadResources(void);

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    // Ogre::WindowEventListener
    //Adjust mouse clipping area
    virtual void windowResized(Ogre::RenderWindow* rw);
    //Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(Ogre::RenderWindow* rw);

    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

    bool mCursorWasVisible;                    // was cursor visible before dialog appeared
    bool mShutDown;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

	// References to the main character and the camera
	OgreCharacter *mChar;

	// Camera mode - Now supports 1st person, 3rd person (chasing) and 3rd person (fixed)
	unsigned int mMode;

	// Whether Cam is locked or not, true at initialization
	int lockedCam;
	float yaw;
	float pitch;

	Real moveScale;
	Real camToPlayer;
	int mMouseSpeed, mRotateSpeed, mZoomSpeed;
	bool fixedCam, fixedYaw, fixedPitch;
	bool camRotate;
	bool yawHasBeenReset;
	Real pitchAngle, yawAngle;

	Ogre::OverlayElement* guiDbg;
	float mTimeUntilNextToggle;
	TextureFilterOptions mFiltering;
	size_t mAniso;
	int mSceneDetailIndex;
};

#endif // #ifndef __BaseApplication_h_
