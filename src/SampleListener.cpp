#include "SampleListener.h"

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
