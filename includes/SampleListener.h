#ifndef _SAMPLE_LISTENER_H
#define _SAMPLE_LISTENER_H

#include "prereqs.h"
#include "OgrePrerequisites.h"

#include "OGRE\Overlay\OgreOverlayElement.h"
#include "OGRE\Overlay\OgreOverlayManager.h"

#include "OIS\OISMouse.h"
#include "OIS\OISKeyboard.h"

#include "Character.h"

using namespace Ogre;



class SampleListener : public OIS::MouseListener, public OIS::KeyListener
 {
 protected:
	// References to the main character and the camera
	Character *mChar;

	// Camera mode - Now supports 1st person, 3rd person (chasing) and 3rd person (fixed)
	unsigned int mMode;

	// Whether Cam is locked or not, true at initialization
	int lockedCam;
	float yaw;
	float pitch;

	Real moveScale;
	Real camToPlayer;
	float mMouseSpeed;
	bool fixedCam;
	bool camRotate;
	Real pitchAngle, yawAngle;
	Quaternion qr;

	RenderWindow * win;
	OverlayElement* guiDbg;
	float mTimeUntilNextToggle;
	TextureFilterOptions mFiltering;
	size_t mAniso;
	int mSceneDetailIndex;
 public:
	 SampleListener(RenderWindow* win, Camera* cam)
     
     {
		 this->win = win;
         mChar = 0;
		 // we're initially on mode 0 (3rd person), next cam is far chase cam(mode 1)
		mMouseSpeed = 50;
		fixedCam = true;
		camRotate = false;

	guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
	// Set the debug txt alignment
	guiDbg->setTop(guiDbg->getTop() - 50);
     }

	 // Virtual function declarations for handling mouse and Keyboard events
    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
     void setCharacter (Character *character) {
         mChar = character;
     }
	
	 void updateStats();
 



     bool frameStarted(const FrameEvent& evt)
     {
	 }

 };

#endif