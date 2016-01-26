#include "SampleListener.h"

#include "OgreStringConverter.h"
#include "OGRE\Overlay\OgreOverlayManager.h"
#include "OgreSceneNode.h"

#include "Character.h"

SampleListener::SampleListener(RenderWindow * win, Camera *) : mMode(0), lockedCam(0), yaw(0), pitch(0),
moveScale(0), camToPlayer(0), pitchAngle(0), yawAngle(0), mTimeUntilNextToggle(0), mFiltering(), mAniso(0), mSceneDetailIndex(0)

{
	this->win = win;
	mChar = nullptr;
	// we're initially on mode 0 (3rd person), next cam is far chase cam(mode 1)
	mMouseSpeed = 50;
	fixedCam = true;
	camRotate = false;

	guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
	// Set the debug txt alignment
	guiDbg->setTop(guiDbg->getTop() - 50);
}

//-------------------------------------------------------------------------------------
bool SampleListener::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

bool SampleListener::keyReleased(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_W) {
		mChar->setAnimationState("Idle1");
	}
	return true;
}

bool SampleListener::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool SampleListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

bool SampleListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	return true;
}

void SampleListener::setCharacter(Character * character) {
	mChar = character;
}

void SampleListener::updateStats() const
{
	try {
		// Camera orientation ( psuedo )
		DisplayString debugtext = ("Camera orientation: ("
			+ ((mChar->getYawNode()->getOrientation().y >= 0) ? std::string("+") :
				std::string("-")) + "" + StringConverter::toString(Math::Floor(2 *
					Degree(Math::ACos(mChar->getYawNode()->getOrientation().w)).valueDegrees())) + ", " +
			((mChar->getPitchNode()->getOrientation().x >= 0) ? std::string("+") : std::string("-")) + "" +
			StringConverter::toString(Math::Floor(2 *
				Degree(Math::ACos(mChar->getPitchNode()->getOrientation().w)).valueDegrees())) + ", " +
			((mChar->getCameraNode()->getOrientation().z >= 0) ? std::string("+") : std::string("-")) + "" +
			StringConverter::toString(Math::Floor(2 *
				Degree(Math::ACos(mChar->getCameraNode()->getOrientation().w)).valueDegrees())) + ")");
		guiDbg->setCaption(debugtext);

		// Yaw, pitch and Roll nodes orientations
		guiDbg->setCaption(guiDbg->getCaption() +
			"\n" + "PitchNode Orientation: " + StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getOrientation().x).valueDegrees())) + ","
			+ StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getOrientation().y).valueDegrees())) + ","
			+ StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getOrientation().z).valueDegrees())) + "\n" +

			"\n" + "PitchNode Position: " + StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getPosition().x).valueDegrees())) + ","
			+ StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getPosition().y).valueDegrees())) + ","
			+ StringConverter::toString(Math::Floor(100 * Degree(mChar->getPitchNode()->getPosition().z).valueDegrees())) + "\n"

			+ "YawNode Orientation: " + StringConverter::toString(mChar->getYawNode()->getOrientation().x) + ","
			+ StringConverter::toString(mChar->getYawNode()->getOrientation().y) + ","
			+ StringConverter::toString(mChar->getYawNode()->getOrientation().z) + "\n"
			+ "RollNode Orientation: " + StringConverter::toString(mChar->getRollNode()->getOrientation().x) + ","
			+ StringConverter::toString(mChar->getRollNode()->getOrientation().y) + ","
			+ StringConverter::toString(mChar->getRollNode()->getOrientation().z) + "\n");
		// Set the text
	}
	catch (...) { /* ignore */ }
}

bool SampleListener::frameStarted(const FrameEvent & evt)
{
	return true;
}