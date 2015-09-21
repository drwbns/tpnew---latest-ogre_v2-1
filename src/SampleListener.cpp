#include "SampleListener.h"

#include "OgreStringConverter.h"
#include "OGRE\Overlay\OgreOverlayManager.h"
#include "OgreSceneNode.h"

#include "Character.h"

inline SampleListener::SampleListener(RenderWindow * win, Camera * cam)

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

//-------------------------------------------------------------------------------------
bool SampleListener::keyPressed( const OIS::KeyEvent &arg )
{
	return true;
}

bool SampleListener::keyReleased( const OIS::KeyEvent &arg )
{
	if (arg.key == OIS::KC_W){
		mChar->setAnimationState("Idle1");
	}
    return true;
}

bool SampleListener::mouseMoved( const OIS::MouseEvent &arg )
{


    return true;
}

bool SampleListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    return true;
}

bool SampleListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    return true;
}

inline void SampleListener::setCharacter(Character * character) {
	mChar = character;
}

inline void SampleListener::updateStats() {
	try {


		// Camera orientation ( psuedo )
		DisplayString debugtext = ("Camera orientation: ("
			+ ((mChar->getYawNode()->getOrientation().y >= 0) ? std::string("+") :
				std::string("-")) + "" + Ogre::StringConverter::toString(Ogre::Math::Floor(2 *
					Ogre::Degree(Ogre::Math::ACos(mChar->getYawNode()->getOrientation().w)).valueDegrees())) + ", " +
			((mChar->getPitchNode()->getOrientation().x >= 0) ? std::string("+") : std::string("-")) + "" +
			Ogre::StringConverter::toString(Ogre::Math::Floor(2 *
				Ogre::Degree(Ogre::Math::ACos(mChar->getPitchNode()->getOrientation().w)).valueDegrees())) + ", " +
			((mChar->getCameraNode()->getOrientation().z >= 0) ? std::string("+") : std::string("-")) + "" +
			Ogre::StringConverter::toString(Ogre::Math::Floor(2 *
				Ogre::Degree(Ogre::Math::ACos(mChar->getCameraNode()->getOrientation().w)).valueDegrees())) + ")");
		guiDbg->setCaption(debugtext);

		// Yaw, pitch and Roll nodes orientations
		guiDbg->setCaption(guiDbg->getCaption() +
			"\n" + "PitchNode Orientation: " + Ogre::StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getOrientation().x).valueDegrees())) + ","
			+ Ogre::StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getOrientation().y).valueDegrees())) + ","
			+ Ogre::StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getOrientation().z).valueDegrees())) + "\n" +

			"\n" + "PitchNode Position: " + Ogre::StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getPosition().x).valueDegrees())) + ","
			+ Ogre::StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getPosition().y).valueDegrees())) + ","
			+ Ogre::StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getPosition().z).valueDegrees())) + "\n"

			+ "YawNode Orientation: " + Ogre::StringConverter::toString(mChar->getYawNode()->getOrientation().x) + ","
			+ Ogre::StringConverter::toString(mChar->getYawNode()->getOrientation().y) + ","
			+ Ogre::StringConverter::toString(mChar->getYawNode()->getOrientation().z) + "\n"
			+ "RollNode Orientation: " + Ogre::StringConverter::toString(mChar->getRollNode()->getOrientation().x) + ","
			+ Ogre::StringConverter::toString(mChar->getRollNode()->getOrientation().y) + ","
			+ Ogre::StringConverter::toString(mChar->getRollNode()->getOrientation().z) + "\n");
		// Set the text

	}
	catch (...) { /* ignore */ }
}

inline bool SampleListener::frameStarted(const FrameEvent & evt)
{
}
