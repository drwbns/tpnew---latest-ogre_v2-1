#ifndef _SAMPLE_LISTENER_H
#define _SAMPLE_LISTENER_H

#include "prereqs.h"
#include "OgrePrerequisites.h"

#include "OGRE\Overlay\OgreOverlayElement.h"


#include "OIS\OISMouse.h"
#include "OIS\OISKeyboard.h"



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
	 SampleListener(RenderWindow* win, Camera* cam);

	 // Virtual function declarations for handling mouse and Keyboard events
    // OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg ) override;
    virtual bool keyReleased( const OIS::KeyEvent &arg ) override;
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg ) override;
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) override;
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) override;
	void setCharacter(Character *character);
	
	 void updateStats() const;


	static bool frameStarted(const FrameEvent& evt);

 };

#endif