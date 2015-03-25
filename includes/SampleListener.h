#ifndef _SAMPLE_LISTENER_H
#define _SAMPLE_LISTENER_H

#include "Ogre.h"
using namespace Ogre;
#include <OIS/OIS.h>

#include "Character.h"
#include "BaseApplication.h"

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
	
	 void updateStats() {
		 try {
			 
			 
			 // Camera orientation ( psuedo )
			DisplayString debugtext = ("Camera orientation: (" 
				+ ( ( mChar->getYawNode()->getOrientation().y >= 0) ? std::string("+") : 
		 std::string("-")) + "" + Ogre::StringConverter::toString( Ogre::Math::Floor( 2 * 
		 Ogre::Degree( Ogre::Math::ACos( mChar->getYawNode()->getOrientation().w ) ).valueDegrees() ) ) + ", " + 
		 ( ( mChar->getPitchNode()->getOrientation().x >= 0 ) ? std::string("+") : std::string("-")) + "" + 
		 Ogre::StringConverter::toString( Ogre::Math::Floor( 2 * 
		 Ogre::Degree( Ogre::Math::ACos(mChar->getPitchNode()->getOrientation().w ) ).valueDegrees() ) ) + ", " + 
		 ( ( mChar->getCameraNode()->getOrientation().z >= 0 ) ? std::string("+") : std::string("-")) + "" + 
		 Ogre::StringConverter::toString( Ogre::Math::Floor(2 * 
		 Ogre::Degree( Ogre::Math::ACos( mChar->getCameraNode()->getOrientation().w)).valueDegrees() ) ) + ")");
		guiDbg->setCaption(debugtext);

			// Yaw, pitch and Roll nodes orientations
		guiDbg->setCaption( guiDbg->getCaption() +
			"\n" + "PitchNode Orientation: " + Ogre::StringConverter::toString( Math::Floor( 100 * Degree( mChar->getPitchNode()->getOrientation().x ).valueDegrees() ) ) + ","
				 + Ogre::StringConverter::toString( Math::Floor( 100 * Degree(mChar->getPitchNode()->getOrientation().y ).valueDegrees() ) ) + ","
				  + Ogre::StringConverter::toString( Math::Floor( 100 * Degree(mChar->getPitchNode()->getOrientation().z ).valueDegrees() ) ) + "\n" +

				  "\n" + "PitchNode Position: " + Ogre::StringConverter::toString( Math::Floor( 100 * Degree( mChar->getPitchNode()->getPosition().x ).valueDegrees() ) ) + ","
				 + Ogre::StringConverter::toString( Math::Floor( 100 * Degree(mChar->getPitchNode()->getPosition().y ).valueDegrees() ) ) + ","
				  + Ogre::StringConverter::toString( Math::Floor( 100 * Degree(mChar->getPitchNode()->getPosition().z ).valueDegrees() ) ) + "\n"

				   + "YawNode Orientation: " + Ogre::StringConverter::toString( mChar->getYawNode()->getOrientation().x ) + ","
				 + Ogre::StringConverter::toString( mChar->getYawNode()->getOrientation().y ) + ","
				  + Ogre::StringConverter::toString( mChar->getYawNode()->getOrientation().z ) + "\n"
				   + "RollNode Orientation: " + Ogre::StringConverter::toString( mChar->getRollNode()->getOrientation().x ) + ","
				 + Ogre::StringConverter::toString( mChar->getRollNode()->getOrientation().y ) + ","
				  + Ogre::StringConverter::toString( mChar->getRollNode()->getOrientation().z ) + "\n" );
		 // Set the text
				  
		 }
		 catch(...) { /* ignore */ }
	 }
 



     bool frameStarted(const FrameEvent& evt)
     {
	 }

 };

#endif